/*
  Token scanner.

  Copyright (C) 2013-2017 Mats G. Liljegren
  SPDX-License-Identifier: Apache-2.0

  This file is part of Sisdel.

  Licensed under the Apache License, Version 2.0 (the "License"); you may not
  use this file except in compliance with the License. You may obtain a copy
  of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
  License for the specific language governing permissions and limitations
  under the License.
*/

#ifndef TOKEN_H
#define TOKEN_H

#include <memory>
#include <system_error>
#include "environment.hh"
#include "sbucket.hh"
#include "position.hh"
#include "mmap_file.hh"
#include <boost/multiprecision/mpfr.hpp>
#include <boost/multiprecision/gmp.hpp>

typedef boost::multiprecision::number<boost::multiprecision::gmp_int> mp_int;
typedef boost::multiprecision::number<boost::multiprecision::mpfr_float_backend<0> > mp_float;

class token_t;

class tokenizer_t {
public:

	tokenizer_t(environment_t& env, const char* file);

	const token_t* next(void);

private:
	void get_number(mp_int& nr, char base,
			const char *valid_digits, size_t& nr_digits);

	environment_t& m_env;
	mmap_file_t m_file;
	position_t m_startofline;
};

class token_t {
public:
	virtual ~token_t() noexcept {};

	virtual const position_t& position(void) const noexcept = 0;
};

class token_eol_t : public token_t {
public:
	token_eol_t(const position_t& pos, size_t indent_level)
		: m_position(pos), m_indent_level(indent_level) {}

	const position_t& position(void) const noexcept
		{ return m_position; }

	constexpr size_t indent_level(void) const noexcept
		{ return m_indent_level; }

private:
	position_t m_position;
	size_t m_indent_level;
};

class token_string_t : public token_t {
public:
	token_string_t(const position_t& pos, string_idx_t str)
		: m_position(pos), m_string(str) {}

	const position_t& position(void) const noexcept
		{ return m_position; }

	constexpr string_idx_t string(void) const noexcept
		{ return m_string; }

private:
	position_t m_position;
	string_idx_t m_string;
};

class token_identifier_t : public token_t {
public:
	token_identifier_t(const position_t& pos, string_idx_t name)
		: m_position(pos), m_name(name) {}

	const position_t& position(void) const noexcept
		{ return m_position; }

	constexpr string_idx_t name(void) const noexcept
		{ return m_name; }

private:
	position_t m_position;
	string_idx_t m_name;
};

class token_integer_t : public token_t {
public:
	token_integer_t(const position_t& pos, const mp_int& value)
		: m_position(pos), m_integer(value) {}

	const position_t& position(void) const noexcept
		{ return m_position; }

	constexpr const mp_int& value(void) const noexcept
		{ return m_integer; }

private:
	position_t m_position;
	mp_int m_integer;
};

class token_float_t : public token_t {
public:
	token_float_t(const position_t& pos, const mp_float& floating)
		: m_position(pos), m_float(floating) {}

	const position_t& position(void) const noexcept
		{ return m_position; }

	constexpr const mp_float& value(void) const noexcept
		{ return m_float; }

private:
	position_t m_position;
	mp_float m_float;
};

std::ostream& operator<<(std::ostream& os, const token_t& t);

#endif /* TOKEN_H */
