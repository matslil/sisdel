/*
  This file defines the parser_error exception.

  SPDX-License-Identifier: MIT

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
