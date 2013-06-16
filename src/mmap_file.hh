/*

Memory mapped file for the token parser.

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

#ifndef MMAP_FILE_H
#define MMAP_FILE_H

#include "sbucket.hh"
#include "mmap.hh"
#include "environment.hh"

class mmap_file_t {
public:
	mmap_file_t(environment_t &env, sbucket_idx_t name);
	mmap_file_t(mmap_file_t &&from);
	virtual ~mmap_file_t() {}
	virtual mmap_file_t& operator=(mmap_file_t &&from);
	virtual const char *buf() const
		{
			return static_cast<const char *>(m_map.map());
		}
	virtual sbucket_idx_t name() const { return m_name; }
	virtual size_t buf_size() const { return m_map.size(); }

	// Forbidden methods
	mmap_file_t() = delete;
	mmap_file_t(const mmap_file_t &) = delete;
	mmap_file_t& operator=(const mmap_file_t &) = delete;

private:
	sbucket_idx_t m_name;
	mmap_t m_map;
};


#endif // MMAP_FILE_H
