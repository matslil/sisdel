/*
  sbucket.h

  This file describes the string bucket interface. String buckets
  are containers with strings that are stored with hash codes.
  
  Copyright 2012-2017 Mats Liljegren.
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
typedef size_t string_idx_t;

class sbucket {
public:
	// Constructor
	sbucket();

	// Destructor
	~sbucket();

	string_idx_t find_add_hashed(const char *str,
				      size_t str_len,
				      hash_t hash);
	string_idx_t find_add_hashed(const std::string& str,
				     hash_t hash)
		{ return find_add_hashed(str.c_str(), str.length(), hash); }
	string_idx_t find_add(const char *str);

	const char *operator[](string_idx_t idx) const;

	// Defaults
	sbucket(const sbucket &other) = default;
	sbucket(sbucket &&tmp) = default;
	sbucket& operator=(const sbucket &other) = default;
	sbucket& operator=(sbucket &&tmp) = default;

private:
	class idx_entry {
	public:
		idx_entry(string_idx_t idx, const char *str, size_t str_len)
			: m_idx(idx), m_next_idx(-1), m_str(str, str_len)
			{}

		// Index of this entry
		const string_idx_t m_idx;

		// Index to next entry if >= 0, or end of chain if < 0
		ssize_t m_next_idx;

		// The string
		const std::string m_str;
	};

	// Number of buckets. Needs to be a power of two.
	static const size_t m_nr_buckets = 4096;

	// The last bits of the hash code translates into a bucket.
	// A bucket is a linked lists of strings sharing that last bit
	// of the hash code.
	std::array<ssize_t, m_nr_buckets> m_buckets;

	// Translate string_idx_t -> m_hash index
	std::vector<idx_entry> m_entry;
};

#endif /* SBUCKET_H */
