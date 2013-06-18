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
#include <ostream>
#include "environment.hh"
#include "file.hh"
#include "sbucket.hh"
#include "error.hh"
#include "mmap_file.hh"

class token_t {
	friend class tokenizer_t;
	friend class test_token;
public:
	enum class type_t {
		end_of_file,
		end_of_line,
		identifier,
		string_constant,
		integer_constant,
		float_constant
	};

	token_t(environment_t& env) : m_idx(0), m_int(0), m_double(0),
				      m_string(0), m_line(0), m_column(0),
				      m_env(env) {}
	token_t(token_t&) = default;
	token_t(token_t&&) = default;

	type_t type() const { return m_type; }

	sbucket_idx_t identifier() const 
		{
			if (m_type != type_t::identifier)
				throw std::system_error(
					error::internal_error,
					error_category());
			return m_idx;
		}
	intmax_t integer_constant(sbucket_idx_t& unit) const
		{
			if (m_type != type_t::integer_constant)
				throw std::system_error(
					error::internal_error,
					error_category());
			unit = m_idx;
			return m_int;
		}
	double float_constant(sbucket_idx_t& unit) const
		{
			if (m_type != type_t::float_constant)
				throw std::system_error(
					error::internal_error,
					error_category());
			unit = m_idx;
			return m_double;
		}
	sbucket_idx_t  string_constant(sbucket_idx_t& unit) const
		{
			if (m_type != type_t::string_constant)
				throw std::system_error(
					error::internal_error,
					error_category());
			unit = m_idx;
			return m_string;
		}
	size_t line() const { return m_line; }
	size_t column() const { return m_column; }

	operator bool() const { return static_cast<bool>(m_error); }
	std::error_code error() const { return m_error; }

	bool operator==(const token_t& rhs) const;
	void print(std::ostream& os) const;

	token_t& set_pos(size_t line, size_t column)
		{
			m_line = line;
			m_column = column;
			return *this;
		}

	token_t& set_end_of_file(void)
		{
			m_type = type_t::end_of_file;
			m_idx = m_int = m_double = m_string = 0;
			return *this;
		}

	token_t& set_end_of_line(void)
		{
			m_type = type_t::end_of_line;
			m_idx = m_int = m_double = m_string = 0;
			return *this;
		}

	token_t& set_identifier(sbucket_idx_t idx, std::error_code ec)
		{
			m_idx = idx;
		        m_type = type_t::identifier;
			m_int = m_double = m_string = 0;
			m_error = ec;
			return *this;
		}
	token_t& set_integer_constant(intmax_t value,
				      sbucket_idx_t unit,
				      std::error_code ec)
		{
			m_int = value;
			m_idx = unit;
			m_type = type_t::integer_constant;
			m_double = m_string = 0;
			m_error = ec;
			return *this;
		}
	token_t& set_float_constant(double value,
				    sbucket_idx_t unit,
				    std::error_code ec)
		{
			m_double = value;
			m_idx = unit;
			m_type = type_t::float_constant;
			m_int = m_string = 0;
			m_error = ec;
			return *this;
		}
	token_t& set_string_constant(sbucket_idx_t str,
				     sbucket_idx_t unit,
				     std::error_code ec)
		{
			m_string = str;
			m_idx = unit;
			m_type = type_t::string_constant;
			m_int = m_double = 0;
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
	double   m_double;

	/** Value of string
	 *  Valid for type string_constant. */
	sbucket_idx_t m_string;

	/** Error code
	 *  0 if no error, otherwise error::<value>. */
	std::error_code m_error;

	/** Token type
	 *  Indicates what type the token is of. */
	type_t m_type;

	/** Defined at line */
	size_t m_line;

	/** Defined at column */
	size_t m_column;

	/** Environment reference, only needed for print() */
	environment_t& m_env;
};

class tokenizer_t {
public:
	tokenizer_t(environment_t& env, const char *buf, size_t buf_size);
	~tokenizer_t() {}

	const token_t& next();

private:
	void next_char(unsigned nr_chars = 1);
	void skip_whitespace_chars();
	void skip_whitespace();
	bool skip(const char *list);
	uintmax_t get_value(unsigned radix, std::error_code& ec);
	sbucket_idx_t get_identifier();

	environment_t& m_env;
	token_t m_token;
	const char *m_currp;
	size_t m_bytes_left;
	size_t m_line;
	size_t m_column;
};

#endif /* TOKEN_H */
