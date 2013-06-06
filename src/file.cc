/*

File open and read.

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

#include "file.hh"
#include <system_error>
#include <unistd.h>
#include <fcntl.h>

///////////////////////////////////////////////////////////////////////////////
//
// Class: file_t
//
///////////////////////////////////////////////////////////////////////////////

file_t::file_t(const char *name, int flags) : m_fd(open(name, flags)) {
	if (m_fd < 0) {
		throw std::system_error(errno, std::generic_category(), "open");
	}
}

file_t::~file_t() {
	if (m_fd >= 0) close(m_fd);
}


off_t file_t::seek_begin(off_t offset) {
	const off_t new_offset = lseek(m_fd, offset, SEEK_SET);
	if (new_offset < 0)
		throw std::system_error(errno, std::generic_category(), "seek_begin");
	return new_offset;
}

off_t file_t::seek_end(off_t offset) {
	const off_t new_offset = lseek(m_fd, offset, SEEK_END);
	if (new_offset < 0)
		throw std::system_error(errno, std::generic_category(), "seek_end");
	return new_offset;
}

