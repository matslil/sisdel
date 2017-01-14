/*

File position.

This class is used to store file position. Used to rewind the read, and to
produce diagnostic outputs.

Copyright (C) 2017 Mats G. Liljegren

This file is part of Sisdel.

Sisdel is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation, version 3.

Sisdel is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with Sisdel; see the file COPYING.  If not see
<http://www.gnu.org/licenses/>.

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
