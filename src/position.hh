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

class position_t {
public:
	position_t(const char *buff, const char *start, const char *end,
		   sbucket_idx_t file, size_t line = 1, size_t col = 1)
		: m_buff(buff), m_start(start), m_end(end), m_file(file),
		  m_line(line), m_col(col) {}

	// Return the position as a string using following format:
	// <file name>:<line>:<column>
	std::string str(const environment_t& env) const;

	// Returns first line of code being pointed at by this position
	std::string buffln(void) const;
	
	const char * m_buff;
	const char * m_start;
	const char * m_end;
	sbucket_idx_t m_file;
	size_t m_line;
	size_t m_col;
};

#endif /* POSITION_HH */
