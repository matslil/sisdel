/*
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

/**
 * @file
 * Token scanner.
 * This file declares the token scanner class, tokenizer_t, plus the
 * different possible tokens that it can return.
 * @par
 * The token scanner interface is trivial, it only has a constructor and
 * a method for providing the next token, if any.
 */

#include <memory>
#include <system_error>
#include "environment.hh"
#include "sbucket.hh"
#include "position.hh"
#include "mmap_file.hh"
#include <boost/multiprecision/mpfr.hpp>
#include <boost/multiprecision/gmp.hpp>

/**
 * Multi-precision integer type.
 * This type is used to store Sisdel integers, which are by default multi-
 * precision integers. This means that the integer can be as large as the
 * available memory allows.
 */
typedef boost::multiprecision::number<boost::multiprecision::gmp_int> mp_int;

/**
 * Multi-precision floating point type.
 * This type is used to store Sisdel floating point numbers, which are by
 * default multi-precision floating point numbers. This means that the
 * value can be as large, and with selectable precision, as the available
 * memory allows.
 */
typedef boost::multiprecision::number<boost::multiprecision::mpfr_float_backend<0> > mp_float;

class token_t;

/**
 * Token scanner.
 * Reads the given file and split it into a stream of tokens.
 *
 * @todo Implement support for standard input as source.
 */
class tokenizer_t {
public:

	/**
	 * Construct the scanner.
	 *
	 * @todo When environment object has been made thread local, the
	 *       env parameter should be removed.
	 */
	tokenizer_t(
		environment_t& env, /**< Reference to environment object. */
		const char* file    /**< Name of file in UTF8 format. */
		);

	/**
	 * Return next token.
	 *
	 * @returns Next token in the token stream, or NULL if end of file.
	 */
	const token_t* next(void);

private:
	void get_number(mp_int& nr, char base,
			const char *valid_digits, size_t& nr_digits);

	environment_t& m_env;
	mmap_file_t m_file;
	position_t m_startofline;
};

/**
 * Base class for tokens.
 * This class provides the interface provided by all inherited tokens.
 * This is a virtual class, so no token is of this type.
 * @par
 * Use constructs like "typeid(returned_token) == typeid(token_eol_t)"
 * to determine what kind of token this is, and then use
 * dynamic_cast to cast it to the correct type.
 */
class token_t {
public:
	/**
	 * Destructor.
	 * Virtual destructor since this class will be inherited by other
	 * classes.
	 */
	virtual ~token_t() noexcept {};

	/**
	 * Determine where in the input stream this token was found.
	 *
	 * @returns Position in input stream.
	 */
	virtual const position_t& position(void) const noexcept = 0;
};

/**
 * End of line token.
 * This token is returned when an end of line character is found.
 * Any multiple end of line characters are skipped, as are empty lines or
 * lines with only comments.
 * The token provides indentation level, i.e. how many tab characters that
 * were found right after the end of line character.
 */
class token_eol_t : public token_t {
public:
	/**
	 * Constructor, used by the scanner.
	 */
	token_eol_t(
		const position_t& pos, /**< Position in input stream where
					* this token was found. */
		size_t indent_level    /**< Indentation level, i.e. number of
					* tab characters right after the
					* end of line character. */
		)
		: m_position(pos), m_indent_level(indent_level) {}

	/**
	 * Return indentation level.
	 *
	 * @returns The indentation level, i.e. number of tab characters,
	 *          right after the end of line character.
	 */
	constexpr size_t indent_level(void) const noexcept
		{ return m_indent_level; }

	//
	// Inherited from token_t
	//

	const position_t& position(void) const noexcept
		{ return m_position; }

private:
	position_t m_position;
	size_t m_indent_level;
};

/**
 * String token.
 * This token represents a string immediate value.
 */
class token_string_t : public token_t {
public:
	/**
	 * Constructor, used by scanner.
	 */
	token_string_t(
		const position_t& pos, /**< Position in input stream where
					* this token was found. */
		string_idx_t str       /**< String content as a string_idx_t. */
		)
		: m_position(pos), m_string(str) {}

	constexpr string_idx_t string(void) const noexcept
		{ return m_string; }


	//
	// Inherited from token_t
	//

	const position_t& position(void) const noexcept
		{ return m_position; }

private:
	position_t m_position;
	string_idx_t m_string;
};

class token_identifier_t : public token_t {
public:
	token_identifier_t(const position_t& pos, string_idx_t name)
		: m_position(pos), m_name(name) {}

	constexpr string_idx_t name(void) const noexcept
		{ return m_name; }

	//
	// Inherited from token_t
	//

	const position_t& position(void) const noexcept
		{ return m_position; }

private:
	position_t m_position;
	string_idx_t m_name;
};

class token_integer_t : public token_t {
public:
	token_integer_t(const position_t& pos, const mp_int& value)
		: m_position(pos), m_integer(value) {}

	constexpr const mp_int& value(void) const noexcept
		{ return m_integer; }

	//
	// Inherited from token_t
	//

	const position_t& position(void) const noexcept
		{ return m_position; }

private:
	position_t m_position;
	mp_int m_integer;
};

class token_float_t : public token_t {
public:
	token_float_t(const position_t& pos, const mp_float& floating)
		: m_position(pos), m_float(floating) {}

	constexpr const mp_float& value(void) const noexcept
		{ return m_float; }

	//
	// Inherited from token_t
	//

	const position_t& position(void) const noexcept
		{ return m_position; }

private:
	position_t m_position;
	mp_float m_float;
};

std::ostream& operator<<(std::ostream& os, const token_t& t);

#endif /* TOKEN_H */
