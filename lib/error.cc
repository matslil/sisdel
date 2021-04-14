/*
  Implementation of the parser_error exception class.

  SPDX-License-Identifier: MIT

*/

#include <sstream>

#include "error.hh"

static std::string repeat_spaces(size_t nr)
{
	std::string str(nr, ' ');
	return str;
}

parser_error::parser_error(const position_t& token_start, const position_t& error_at, const std::string& msg)
{
	std::stringstream ss;
	ss << token_start << ": error: " << msg
	   << "\n        " << token_start.str()
	   << "\n        " << repeat_spaces(error_at.column() - 1) << '^';

	m_what = ss.str();
}
