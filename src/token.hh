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
#include <system_error>
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

	constexpr type_t type(void) const noexcept { return m_type; }
	constexpr const position_t& position(void) const noexcept
		{ return m_position; }
	constexpr double float_value(void) const throw(std::system_error)
		{
			if (m_type != floating) throw std::system_error(
				ENOTSUP, std::generic_category(),
				"Requesed floating value for non-floating token");
			return m_float;
		}
	constexpr sbucket_idx_t string_value(void) const throw(std::system_error)
		{
			if ((m_type != string) && (m_type != identifier))
				throw std::system_error(
					ENOTSUP, std::generic_category(),
					"Requested string value for non-string or non-identifier token");
			return m_string;
		}
	constexpr size_t value(void) const throw(std::system_error)
		{
			if ((m_type != eol) && (m_type != integer))
				throw std::system_error(
					ENOTSUP, std::generic_category(),
					"Requested integer value for non-integer or non-eol token");
			return m_value;
		}
	
private:
	type_t m_type;
	position_t m_position;

	union { // FIXME: Float should store precision (nr of digits)
		double m_float;         // Only for type number_float
		sbucket_idx_t m_string; // Only for type string & identifier
		size_t m_value     ;    // Only given for type eol & integer
	};
};

std::ostream& operator<<(std::ostream& os, const token_t& t);

#define TOKEN_SEPARATORS         "\n\r\t "
#define IDENTIFIER_INVALID_CHARS "\"()[]\{}'#" TOKEN_SEPARATORS

class tokenizer_t {
public:
	tokenizer_t(environment_t& env, const char* file);

	const token_t next(void);

private:
	uint64_t get_number(char base, const char *valid_digits, size_t& nr_digits);

	environment_t& m_env;
	mmap_file_t m_file;
	position_t m_startofline;
};

#endif /* TOKEN_H */
