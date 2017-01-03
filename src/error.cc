/*

Exception object.

Copyright (C) 2013-2017 Mats G. Liljegren

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

#include <sstream>

#include "error.hh"

static std::string repeat_spaces(size_t nr)
{
	std::string str(nr, ' ');
	return str;
}

parser_error::parser_error(const environment_t& env, const position_t& token_start, const position_t& error_at, const std::string& msg)
{
	std::stringstream ss;
	ss << token_start.str(env) << ": error: " << msg
	   << "\n        " << token_start.buffln()
	   << "\n        " << repeat_spaces(error_at.m_col);

	m_what = ss.str();
}
