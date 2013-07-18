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
#include "hash.hh"

#include <cstring>

bool token_t::operator==(const token_t& rhs) const
{
	if ((m_type != rhs.m_type) || (m_error != rhs.m_error)
	    || (m_line != rhs.m_line) || (m_column != rhs.m_column)
	    || (m_idx != rhs.m_idx))
		return false;

	switch (m_type) {
	case type_t::end_of_file:
	case type_t::end_of_line:
		break;
	case type_t::identifier:
		if (m_idx != rhs.m_idx)
			return false;
		break;
	case type_t::integer_constant:
		if ((m_int != rhs.m_int) || (m_idx != rhs.m_idx))
			return false;
		break;
	case type_t::float_constant:
		if ((m_double != rhs.m_double) || (m_idx != rhs.m_idx))
			return false;
		break;
	case type_t::string_constant:
		if ((m_string != rhs.m_string) || (m_idx != rhs.m_idx))
			return false;
		break;
	}

	return true;
}

void token_t::print(std::ostream& os) const
{
	os << "line " << m_line << ", column " << m_column << ": ";

	switch (m_type) {
	case type_t::end_of_file:
		os << "end-of-file";
		break;
	case type_t::end_of_line:
		os << "end-of-line";
		break;
	case type_t::identifier:
		os << "identifier";
		break;
	case type_t::string_constant:
		os << "string";
		break;
	case type_t::integer_constant:
		os << "integer";
		break;
	case type_t::float_constant:
		os << "float";
		break;
	};

	if (!m_error)
		switch (m_type) {
		case type_t::end_of_file:
		case type_t::end_of_line:
			break;
		case type_t::identifier:
			os << " '" << m_env.sbucket()[m_idx] << "'";
			break;
		case type_t::integer_constant:
			os << " " << m_int << " ["
			   << m_env.sbucket()[m_idx] << "]";
			break;
		case type_t::float_constant:
			os << " " << m_double << " ["
			   << m_env.sbucket()[m_idx] << "]";
			break;
		case type_t::string_constant:
			os << " \"" << m_env.sbucket()[m_string] << "\"["
			   << m_env.sbucket()[m_idx] << "]";
			break;
		}

	os << ": " << m_error.category().name() << ": "
	   << m_error.message();

	os << "\n";
}

tokenizer_t::tokenizer_t(environment_t& env, const char *buf, size_t buf_size)
	: m_env(env), m_token(env), m_currp(buf), m_bytes_left(buf_size),
	  m_line(1), m_column(1)
{
}

void tokenizer_t::next_char(unsigned nr_chars)
{
	for (; nr_chars > 0; nr_chars--) {
		if (!m_bytes_left)
			return;

		if (*m_currp == '\n') {
			m_line++;
			m_column = 1;
		} else {
			m_column++;
		}

		m_bytes_left--;
		m_currp++;
	}
}

void tokenizer_t::skip_whitespace_chars()
{
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
		if ((*m_currp == ' ')
		    || (*m_currp == '\t')
		    || (*m_currp == '\r')) {
			next_char();
		} else {
			break;
		}
	}
}

void tokenizer_t::skip_whitespace()
{
	while (m_bytes_left) {
		skip_whitespace_chars();

		// Skip linefeed if followed by line continuation
		if (*m_currp == '\n') {
			// Remember position in case this wasn't continuation
			const size_t old_column = m_column;
			const size_t old_line = m_line;
			const char * const old_currp = m_currp;
			const size_t old_bytes_left = m_bytes_left;

			next_char();
			skip_whitespace_chars();
			if ((m_bytes_left >= 3)
			    && (m_currp[0] == '.')
			    && (m_currp[1] == '.')
			    && (m_currp[2] == '.')) {
				next_char(3);
				continue;
			} else {
				// No continuation, restore position
				m_column = old_column;
				m_line = old_line;
				m_currp = old_currp;
				m_bytes_left = old_bytes_left;
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

uintmax_t tokenizer_t::get_value(unsigned radix, std::error_code& ec)
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

	if (parser_error && !ec)
		ec = error::overflow_error;

	return value;
}

sbucket_idx_t tokenizer_t::get_identifier()
{
	const char * const str = m_currp;
	hasher_t hash;

	while (m_bytes_left
	       && (*m_currp != ' ')
	       && (*m_currp != '\t')
	       && (*m_currp != '\r')
	       && (*m_currp != '\n')
	       && (*m_currp != '#')) {
		hash.next(*m_currp);
		next_char();
	}

	if ((m_currp - str) == 0)
		throw std::system_error(error::internal_error,
					error_category(),
					"Empty identifier");

	return m_env.sbucket().find_add_hashed(str, m_currp - str, hash);
}

sbucket_idx_t tokenizer_t::get_string(std::error_code& ec)
{
	const char * const str = m_currp;
	hasher_t hash;

	if (*m_currp != '"')
		return -1;

	next_char();

	while (*m_currp != '"') {
		if (!m_bytes_left) {
			if (!ec) ec = error::unexpected_eof;
			return -1;
		}
		hash.next(*m_currp);
		next_char();
	}

	return m_env.sbucket().find_add_hashed(str, m_currp - str, hash);	
}

sbucket_idx_t tokenizer_t::get_unit(std::error_code& ec)
{
	if (*m_currp != '[') {
		if (!ec
		    && (*m_currp != '\n')
		    && (*m_currp != '\r')
		    && (*m_currp != '\t')
		    && (*m_currp != ' ')) {
			ec = error::illegal_character;
		}
		return -1;
	}

	next_char();

	const char * const str = m_currp;
	hasher_t hash;

	while (*m_currp != ']') {
		if (!m_bytes_left) {
			if (!ec) ec = error::unexpected_eof;
			return -1;
		}
		if ((*m_currp == '\n')
		    || (*m_currp == ' ')
		    || (*m_currp == '\t')
		    || (*m_currp == '\r')) {
			if (!ec) ec = error::illegal_character;
			return -1;
		}
		hash.next(*m_currp);
		next_char();
	}

	const size_t nr_chars = m_currp - str;

	next_char();

	if (nr_chars == 0)
		return -1;

	return m_env.sbucket().find_add_hashed(str, nr_chars, hash);
	
}

const token_t& tokenizer_t::next()
{
	std::error_code ec;

	skip_whitespace();
	
	// We have something, mark this position
	m_token.set_pos(m_line, m_column);

	// Check if we got end of file
	if (!m_bytes_left)
		return m_token.set_end_of_file();

	// Check for line feed
	if (*m_currp == '\n') {
		m_token.set_pos(m_line, m_column);
		next_char();
		return m_token.set_end_of_line();
	}

	// Check for string
	if (*m_currp == '"') {
		const sbucket_idx_t str = get_string(ec);
		const sbucket_idx_t unit = get_unit(ec);

		return m_token.set_string_constant(
			str, unit, ec);
	}		

	// Check for number
	if (isdigit(*m_currp) || ((*m_currp == '-') && (m_bytes_left > 1)
				  && isdigit(m_currp[1]))) {
		unsigned radix = 10;
		int signedness = 1;

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

		value = get_value(radix, ec);
		value *= signedness;
		
		// Check if float
		if (*m_currp == '.') {
			double dvalue = value;

			if ((intmax_t) dvalue != value)
				ec = error::overflow_error;

			next_char();
			uintmax_t divisor = radix;
			while (m_bytes_left && isdigit(*m_currp)) {
				dvalue += (double) (*m_currp - '0') / divisor;
				divisor *= radix;
				next_char();
			}

			const sbucket_idx_t unit = get_unit(ec);
			return m_token.set_float_constant(dvalue, unit, ec);
		}

		const sbucket_idx_t unit = get_unit(ec);
		return m_token.set_integer_constant(value, unit, ec);
	}

	// If nothing else, then this is an identifier
        const sbucket_idx_t idx = get_identifier();
	return m_token.set_identifier(idx, ec);
}
