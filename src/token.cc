/*

Parser.

Copyright (C) 2013 Mats G. Liljegren

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


#include "token.hh"
#include "hash.h"
#include "cctype.hh"

/**
 * Token syntax
 *
 * This token scanner adheres to the syntax as described below.
 * The file being scanned consists of tokens separated by either blank,
 * tab, or carriage return.
 *
 * Line feed has a special meaning for the parser, so it is returned as an
 * identifier token. There is an exception however: if the first token on
 * the line following the line feed is three dots, then this measns that
 * the line feed should be ignored.
 *
 * For example, the following produces a line feed token between foo and bar:
 *
 * foo
 * bar
 *
 * But in this example, only foo and bar will be tokens:
 *
 * foo
 * ... bar
 *
 * This therefore becomes equivalent with:
 *
 * foo bar
 *
 * Tokens can have either of following types:
 * - Identifier   This can be line-feed or any word consisting of any valid
 *                character.
 * - Integer      An integer value with optinally a unit name.
 * - Float        A floating point value with optionally a unit name.
 * - String       A string with optionally a unit name.
 *
 * The token might also be an error token. The above will still be filled in,
 * but a check for error will indicate that the token wasn't parsed correctly.
 */

tokenizer_t::tokenizer_t(environment_t& env, shared_ptr<mmap_file_t> file)
	: m_env(env), m_file(file), m_token(-1, 1, 1),
	  m_currp(file.buf()), m_bytes_left(file.buf_size())
{
}

void next_char()
{
	if (*m_currp == '\n') {
		m_line++;
		m_column = 1;
	} else {
		m_column++;
	}

	m_bytes_left--;
	m_currp++;
}

void tokenizer_t::skip_whitespace()
{
	bool in_comment = false;

	while (m_bytes_left) {
		// Skip comments
		if (*m_currp == '#') {
			next_char();
			while (m_bytes_left) {
				if (skip("\n"))
					break;
				next_char();
			}
			continue;
		}

		// Skip whitespace characters
		if (skip(" \t\r"))
			continue;

		// Skip linefeed if followed by line continuation
		if (*m_currp == '\n') {
			size_t old_column = m_column;
			m_column = 1;
			skip(" \t\r");
			if ((m_bytes_left >= 3)
			    && (m_currp[0] == m_currp[1] == m_currp[2] == '.')) {
				m_bytes_left -= 3;
				m_currp += 3;
				m_line++;
				continue;
			}
			else {
				m_column += (old_column - 1);
			}
		}
		break;
	}
}

bool tokenizer_t::skip(const char *list)
{
	bool found = false;

	while (m_bytes_left) {
		unsigned i;

		for (i = 0; list[i] != '\0'; i++) {
			if (*m_currp == list[i]) {
				found = true;
				next_char();
				break;
			}
		}
		if (list[i] == '\0')
			return found;
	}

	return found;
}

intmax_t get_value(std::error_code& ec)
{
	uintmax_t value = 0;
	bool parser_error = false;
	while (m_bytes_left && isdigit(*m_currp)) {
		const uintmax_t old_value = value;
		value *= radix;
		value += *m_currp - '0';
		if (value < old_value) {
			// TODO: Report overflow
			parser_error = true;
		}
		next_char();
	}

	if (parser_error)
		ec = error::overflow_error;

	return value;
}

sbucket_idx_t get_identifier(std::error_code& ec)
{
	const char * const str = m_currp;
	hasher_t hash;

	while (m_bytes_left && (std::strchr(" \t\r\n#", *m_currp) != NULL)) {
		hash._next(m_currp;
		next_char();
	}

	return m_env.sbucket().find_add_hashed(str, m_currp - str, hash);
}

token_t& tokenizer_t::next()
{
	std::error_code ec = 0;

	skip_whitespace();
	
	// Check for line feed
	if (*m_currp == '\n') {
		m_token.m_line = m_line;
		m_token.m_column = m_column;
		while (skip("\n") || skip_whitespace());
		return m_token.set_identifier(token_t::eol, ec);
	}

	// We have something, mark this position
	m_token.m_line = m_line;
	m_token.m_column = m_column;	
		
	// Check for string
	if (*m_currp == '"') {
		next_char();
		const char *str = m_currp;
		hasher_t hash;
		size_t nr_chars = 0;
		while (m_currp != '"') {
			if (!m_bytes_left--) {
				ec = error::unexpected_eof;
				return m_token.set_string_constant("", ec);
			}
			hash.next(m_currp++);
			nr_chars++;
		}
		const sbucket_idx_t unit = get_identifier(ec);		
		return m_token.set_string_constant(
			env.sbucket().find_add_hashed(
				str, nr_chars, hash), unit, ec);
	}		

	// Check for number
	if (isdigit(*m_currp) || ((*m_currp == '-') && (m_bytes_left > 1)
				  && isdigit(*m_currp))) {
		unsigned radix = 10;
		const int signedness = 1;

		if (*m_currp == '-') {
			signedness = -1;
			next_char();
		}

		if ((m_bytes_left > 1) && (m_currp[0] == '0')) {
			if ((m_currp[1] == 'x') || (m_currp[1] == 'X')) {
				radix = 16;
				next_char(2);
			}
			else if ((m_currp[1] == 'b') || (m_currp[1] == 'B')) {
				radix = 2;
				next_char(2);
			}
			else if (isdigit(m_currp[2])) {
				radix = 8;
				next_char();
			}
		}

		intmax_t value;

		value = get_value(ec);
		value *= signedness;
		
		// Check if float
		if (*m_currp == '.') {
			m_token.m_type = token_t::floating_point;
			double dvalue = value;

			if ((uintmax_t) dvalue != value)
				ec = error::overflow_error;

			next_char();
			uintmax_t divisor = radix;
			while (m_bytes_left && isdigit(*m_currp)) {
				const double old_dvalue = dvalue;
				dvalue += (double) (*m_currp - '0') / divisor;
				divisor *= radix;
				next_char();
			}

			const sbucket_idx_t unit = get_identifier(ec);
			return m_token.set_float_constant(dvalue, unit, ec);
		}

		const sbucket_idx_t unit = get_identifier(ec);		
		return m_token.set_integer_constant(value, unit, ec);
	}

	// If nothing else, then this is an identifier
        const sbucket_idx_t idx = get_identifier(ec);
	return m_token.set_identifier(idx, ec);
}
