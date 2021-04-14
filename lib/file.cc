/*
  Implementation of the file_t class.

  SPDX-License-Identifier: MIT

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

static size_t file_size(int fd) {
	if (fd < 0) {
		throw std::system_error(errno, std::generic_category(), "open");
	}

	const off_t end_offset = lseek(fd, 0, SEEK_END);

	if (end_offset < 0)
		throw std::system_error(errno, std::generic_category(), "seek_end");

	if (lseek(fd, 0, SEEK_SET) < 0)
		throw std::system_error(errno, std::generic_category(), "seek_set");
	
	return static_cast<size_t>(end_offset);
}

file_t::file_t(const char *name, int flags)
	: m_fd(open(name, flags)), m_size(file_size(m_fd)) {
}

file_t::~file_t() {
	close(m_fd);
}
