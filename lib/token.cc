/*
  Implements the scanner (tokenizier_t).

  SPDX-License-Identifier: MIT

*/

//////////////////////////////////////////////////////////////////////////////
// token_t
/////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <ostream>
#include <string.h>
#include <boost/core/demangle.hpp>

#include "token.hh"
#include "hash.hh"
#include "mmap_file.hh"
#include "string.h"

#define TOKEN_SEPARATORS         "\n\r\t "
#define SINGLE_LETTER_IDENTIFIERS "()[]\{}"
#define IDENTIFIER_INVALID_CHARS "\"'#" TOKEN_SEPARATORS

#ifndef NDEBUG

long long value_of_int(mp_int value)
{
	return (long long)value;
}

long double value_of_float(mp_float value)
{
	return (long double)value;
}

#endif // NDEBUG

tokenizer_t::tokenizer_t(environment_t& env, const char *file)
	: m_env(env), m_file(env, file),
	  m_startofline(m_file.get_position())
{
}

static bool valid_digit(char ch, char base)
{
	if (base > 10)
		return ((ch >= '0') && (ch <= '9'))
			|| ((ch >= 'a') && (ch <= ('a' + base - 11)));
	else
		return (ch >= '0') && (ch <= ('0' + base - 1));
}

void tokenizer_t::get_number(mp_int& nr, char base,
			     const char *valid_digits, size_t& nr_digits)
{
	char ch;

	nr_digits = 0;
	while (!m_file.eof()) {
		ch = m_file.peek();
		
		if (strchr(valid_digits, ch) != NULL) {
			if ((ch >= '0') && (ch <= '9')) {
				nr *= base;
				nr += (ch - '0');
			} else if ((ch >= 'a') && (ch <= 'f')) {
				nr *= base;
				nr += (ch - 'a' + 10);
			} else if ((ch >= 'A') && (ch <= 'F')) {
				nr *= base;
				nr += (ch - 'A' + 10);
			}
			nr_digits++;
		} else {
			break;
		}

		m_file.skip();
	}
}

const token_t* tokenizer_t::next(void)
{
	for (;;) {
		if (m_file.eof())
			break;

		// Skip carriage-return and space
		m_file.skip("\r ");

		// Check first character to determine token type
		char ch = m_file.peek();

		// Handle new-line
		if (ch == '\n') {
			// Handle multiple new-lines
			(void) m_file.skip("\r\n");

			// Determine indentation level, which is number of
			// tab characters
			const size_t nr_tabs = m_file.skip('\t');

			// Remove carriage-returns and spaces
			m_file.skip("\r ");

			ch = m_file.peek();
			
			// If line was empty, start all over again
			if (ch == '\n')
				continue;

			// Handle comment
			if (ch == '#') {
				m_file.skip_until('\n');
				continue;
			}

			// Return new-line token
			return new token_eol_t(m_file.get_position(), nr_tabs);
		}

		if (valid_digit(ch, 10)) {
			// Handle numbers

			const position_t start_of_number =
				m_file.get_position();

			// Determine base
			const char *valid_digits;
			char base;
			if (ch == '0')
				m_file.skip();
			switch (m_file.peek()) {
			case 'b':
				base = 2;
				valid_digits = "01";
				m_file.skip();
				break;
			case 'o':
				base = 8;
				valid_digits = "01234567";
				m_file.skip();
				break;
			case 'x':
				base = 16;
				valid_digits = "0123456789abcdefABCDEF";
				m_file.skip();
				break;
			default:
				base = 10;
				valid_digits = "0123456789";
				break;
			}

			size_t nr_digits;
			mp_int integer(0);
			get_number(integer, base, valid_digits, nr_digits);

			if (m_file.peek() == '.') {
				size_t nr_decimals;
				m_file.skip();
				mp_int decimals(0);
				get_number(decimals, base, valid_digits,
					   nr_decimals);
				mp_float d;
				d = (mp_float)decimals / pow((mp_float) base, (mp_float) nr_digits);
				d += integer;
				d.precision(((nr_digits + nr_decimals) * base) / 10);

				// Ensure there's no trailing garbage
				if (strchr(TOKEN_SEPARATORS, m_file.peek()) == NULL)
					throw parser_error(
						start_of_number,
						m_file.get_position(),
						"Trailing garbage after float constant");

				// Return floating token
				return new token_float_t(
						start_of_number, d);
			}
				
			// Ensure there's no trailing garbage
			if (strchr(TOKEN_SEPARATORS, m_file.peek()) == NULL)
				throw parser_error(
						start_of_number,
						m_file.get_position(),
						"Trailing garbage after integer constant");


			// Return integer token
			return new token_integer_t(
					start_of_number, integer);
		}

		if (ch == '"') {
			// Handle strings

			// Get start position for the string
			const position_t string_start = m_file.get_position();

			// Skip past current '"'
			m_file.skip();

			// Remember start of string
			m_file.marker_start();
			
			// Skip until matching '"'
			hash_t hash;
			m_file.skip_until_hashed('"', hash);

			// Create a string index from the string
			const string_idx_t idx = m_env.sbucket().find_add_hashed(m_file.marker_end(), hash);
			
			// Skip the matching '"'
			m_file.skip();

			// Ensure there's no trailing garbage
			if (strchr(TOKEN_SEPARATORS, m_file.peek()) == NULL)
				throw parser_error(
					string_start,
					m_file.get_position(),
					"Trailing garbage after string constant");

			// Return string token
			return new token_string_t(string_start, idx);
		}

		// If a comment, skip the rest of the line
		if (ch == '#') {
			m_file.skip_until('\n');
			continue;
		}

		// Check for single letter identifiers
		bool is_single_letter_identifier = false;
		if (strchr(SINGLE_LETTER_IDENTIFIERS, ch) != NULL)
			is_single_letter_identifier = true;
		
		// Anything else becomes a name of an identifier
		const position_t identifier_start = m_file.get_position();
		m_file.marker_start();
		
		// Skip until invalid character or token separator
		hash_t hash;
		const size_t size = m_file.skip_until_hashed(
			IDENTIFIER_INVALID_CHARS, hash);

		if (size == 0)
			throw parser_error(
				identifier_start, m_file.get_position(),
				"Invalid identifier name");
		
		if (strchr(TOKEN_SEPARATORS, m_file.peek()) == NULL)
			throw parser_error(
				identifier_start, m_file.get_position(),
				"Invalid identifier name");

		// Check that if this is supposed to be a single letter
		// identifier, it doesn't consist of multiple letters
		if (is_single_letter_identifier && (size > 1))
			throw parser_error(
				identifier_start, m_file.get_position(),
				"Invalid identifier name");

		// Create a string index from the identifier name
		const string_idx_t idx = m_env.sbucket().find_add_hashed(
			m_file.marker_end(), hash);
		
		// Return identifier token
		return new token_identifier_t(identifier_start, idx);
	}

	// No more input
	return NULL;
}

std::ostream& operator<<(std::ostream& os, const token_t& t)
{
	const std::type_info& ti = typeid(t);
	os << boost::core::demangle(ti.name()) << '(';
	if (ti == typeid(token_eol_t)) {
		os << "indent: "
		   << dynamic_cast<const token_eol_t&>(t).indent_level();
	} else if (ti == typeid(token_integer_t)) {
		os << dynamic_cast<const token_integer_t&>(t).value();
	} else if (ti == typeid(token_float_t)) {
		const token_float_t& tf(dynamic_cast<const token_float_t&>(t));
		os.precision(tf.value().precision());
		os << tf.value() << ':' << tf.value().precision();
	} else if (ti == typeid(token_string_t)) {
		os << dynamic_cast<const token_string_t&>(t).string();
	} else if (ti == typeid(token_identifier_t)) {
		os << dynamic_cast<const token_identifier_t&>(t).name();
	}

	os << ')';
	
	return os;
}
