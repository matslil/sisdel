/*
  Implementation of the mmap_file_t class.

  SPDX-License-Identifier: MIT

*/

#include <system_error>

#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include "mmap_file.hh"
#include "position.hh"
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

mmap_file_t::mmap_file_t(environment_t &env, const char * name)
	: m_env(env), m_map(name), m_buff(m_map.map()), m_start(m_buff),
	  m_end(m_buff + m_map.file_size()), m_col(1), m_line(1),
	  m_filename(m_env.sbucket().find_add(name))
{}

size_t mmap_file_t::skip(char skip_ch)
{
	size_t count = 0;

	while (!eof() && (*m_buff == skip_ch)) {
		skip();
		count++;
	}
	
	return count;
}

size_t mmap_file_t::skip(const char *skip_str)
{
	size_t count = 0;

	while (!eof() && (strchr(skip_str, *m_buff) != NULL)) {
		skip();
		count++;
	}

	return count;
}

void mmap_file_t::skip_until(char until_ch)
{
	if (eof())
		throw std::system_error(EINVAL, std::generic_category(), "Failed finding character");
	while (*m_buff != until_ch) {
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
	while (*m_buff != until_ch) {
		hash = hash_next(*m_buff, hash);
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
	while (strchr(until_str, *m_buff) == NULL) {
		hash = hash_next(*m_buff, hash);
		count++;
		skip();
		if (eof())
			throw std::system_error(EINVAL, std::generic_category(), "Failed finding character");
	}

	hash = hash_finish(hash);

	return count;
}

// This function will move the current character one step forward. This function
// is called by all other functions for moving the current character pointer.
void mmap_file_t::skip(void)
{
	if (eof())
		throw std::system_error(EPIPE, std::generic_category(), "mmap_file_t::skip");
	if ((*m_buff) == '\n') {
		m_line++;
		m_col = 1;
		m_start = m_buff + 1;
	} else if ((*m_buff) == '\t') {
		m_col += m_env.spaces_per_tab;
	} else {
		m_col++;
	}
	m_buff++;
}

size_t strsz(const char * str, char find, const char * end)
{
	size_t idx = 0;
	
	while (str != end) {
		if (str[idx] == find)
			return idx;
		idx++;
	}

	return idx;
}

const position_t mmap_file_t::get_position(void) const noexcept
{
	std::string line(m_start, strsz(m_start, '\n', m_end));
	position_t position(line, this, m_line, m_col);
	return position;
}

std::string mmap_file_t::marker_end(void)
{
	std::string str(m_marker_start, m_buff - m_marker_start);
	return str;
}
