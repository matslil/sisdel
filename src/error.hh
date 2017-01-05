/*

Exception object.

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
along with GCC; see the file COPYING.  If not see
<http://www.gnu.org/licenses/>.

*/

#ifndef ERROR_HH
#define ERROR_HH


#include <exception>

#include "position.hh"
#include "sbucket.hh"
#include "environment.hh"

class parser_error : public std::exception {
public:
	parser_error(const position_t& token_start,
		     const position_t& error_at, const std::string& msg);

	const char *what() const noexcept { return m_what.c_str(); }

private:
	std::string m_what;
};

#endif // ERROR_HH
