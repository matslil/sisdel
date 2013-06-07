/*

Token scanner.

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

#ifndef TOKEN_H
#define TOKEN_H

#include <memory>
#include "scope.h"
#include "file.h"
#include "sbucket.h"
#include "value.hh"

class token_t {
	friend class tokenizer_t;
public:
	enum : sbucket_idx_t {
		eof = -1,
		eol = -2,
		err = -3
	};

	enum token_type_t {
		identifier,
		string
	};

	token_t(sbucket_idx_t idx, size_t line, size_t column)
		: m_idx(idx), m_line(line), m_column(column) {}
	sbucket_idx_t token() const { return m_idx; }
	size_t line() const { return m_line; }
	size_t column() const { return m_column; }

private:
	sbucket_idx_t m_idx;	
	token_type_t m_type;
	size_t m_line;
	size_t m_column;
};

class tokenizer_t {
public:
	tokenizer_t(environment_t& env, shared_ptr<mmap_file_t> file);
	~tokenizer_t();

	const token_t& next(type_t valid_types);

private:
	void next_char();
	void skip_whitespace();
	bool skip(const char *list);

	environment_t& m_env;
	shared_ptr<mmap_file_t> m_file;
	token_t m_token;
	const char *m_currp;
	size_t m_bytes_left;
	size_t m_line;
	size_t m_column;
};

#endif /* TOKEN_H */
