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
#include "file.hh"
#include "sbucket.hh"
#include "value.hh"
#include "error.hh"

class token_t {
	friend class tokenizer_t;
public:
	enum : sbucket_idx_t {
		eof = -1,
		eol = -2
	};

	enum token_type_t {
		identifier,
		string_constant,
		integer_constant,
		float_constant
	};

	token_t() : m_idx(0), m_line(0), m_column(0) {}
	sbucket_idx_t identifier() const 
		{
			if (m_type != identifier) throw std::system_error(
				error::internal_error,
				error_category());
			return m_idx;
		}
	intmax_t integer_constant(sbucket_idx_t& unit) const
		{
			if (m_type != integer_constant) throw std::system_error(
				error::internal_error,
				error_category());
			unit = m_idx;
			return m_int;
		}
	double float_constant(sbucket_idx_t& unit) const
		{
			if (m_type != float_constant) throw std::system_error(
				error::internal_error,
				error_category());
			unit = m_idx;
			return m_double;
		}
	const std::string& string_constant(sbucket_idx_t& unit) const
		{
			if (m_type != string_constant) throw std::system_error(
				error::internal_error,
				error_category());
			unit = m_idx;
			return m_string;
		}
	size_t line() const { return m_line; }
	size_t column() const { return m_column; }

	bool operator bool() const { return ec == true; }
	std::error_code error() const { return m_error; }

private:
	const token_t& set_identifier(sbucket_idx_t idx,
				      std::error_code ec)
		{
			m_idx = idx;
		        m_type = identifier;
			m_error = ec;
			return *this;
		}
	const token_t& set_integer_constant(intmax_t value,
					    sbucket_idx_t unit,
					    std::error_code ec)
		{
			m_int = value;
			m_idx = unit;
			m_type = integer;
			m_error = ec;
			return *this;
		}
	const token_t& set_float_constant(double value,
					  sbucket_idx_t unit,
					  std::error_code ec)
		{
			m_double = value;
			m_idx = unit;
			m_type = floating_point;
			m_error = ec;
			return *this;
		}
	const token_t& set_string_constant(sbucket_idx_t str,
					   sbucket_idx_t unit,
					   std::error_code ec)
		{
			m_string = str;
			m_idx = unit;
			m_type = string_constant;
			m_error = ec;
			return *this;
		}

	/** Name of identifier or unit
	 *  Valid for types identifier, integer and floating_point. */
	sbucket_idx_t m_idx;

	/** Value of integer
	 *  Valid for type integer. */
	intmax_t m_int;

	/** Value of double
	 *  Valid for type floating_point. */
	double   m_dobule;

	/** Value of string
	 *  Valid for type string_constant. */
	sbucket_idx_t m_string;

	/** Error code
	 *  0 if no error, otherwise error::<value>. */
	std::error_code m_error;

	/** Token type
	 *  Indicates what type the token is of. */
	token_type_t m_type;

	/** Defined at line */
	size_t m_line;

	/** Defined at column */
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
	uintmax_t get_value();
	sbucket_idx_t get_identifier(std:error_code& ec);

	environment_t& m_env;
	shared_ptr<mmap_file_t> m_file;
	token_t m_token;
	const char *m_currp;
	size_t m_bytes_left;
	size_t m_line;
	size_t m_column;
};

#endif /* TOKEN_H */
