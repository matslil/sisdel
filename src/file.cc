/*
  Implementation of the file_t class.

  Copyright (C) 2013 Mats G. Liljegren
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
