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
// Class: mmap_file_t
//
///////////////////////////////////////////////////////////////////////////////

mmap_t l_mmap_file_helper(const char *name) {
	file_t file(name, O_RDONLY);

	const off_t size = file.seek_end();

	(void) file.seek_begin();

	mmap_t map(NULL, size, PROT_READ, MAP_PRIVATE, file.fd(), 0);

	return map;
}

mmap_file_t::mmap_file_t(environment_t &env, sbucket_idx_t name)
	: m_name(name), m_map(l_mmap_file_helper(env.sbucket()[name]))
{}

mmap_file_t::mmap_file_t(mmap_file_t &&from)
	: m_name(from.m_name), m_map(std::move(from.m_map))
{}

mmap_file_t& mmap_file_t::operator=(mmap_file_t &&from) {
	m_name = from.m_name;
	m_map = std::move(from.m_map);

	return *this;
}
