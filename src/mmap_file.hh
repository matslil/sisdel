/*
  Memory mapped file for the token parser.

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

#ifndef MMAP_FILE_H
#define MMAP_FILE_H

#include "file.hh"
#include "sbucket.hh"
#include "environment.hh"
#include "error.hh"
#include "position.hh"

class position_t;

class mmap_file_t {
public:
	mmap_file_t(environment_t &env, const char *name);

	mmap_file_t& operator=(mmap_file_t &&from);

	constexpr size_t buf_size() const noexcept { return m_map.file_size(); }

//	constexpr const char *str() const noexcept { return m_pos.m_buff; }
	constexpr char peek(void) const noexcept
		{ return eof() ? '\0' : *m_buff; }
	constexpr bool eof(void) const noexcept
		{ return m_buff >= m_end; }
	size_t skip(char skip_ch);
	size_t skip(const char *skip_str);
	void skip_until(char until_ch);
	size_t skip_until_hashed(char until_ch, hash_t& hash);
	size_t skip_until_hashed(const char* until_str, hash_t& hash);
	void skip(void);

	const position_t get_position(void) const noexcept;

	constexpr const char * filename(void) const
		{ return m_env.sbucket()[m_filename]; }

	void marker_start(void) { m_marker_start = m_buff; }
	std::string marker_end(void);
	
	// Forbidden methods
	mmap_file_t() = delete;
	mmap_file_t(const mmap_file_t &) = delete;
	mmap_file_t& operator=(const mmap_file_t &) = delete;

private:
	
	// Wrap map/munmap to make deallocation work with exceptions
	class mmap_t {
	public:
		mmap_t(const char *name);
		~mmap_t();
		constexpr const char *map(void) const noexcept
			{ return m_map; }
		constexpr size_t file_size(void) const noexcept
			{ return m_file.size(); }

		// Forbidden methods
		mmap_t() = delete;
		mmap_t(const mmap_t&) = delete;
		mmap_t& operator=(const mmap_t&) = delete;

	private:
		const file_t m_file;
		const char * const m_map;
	};

	environment_t& m_env;
	mmap_t m_map;
	const char * m_buff;
	const char * m_start;
	const char * const m_end;
	size_t m_col;
	size_t m_line;
	const string_idx_t m_filename;
	const char * m_marker_start;
};


#endif // MMAP_FILE_H
