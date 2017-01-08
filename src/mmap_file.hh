/*

Memory mapped file for the token parser.

Copyright (C) 2013-2016 Mats G. Liljegren

This file is part of Sisdel.

Sisdel is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation, version 3.

Sisdel is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with Sisdel; see the file COPYING.  If not see
<http://www.gnu.org/licenses/>.

*/

#ifndef MMAP_FILE_H
#define MMAP_FILE_H

#include "file.hh"
#include "sbucket.hh"
#include "environment.hh"
#include "error.hh"
#include "position.hh"

class mmap_file_t {
public:
	mmap_file_t(environment_t &env, const char *name);

	mmap_file_t& operator=(mmap_file_t &&from);

	constexpr size_t buf_size() const noexcept { return m_map.file_size(); }

	constexpr const char *str() const noexcept { return m_pos.m_buff; }
	constexpr char peek(void) const noexcept
		{ return eof() ? '\0' : *m_pos.m_buff; }
	constexpr bool eof(void) const noexcept
		{ return m_pos.m_buff >= m_pos.m_end; }
	size_t skip(char skip_ch);
	size_t skip(const char *skip_str);
	void skip_until(char until_ch);
	size_t skip_until_hashed(char until_ch, hash_t& hash);
	size_t skip_until_hashed(const char* until_str, hash_t& hash);
	void skip(void);

	constexpr const position_t& get_position(void) const noexcept
		{ return m_pos;}
	void set_position(const position_t& pos) noexcept { m_pos = pos; }

	constexpr const char * filename(void) const
		{ return m_env.sbucket()[m_filename]; }
	
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
	position_t m_pos;
	const string_idx_t m_filename;
};


#endif // MMAP_FILE_H
