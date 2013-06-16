/*

Error reporting.

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

#ifndef ERROR_HH
#define ERROR_HH

#include <iostream>
#include <system_error>

namespace error {

	// Sisdel error codes
	enum error_code_t {
		no_error = 0,
		syntax_error,
		unexpected_eof,
		internal_error,
		overflow_error
	};

	// Sisdel error conditions
	enum error_condition_t {
		parser_error = 1,
		implementation_error
	};

	std::error_code make_error_code(error_code_t err);
	std::error_condition make_error_condition(error_condition_t econd);
}

class error_category_impl : public std::error_category {
public:
	virtual const char *name() const;
	virtual std::string message(int ev) const;
	virtual std::error_condition default_error_condition(int ev) const;
};

const std::error_category& error_category();

namespace std {
	template <> struct is_error_code_enum<error::error_code_t>
		: public true_type {};

	template <> struct is_error_condition_enum<error::error_condition_t>
		: public true_type {};
}

#endif // ERROR_HH
