/*

File operations.

Represents an opened file descriptor. The purpose of the class is primarily
to ensure the file is closed when the file descriptor goes out of scope.

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
along with GCC; see the file COPYING.  If not see
<http://www.gnu.org/licenses/>.

*/

#ifndef FILE_H
#define FILE_H

#include <sys/types.h>

class file_t {
public:
	file_t(const char *name, int flags);
	~file_t();
	constexpr size_t size(void) const noexcept { return m_size; }
	constexpr int fd() const noexcept { return m_fd; }

	// Forbidden methods
	file_t() = delete;
	file_t(const file_t&) = delete;
	file_t& operator=(const file_t&) = delete;
	file_t& operator=(file_t &&from) = delete;
	file_t(file_t &&from) = delete;

private:
	const int m_fd;
	const size_t m_size;
};

#endif /* FILE_H */
