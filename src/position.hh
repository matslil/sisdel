/*
  File position.

  This class is used to store file position. Used to produce diagnostic outputs.

  Copyright (C) 2017 Mats G. Liljegren
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

#ifndef POSITION_HH
#define POSITION_HH

#include "environment.hh"
#include "sbucket.hh"

class mmap_file_t;

class position_t {
public:
	position_t(std::string& str, const mmap_file_t * file,
		   size_t line, size_t col)
		: m_str(str), m_file(file), m_line(line), m_col(col) {}

	bool operator==(const position_t& rhs) const noexcept
		{
			return (m_str == rhs.m_str) &&
				(m_file == rhs.m_file) &&
				(m_line == rhs.m_line) &&
				(m_col == rhs.m_col);
		}

	// Return line of code
	const std::string& str(void) const { return m_str; }

	constexpr size_t column(void) const noexcept
		{ return m_col; }
	constexpr size_t line(void) const noexcept
		{ return m_line; }
	constexpr const mmap_file_t& file(void) const noexcept
		{ return *m_file; }
	
private:
	const std::string m_str;
	const mmap_file_t * const m_file;
	const size_t m_line;
	const size_t m_col;
};

// Return the position as a string using following format:
// <file name>:<line>:<column>
std::ostream &operator<<(std::ostream &os, position_t const &m);

#endif /* POSITION_HH */
