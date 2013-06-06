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

#include <system_error>
#include <sys/mman.h>
#include <errno.h>

#include "mmap.hh"

///////////////////////////////////////////////////////////////////////////////
//
// Class: mmap_t
//
///////////////////////////////////////////////////////////////////////////////

mmap_t::mmap_t(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
	: m_map(mmap(addr, length, prot, flags, fd, offset)), m_size(length)
{
	if (m_map == NULL) {
		throw std::system_error(errno, std::generic_category(), "mmap");
	}
}

mmap_t::mmap_t(mmap_t &&from)
	: m_map(from.m_map), m_size(from.m_size)
{
	// Tell destructor object has been moved
	from.m_map = NULL;
	from.m_size = 0;
}

mmap_t& mmap_t::operator=(mmap_t &&from) {
	// Move information
	m_map = from.m_map;
	m_size = from.m_size;

	// Tell destructor we've moved
	from.m_map = NULL;
	from.m_size = 0;

	return *this;
}

mmap_t::~mmap_t() {
	// Only unmap if object is still active, i.e. hasn't been moved
	if (m_map != NULL) {
		munmap(m_map, m_size);
	}
}
