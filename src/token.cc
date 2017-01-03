/*

Parser.

Copyright (C) 2013-2017 Mats G. Liljegren

This file is part of Sisdel.

Sisdel is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation, version 3.

Sisdel is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not see
<http://www.gnu.org/licenses/>.

*/

//////////////////////////////////////////////////////////////////////////////
// token_t
/////////////////////////////////////////////////////////////////////////////

#include <cmath>

#include "token.hh"
#include "hash.hh"
#include "mmap_file.hh"
#include "string.h"

tokenizer_t::tokenizer_t(environment_t& env, const char *file)
	: m_env(env), m_file(env, env.sbucket().find_add(file)),
	  m_startofline(m_file.get_position())
{
}

static bool valid_digit(char ch, char base)
{
	if (base > 10)
		return ((ch >= '0') && (ch <= '9'))
			|| ((ch >= 'a') && (ch <= ('a' + base - 10)));
	else
		return (ch >= '0') && (ch <= ('9' - base));
}

uint64_t tokenizer_t::get_number(char base, unsigned divisor_step, const char *valid_digits, size_t& nr_digits, const position_t& token_start)
{
	uint64_t nr = 0;
	bool divisor_present = false;
	unsigned divisor_steps_left = divisor_step;
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
			if (divisor_steps_left == 0)
				throw parser_error(m_env, token_start,
						   m_file.get_position(),
						   "Expected ' divisor here");

			else
				divisor_steps_left--;
			nr_digits++;
		} else if (ch == '\'') {
			if (divisor_present && (divisor_steps_left != 0))
				throw parser_error(m_env, token_start,
						   m_file.get_position(),
						   "Did not expect ' divisor here");
			divisor_steps_left = divisor_step;
		} else {
			break;
		}

		m_file.skip();
	}

	if (divisor_present && (divisor_steps_left != 0))
		throw parser_error(m_env, token_start, m_file.get_position(),
				   "Did not expect ' divisor here");

	return nr;
}


const token_t tokenizer_t::next(void)
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

			// Remember line start for diagnostic messages
			m_startofline = m_file.get_position();

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
			token_t token(token_t::eol, m_startofline, nr_tabs);
			return token;
		}

		if (valid_digit(ch, 10)) {
			// Handle numbers

			const position_t start_of_number =
				m_file.get_position();

			// Determine base
			const char *valid_digits;
			unsigned divisor_step;
			char base;
			if (ch == '0')
				m_file.skip();
			switch (m_file.peek()) {
			case 'b':
				base = 2;
				divisor_step = 4;
				valid_digits = "01";
				m_file.skip();
				break;
			case 'o':
				base = 8;
				divisor_step = 4;
				valid_digits = "01234567";
				m_file.skip();
				break;
			case 'x':
				base = 16;
				divisor_step = 4;
				valid_digits = "0123456789abcdefABCDEF";
				m_file.skip();
				break;
			default:
				base = 10;
				divisor_step = 3;
				valid_digits = "0123456789";
				break;
			}

			size_t nr_digits;
			uint64_t integer = get_number(base, divisor_step, valid_digits, nr_digits, start_of_number);

			if (m_file.peek() == '.') {
				m_file.skip();
				uint64_t decimals = get_number(base, divisor_step, valid_digits, nr_digits, start_of_number);
				double d = (double) integer + (decimals / pow(base, nr_digits));

				// Ensure there's no trailing garbage
				if (strchr(TOKEN_SEPARATORS, m_file.peek()) == NULL)
					throw parser_error(
						m_env, start_of_number,
						m_file.get_position(),
						"Trailing garbage after string constant");

				// Return floating token
				token_t token(token_t::floating,
					      start_of_number,
					      d);
				return token;
			}
				
			// Ensure there's no trailing garbage
			if (strchr(TOKEN_SEPARATORS, m_file.peek()) == NULL)
				throw parser_error(
						m_env, start_of_number,
						m_file.get_position(),
						"Trailing garbage after string constant");


			// Return integer token
			token_t token(token_t::integer, start_of_number,
				      integer);
			return token;
		}

		if (ch == '"') {
			// Handle strings

			// Get start position for the string
			const position_t string_start = m_file.get_position();

			// Skip past current '"'
			m_file.skip();

			const char * const start_of_content = m_file.str();
			
			// Skip until matching '"'
			hash_t hash;
			const size_t size = m_file.skip_until_hashed('"', hash);

			// Create a string index from the string
			const sbucket_idx_t idx = m_env.sbucket().find_add_hashed(start_of_content, size, hash);
			
			// Skip the matching '"'
			m_file.skip();

			// Ensure there's no trailing garbage
			if (strchr(TOKEN_SEPARATORS, m_file.peek()) == NULL)
				throw parser_error(
					m_env, string_start,
					m_file.get_position(),
					"Trailing garbage after string constant");

			// Return string token
			token_t token(token_t::string, string_start, idx);
			return token;
		}

		// If a comment, skip the rest of the line
		if (ch == '#') {
			m_file.skip_until('\n');
			continue;
		}
		
		// Anything else becomes a name of an identifier
		const position_t identifier_start = m_file.get_position();
		const char * const start_of_content = m_file.str();
		
		// Skip until invalid character or token separator
		hash_t hash;
		const size_t size = m_file.skip_until_hashed(
			IDENTIFIER_INVALID_CHARS, hash);

		if (size == 0)
			throw parser_error(
				m_env, identifier_start, m_file.get_position(),
				"Invalid identifier name");
		
		if (strchr(TOKEN_SEPARATORS, m_file.peek()) == NULL)
			throw parser_error(
				m_env, identifier_start, m_file.get_position(),
				"Invalid identifier name");

		// Create a string index from the identifier name
		const sbucket_idx_t idx = m_env.sbucket().find_add_hashed(
			start_of_content, size, hash);
		
		// Return identifier token
		token_t token(token_t::identifier, identifier_start, idx);
		return token;
	}

	// Return end-of-file token
	token_t token(token_t::eof, m_file.get_position());
	return token;
}
