/*
  Implementation for position_t class.

  SPDX-License-Identifier: MIT
*/

#include "position.hh"
#include "mmap_file.hh"
#include <sstream>
#include <string.h>

std::ostream& operator<<(std::ostream& os, const position_t& m)
{ 
	os << m.file().filename() << ':' << m.line() << ':' << m.column();
	return os;
}
