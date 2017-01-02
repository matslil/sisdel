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

class mmap_t;

class mmap_file_t {
public:
	mmap_file_t(environment_t &env, sbucket_idx_t name);
	mmap_file_t(mmap_file_t &&from);
	~mmap_file_t() {}
	mmap_file_t& operator=(mmap_file_t &&from);

	constexpr sbucket_idx_t name() const noexcept { return m_name; }
	constexpr size_t buf_size() const noexcept { return m_map.file_size(); }

	constexpr const char *str() const noexcept { return m_currpos; }
	char get();
	constexpr char peek(void) const noexcept
		{ eof() ? '\0' : *m_currpos; }
	constexpr bool eof(void) const noexcept (void)
		{ return m_currpos >= m_endpos; }
	size_t skip(char skip_ch);
	size_t skip(char *skip_str);
	void skip_until(char until_ch);
	size_t skip_until_hashed(char until_ch, hash_t& hash);
	size_t skip_until_hashed(const char* until_str, hash_t& hash);
	void skip(void);

	constexpr const position_t& get_position const noexcept (void)
		{ return m_pos;}
	void set_position(const position_t& pos) { m_pos = pos; }
	size_t distance(const position_t& from, const position_t& to);
	
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
		constexpr const void *map(void) const noexcept { return m_map; }
		constexpr size_t file_size(void) const noexcept { return m_file.size(); }

		// Forbidden methods
		mmap_t() = delete;
		mmap_t(const mmap_t&) = delete;
		mmap_t& operator=(const mmap_t&) = delete;

	private:
		const file_t m_file;
		const void *m_map;
	};

	mmap_t m_map;
	position_t m_pos;
	const char * const m_endpos;
};


#endif // MMAP_FILE_H
