/*
    sbucket.cc

    This file implements the string bucket interface.
  
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

#include <utility>
#include <lttng/tracepoint.h>
#include "sisdel_tracepoints.h"
#include "sbucket.hh"
#include "hash.hh"

/*
 * Public function implementation
 */

sbucket::sbucket()
	: m_entry()
{
	m_buckets.fill(-1);
	tracepoint(tp_sisdel, tp_sbucket_init, this);
}

sbucket::~sbucket()
{
	tracepoint(tp_sisdel, tp_sbucket_destroy, this);
}

string_idx_t sbucket::find_add_hashed(const char *str,
				    size_t str_len,
				    hash_t hash)
{
	const size_t bucket_idx = hash % m_nr_buckets;
	ssize_t idx;

	for (idx = m_buckets[bucket_idx];
	     idx >= 0;
	     idx = m_entry[idx].m_next_idx) {
		if ((m_entry[idx].m_str.compare(0, str_len, str, str_len) == 0)
		    && (m_entry[idx].m_str.length() == str_len)) {
			tracepoint(tp_sisdel, tp_sbucket_find, hash, str, str_len, idx);
			return idx;
		}
	}

	/*
	 * Not found, add.
	 */

	// Assign a new string_idx_t for the new entry
	const string_idx_t new_idx = m_entry.size();

	{
		idx_entry new_entry(new_idx, str, str_len);
		m_entry.push_back(std::move(new_entry));
	}

	if (m_buckets[bucket_idx] < 0)
		// First entry in the bucket chain
		m_buckets[bucket_idx] = new_idx;
	else {
		// Start from beginning in the bucket, and find that last
		// entry in the chain
		for (idx = m_buckets[bucket_idx];
		     m_entry[idx].m_next_idx >= 0;
		     idx = m_entry[idx].m_next_idx);
		
		// Link to previous chain in bucket chain
		m_entry[idx].m_next_idx = new_idx;
	}
	
	tracepoint(tp_sisdel, tp_sbucket_add, hash, str, str_len, new_idx);

	return new_idx;
}

string_idx_t sbucket::find_add(const char *str)
{
	hash_t hash = 0;
	size_t str_len = 0;
	const char *curr;
	
	for (curr = str; *curr != '\0'; curr++) {
		hash = hash_next(*curr, hash);
		str_len++;
	}
	
	hash = hash_finish(hash);

	return find_add_hashed(str, str_len, hash);
}

const char *sbucket::operator[](string_idx_t idx) const {
	const char * const str = m_entry[idx].m_str.c_str();
	tracepoint(tp_sisdel, tp_sbucket_index, idx, str);
	return str;
}
