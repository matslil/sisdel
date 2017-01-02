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
#include "position.hh"

class token_t {
	enum type_t : unsigned {
		eof,
		eol,
		integer,
		floating,
		string,
		identifier
	};

	// type == eof
	token_t(type_t type, const position_t& pos)
		: m_type(type), m_position(pos), m_int(0) {}

	// type == eol
	token_t(type_t type, const position_t& pos, size_t nr_indents)
		: m_type(type), m_position(pos), m_value(name),
		  m_nr_indents(nr_indents) {}

	// type = integer
	token_t(type_t type, const position_t& pos, uint64_t integer)
		: m_type(type), m_int(integer), m_position(pos) {}

	// type = floating
	token_t(type_t type, const position_t& pos, double floating)
		: m_type(type), m_position(pos), m_float(floating) {}

	// type = string or identifier
	token_t(type_t type, const position_t& pos, sbucket_idx_t str)
		: m_type(type), m_position(pos), m_string(str) {}

	type_t m_type;
	position_t m_position;

	union {
		uint64_t m_int;         // Only for type number_integer
		double m_float;         // Only for type number_float
		sbucket_idx_t m_string; // Only for type string & identifier
		size_t m_nr_indents;    // Only given for type eol
	};
};
	
#define IDENTIFIER_INVALID_CHARS "\"()[]\{}'#"
#define TOKEN_SEPARATORS         "\n\r\t "

class tokenizer_t {
public:
	tokenizer_t(environment_t& env, const char* file);
	~tokenizer_t();

	const token_t next(void);

private:
	uint64_t get_number(unsigned base, unsigned divisor_step, const char *valid_digits, size_t& nr_digits);

	environment_t& m_env;
	mmap_file_t m_file;
	const position_t m_startofline;
};

#endif /* TOKEN_H */
