/*
    sbucket.h

    This file describes the string bucket interface. String buckets
    are containers with strings that are stored with hash codes.
  
    Copyright 2012-2013 Mats Liljegren.

    This file is part of Sisdel.

    Sisdel is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3
    as published by the Free Software Foundation.

    Sisdel is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Sisdel.  If not, see <http://www.gnu.org/licenses/>.

 */

#ifndef SBUCKET_H
#define SBUCKET_H

#include <memory>
#include <vector>
#include <string>
#include <array>
#include "hash.hh"
#include "assert.h"

/**
 * Integer index for strings.
 * Equivalent indexes are guaranteed to be equivalent strings, and
 * vice versa. Negative values are for reserved token names, such as
 * new-line, end-of-file, parenthesis, brackets, etc.
 */
typedef ssize_t sbucket_idx_t;

class sbucket_t {
public:
	// Constructor
	sbucket_t();

	// Destructor
	~sbucket_t();

	sbucket_idx_t find_add_hashed(const char *str,
				      size_t str_len,
				      hash_t hash);
	sbucket_idx_t find_add(const char *str);

	const char *operator[](sbucket_idx_t idx);

	// Defaults
	sbucket_t(const sbucket_t &other) = default;
	sbucket_t(sbucket_t &&tmp) = default;
	sbucket_t& operator=(const sbucket_t &other) = default;
	sbucket_t& operator=(sbucket_t &&tmp) = default;

private:
	class idx_entry_t {
	public:
		idx_entry_t(const char *str, size_t str_len)
			: m_hash_next(-1), m_str(str, str_len) {}
		idx_entry_t& hash_next(sbucket_idx_t idx) { assert(m_hash_next == -1); m_hash_next = idx; return *this; }
		sbucket_idx_t hash_next() const { return m_hash_next; }
		const std::string& str() const { return m_str; }

		// Defaults
		idx_entry_t(idx_entry_t &other) = delete;
		idx_entry_t(idx_entry_t &&tmp) 
			: m_hash_next(tmp.m_hash_next), m_str(std::move(tmp.m_str)) {}
		idx_entry_t& operator=(idx_entry_t &other) = delete;
		idx_entry_t& operator=(idx_entry_t &&tmp) {
			m_hash_next = tmp.m_hash_next;
			m_str = std::move(tmp.m_str);
			return *this;
		}

	private:
		sbucket_idx_t m_hash_next;
		std::string m_str;
	};

	static const size_t m_hash_entries = 4096;

	std::array<sbucket_idx_t, m_hash_entries> m_hash;
	sbucket_idx_t m_idx_next;
	std::vector<idx_entry_t> m_idx;
};

#endif /* SBUCKET_H */
