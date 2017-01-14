/*

Memory mapped file for the token parser.

Copyright (C) 2013-2016 Mats G. Liljegren

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

#include "position.hh"
#include "mmap_file.hh"
#include <sstream>
#include <string.h>

#if 0
size_t strsz(const char * str, char find, const char * end)
{
	size_t idx = 0;
	
	while (str != end) {
		if (str[idx] == find)
			return idx;
		idx++;
	}

	return idx;
}

std::string position_t::buffln(void) const
{
	std::string str(m_start, strsz(m_start, '\n', m_end));
	return str;
}

#endif

std::ostream& operator<<(std::ostream& os, const position_t& m)
{ 
	os << m.file().filename() << ':' << m.line() << ':' << m.column();
	return os;
}
