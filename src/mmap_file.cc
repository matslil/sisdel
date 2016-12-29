/*

Memory mapped file for token parser.

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


#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>

#include "mmap_file.hh"
#include "file.hh"

///////////////////////////////////////////////////////////////////////////////
//
// Private class: mmap_t
//
///////////////////////////////////////////////////////////////////////////////

static const void *mmap_wrap(size_t length, int fd) {
	const void * const ptr = mmap(NULL, length, PROT_READ, MAP_PRIVATE, fd, 0);
	if (ptr == NULL)
		throw std::system_error(errno, std::generic_category(), "mmap");

	return ptr;
}

mmap_file_t::mmap_t::mmap_t(const char *name)
	: m_file(name, O_RDONLY), m_map(mmap_wrap(m_file.size(), m_file.fd())) {
}

mmap_file_t::mmap_t::~mmap_t() {
	munmap(const_cast<void*>(m_map), m_file.size());
}

///////////////////////////////////////////////////////////////////////////////
//
// Class: mmap_file_t
//
///////////////////////////////////////////////////////////////////////////////

mmap_file_t::mmap_file_t(environment_t &env, sbucket_idx_t name)
	: m_name(name), m_map(env.sbucket()[name])
{}
