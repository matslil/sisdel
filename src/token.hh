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

struct token_t {
	enum : sbucket_idx_t {
		eof = -1,
		eol = -2,
		lparen = -3,
		rparen = -4,
		lbracket = -5,
		rbracket = -6,
		lcurly = -7,
		rcurly = -8
	};

	token_t(sbucket_idx_t idx, size_t line, size_t column)
		: idx(idx), line(line), column(column) {}

	sbucket_idx_t idx;	
	size_t line;
	size_t column;
};

class tokenizer_t {
public:
	tokenizer_t(environment_t& env, shared_ptr<mmap_file_t> file);
	~tokenizer_t();

	const token_t& next(type_t valid_types);

private:
	bool skip(const char *list);

	environment_t& m_env;
	shared_ptr<mmap_file_t> m_file;
	token_t m_token;
	const char *m_currp;
	size_t m_bytes_left;
};

#endif /* TOKEN_H */
