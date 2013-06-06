/*

Memory mapped file support.
This basically puts a class around mmap()/munmap().

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

#ifndef MMAP_H
#define MMAP_H

class mmap_t {
public:
	mmap_t(void *addr, size_t length, int prot, int flags, int fd,
	       off_t offset);
	mmap_t(mmap_t &&from);
	~mmap_t();
	mmap_t& operator=(mmap_t &&from);
	void *map(void) { return m_map; }
	size_t size(void) const { return m_size; }
        const void *map(void) const { return m_map; }

	// Forbidden methods
	mmap_t() = delete;
	mmap_t(const mmap_t&) = delete;
	mmap_t& operator=(const mmap_t&) = delete;

private:
	void *m_map;
	size_t m_size;
};

#endif // MMAP_H
