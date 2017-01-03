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
#include <sstream>
#include <string.h>

std::string position_t::str(const environment_t& env) const
{
	std::stringstream ss;
	ss << env.sbucket()[m_file] << ':' << m_line << ':' << m_col;
	return ss.str();
}

std::string position_t::buffln(void) const
{
	std::string str(m_buff, strnlen(m_start, m_end - m_start));
	return str;
}
