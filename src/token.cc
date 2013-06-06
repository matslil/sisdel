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
