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

#include <system_error>

#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include "mmap_file.hh"
#include "file.hh"

///////////////////////////////////////////////////////////////////////////////
//
// Private class: mmap_t
//
///////////////////////////////////////////////////////////////////////////////

static const void *mmap_wrap(size_t length, int fd)
{
	const void * const ptr = mmap(NULL, length, PROT_READ, MAP_PRIVATE, fd, 0);
	if (ptr == NULL)
		throw std::system_error(errno, std::generic_category(), "mmap");

	return ptr;
}

mmap_file_t::mmap_t::mmap_t(const char *name)
	: m_file(name, O_RDONLY), m_map(static_cast<const char*>(mmap_wrap(m_file.size(), m_file.fd())))
{
}

mmap_file_t::mmap_t::~mmap_t()
{
	munmap((void*)m_map, m_file.size());
}

///////////////////////////////////////////////////////////////////////////////
//
// Class: mmap_file_t
//
///////////////////////////////////////////////////////////////////////////////

mmap_file_t::mmap_file_t(environment_t &env, sbucket_idx_t name)
	: m_map(env.sbucket()[name]),
	  m_pos(m_map.map(), m_map.map(), m_map.map() + m_map.file_size(),
		this),
	  m_filename(name)
{}

size_t mmap_file_t::skip(char skip_ch)
{
	size_t count = 0;

	while (!eof() && (*m_pos.m_buff == skip_ch)) {
		skip();
		count++;
	}
	
	return count;
}

size_t mmap_file_t::skip(const char *skip_str)
{
	size_t count = 0;

	while (!eof() && (strchr(skip_str, *m_pos.m_buff) != NULL)) {
		skip();
		count++;
	}

	return count;
}

void mmap_file_t::skip_until(char until_ch)
{
	if (eof())
		throw std::system_error(EINVAL, std::generic_category(), "Failed finding character");
	while (*m_pos.m_buff != until_ch) {
		skip();
		if (eof())
			throw std::system_error(EINVAL, std::generic_category(), "Failed finding character");
	}
}

size_t mmap_file_t::skip_until_hashed(char until_ch, hash_t& hash)
{
	size_t count = 0;
	hash = 0;
	if (eof())
		throw std::system_error(EINVAL, std::generic_category(), "Failed finding character");
	while (*m_pos.m_buff != until_ch) {
		hash = hash_next(*m_pos.m_buff, hash);
		count++;
		skip();
		if (eof())
			throw std::system_error(EINVAL, std::generic_category(), "Failed finding character");
	}

	hash = hash_finish(hash);

	return count;
}

size_t mmap_file_t::skip_until_hashed(const char* until_str, hash_t& hash)
{
	size_t count = 0;
	hash = 0;
	if (eof())
		throw std::system_error(EINVAL, std::generic_category(), "Failed finding character");
	while (strchr(until_str, *m_pos.m_buff) == NULL) {
		hash = hash_next(*m_pos.m_buff, hash);
		count++;
		skip();
		if (eof())
			throw std::system_error(EINVAL, std::generic_category(), "Failed finding character");
	}

	hash = hash_finish(hash);

	return count;
}

void mmap_file_t::skip(void)
{
	if (eof())
		throw std::system_error(EPIPE, std::generic_category(), "mmap_file_t::skip");
	if ((*m_pos.m_buff) == '\n') {
		m_pos.m_line++;
		m_pos.m_col = 1;
		m_pos.m_start = m_pos.m_buff;
	} else {
		m_pos.m_col++;
	}
	m_pos.m_buff++;
}
