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

token_t& tokenizer_t::next()
{
	skip_whitespace();
	
	// Check for line feed
	if (*m_currp == '\n') {
		m_token.m_line = m_line;
		m_token.m_column = m_column;
		m_token.m_type = token_t::identifier;
		m_token.m_idx = token_t::eol;
		while (skip("\n") || skip_whitespace());
		return m_token;
	}
		
	// Check for string
	if (if *m_currp == '"') {
		next_char();
		const char *str = m_currp;
		hash_t hash = 0;
		size_t nr_chars = 0;
		while (m_currp != '"') {
			if (!m_bytes_left--) {
				// TODO: Report error
				m_token.m_idx = token_t::err;
				return m_token;
			}
			hash = hash_next(m_currp++, hash);
			nr_chars++;
		}
		
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
