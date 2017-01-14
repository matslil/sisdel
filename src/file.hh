/*
  File operations.

  Represents an opened file descriptor. The purpose of the class is primarily
  to ensure the file is closed when the file descriptor goes out of scope.

  Copyright (C) 2013-2017 Mats G. Liljegren
  SPDX-License-Identifier: Apache-2.0

  This file is part of Sisdel.

  Licensed under the Apache License, Version 2.0 (the "License"); you may not
  use this file except in compliance with the License. You may obtain a copy
  of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
  License for the specific language governing permissions and limitations
  under the License.
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
