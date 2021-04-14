/*
  File operations.

  Represents an opened file descriptor. The purpose of the class is primarily
  to ensure the file is closed when the file descriptor goes out of scope.

  SPDX-License-Identifier: MIT

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
