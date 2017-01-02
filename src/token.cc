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
#include <cmath>

tokenizer_t::tokenizer_t(environment_t& env, shared_ptr<mmap_file_t> file)
	: m_env(env), m_file(file), m_token(-1, 1, 1),
	  m_currp(file.buf()), m_bytes_left(file.buf_size())
{
}


bool tokenizer_t::skip(const char *list)
{
	bool found = false;

	while (m_bytes_left) {
		unsigned i;

		for (i = 0; list[i] != '\0'; i++) {
			if (*m_currp == list[i]) {
				found = true;
				m_currp++;
				m_bytes_left--;
				break;
			}
		}
		if (list[i] == '\0')
			return found;
	}

	return found;
}

token_t& tokenizer_t::next()
{
	// Check whitespace, including comments
	for(;;) {
		if (!m_bytes_left) {
			// End of file
			token_t token(m_line, m_column);
			return std::move(token);
		}
		if (*m_currp == ' ')
			m_column++;
		else if (*m_currp == '\t')
			m_column += 8;
		else if (*m_currp == '#') {
			while (--m_bytes_left && (*(++m_currp) != '\n'));
			continue;
		}
			
		m_currp++;
		m_bytes_left--;
	}
	
	// Check for line feed/carriage return
	if (skip("\n\r")) {
		m_token.m_idx = token_t::eol;
		return m_token;
	}

	// Check for string
	if (if *m_currp == '"') {
		m_currp++;
		m_bytes_left--;
		const char *str = m_currp;
		
	}
}

enum Status token_next(struct Scope *scope,
		       TokenHandle handle,
		       SbucketIdx *out_token,
		       enum TokenType *out_type) {
	const char * const old_linep = &handle->linep[handle->pos];
	enum Status status;
	size_t nr_chars = 0;
	hash_t hash = 0;

	if (handle->pos == handle->linelen) return status_end_of_file;

	if ((handle->pos == 0) && ((handle->linep[handle->pos] == ' ') || (handle->linep[handle->pos] == '\t'))) {
		/* Indent token */
		*out_type = token_type_indent;
		
		while ((handle->pos != handle->linelen)
		       && ((handle->linep[handle->pos] == ' ')
			   || (handle->linep[handle->pos] == '\t'))) {
			hash = hash_next(hash, handle->linep[handle->pos]);
			nr_chars++;
			handle->pos++;
		}

		hash = hash_finish(hash);

		status = sbucket_find_add_hashed(
			scope, handle->bucket, old_linep, nr_chars, hash, out_token);

		if (status == status_success)
			*out_type = token_type_indent;

		return status;
	}

	/* Skip leading white-space */
	while ((handle->pos != handle->linelen)
	       && ((handle->linep[handle->pos] == ' ')
		   || (handle->linep[handle->pos] == '\t'))) handle->pos++;

	if (handle->pos == handle->linelen) return status_end_of_file;

	hash = 0;

	while ((handle->pos != handle->linelen)
	       && ((handle->linep[handle->pos] != ' ')
		   || (handle->linep[handle->pos] != '\t'))) {
		hash = hash_next(hash, handle->linep[handle->pos]);
		nr_chars++;
		handle->pos++;
	}

	hash = hash_finish(hash);

	status = sbucket_find_add_hashed(
		scope, handle->bucket, old_linep, nr_chars, hash, out_token);

	if (status == status_success)
		*out_type = token_type_token;

	return status;
}




//////////////////////////////////////////

tokenizer_t::tokenizer_t(environment_t& env, const char *file)
	: m_env(env), m_file(env, env.bucket().find_add(file)),
	  m_startofline(m_file.get_position())
{
}

static bool valid_digit(char ch, unsigned base)
{
	if (base > 10)
		return ((ch >= '0') && (ch <= '9'))
			|| ((ch >= 'a') && (ch <= ('a' + base - 10)));
	else
		return (ch >= '0') && (ch <= ('9' - base));
}

uint64_t tokenizer_t::get_number(unsigned base, unsigned divisor_step, const char *valid_digits, size_t& nr_digits)
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
				throw std::system_error(EINVAL, std::generic_category(), "Expected divisor here");
			else
				divisor_steps_left--;
			nr_digits++;
		} else if (ch == '\'') {
			if (divisor_present && (divisor_steps_left != 0))
				throw std::system_error(EINVAL, std::generic_category(), "Did not expect divisor here");
			divisor_steps_left = divisor_step;
		} else {
			break;
		}

		m_file.skip();
	}

	if (divisor_present && (divisor_steps_left != 0))
		throw std::system_error(EINVAL, std::generic_category(), "Did not expect divisor here");

	return nr;
}


token_t tokenizer_t::next(void)
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
			(void) skip("\r\n");

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
			token_t token(token_t::eol, m_startofline, nr_indents);
			return token;
		}

		if (valid_digit(ch, 10)) {
			// Handle numbers

			const position_t start_of_number =
				m_file.get_position();

			// Determine base
			const char *valid_digits;
			unsigned divisor_step;
			unsigned base;
			if (ch == '0')
				skip();
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

			unsigned nr_digits;
			uint64_t integer = get_number(base, divisor_step, valid_digits, nr_digits);

			if (m_file.peek() == '.') {
				m_file.skip();
				uint64_t decimals = get_number(base, divisor_step, valid_digits, nr_digits);
				double d = (double) integer + (decimals / pow(base, nr_digits));

				// Ensure there's no trailing garbage
				if (strchr(TOKEN_SEPARATORS, m_file.peek()) == NULL)
					throw std::system_error(EINVAL, std::generic_category, "Trailing garbage after string constant");

				// Return floating token
				token_t token(token_t::floating,
					      start_of_number,
					      d);
				return token;
			}
				
			// Ensure there's no trailing garbage
			if (strchr(TOKEN_SEPARATORS, m_file.peek()) == NULL)
				throw std::system_error(EINVAL, std::generic_category, "Trailing garbage after string constant");


			// Return integer token
			token_t token(token_t::integer, start_of_number, nr);
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
			skip();

			// Ensure there's no trailing garbage
			if (strchr(TOKEN_SEPARATORS, m_file.peek()) == NULL)
				throw std::system_error(EINVAL, std::generic_category, "Trailing garbage after string constant");

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
			IDENTIFIER_INVALID_CHARS TOKEN_SEPARATOR, hash);

		if (size == 0)
			throw std::system_error(EINVAL, std::generic_category, "Invalid identifier name");
		
		if (strchr(TOKEN_SEPARATORS, m_file.peek()) == NULL)
			throw std::system_error(EINVAL, std::generic_category, "Invalid identifier name");

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
