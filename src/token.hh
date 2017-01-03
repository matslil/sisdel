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
#include "environment.hh"
#include "sbucket.hh"
#include "position.hh"
#include "mmap_file.hh"

class token_t {
public:
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
		: m_type(type), m_position(pos), m_value(0) {}

	// type == eol or integer
	// For eol, value is number of indents.
	// For integer, this is the actual integer value
	token_t(type_t type, const position_t& pos, size_t value)
		: m_type(type), m_position(pos), m_value(value) {}

	// type = floating
	token_t(type_t type, const position_t& pos, double floating)
		: m_type(type), m_position(pos), m_float(floating) {}

	// type = string or identifier
	token_t(type_t type, const position_t& pos, sbucket_idx_t str)
		: m_type(type), m_position(pos), m_string(str) {}

private:
	type_t m_type;
	position_t m_position;

	union {
		double m_float;         // Only for type number_float
		sbucket_idx_t m_string; // Only for type string & identifier
		size_t m_value     ;    // Only given for type eol & integer
	};
};
	
#define TOKEN_SEPARATORS         "\n\r\t "
#define IDENTIFIER_INVALID_CHARS "\"()[]\{}'#" TOKEN_SEPARATORS

class tokenizer_t {
public:
	tokenizer_t(environment_t& env, const char* file);
	~tokenizer_t();

	const token_t next(void);

private:
	uint64_t get_number(char base, unsigned divisor_step, const char *valid_digits, size_t& nr_digits, const position_t& token_start);

	environment_t& m_env;
	mmap_file_t m_file;
	position_t m_startofline;
};

#endif /* TOKEN_H */
