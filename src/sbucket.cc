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
 * Internal helper functios
 */

static inline size_t max(size_t a, size_t b) {
	return (a > b) ? a : b;
}

/*
 * Public function implementation
 */

sbucket_t::sbucket_t()
	: m_idx_next(0),
	  m_idx()
{
	m_hash.fill(-1);
	tracepoint(tp_sisdel, tp_sbucket_init, this);
}

sbucket_t::~sbucket_t()
{
	tracepoint(tp_sisdel, tp_sbucket_destroy, this);
}

sbucket_idx_t sbucket_t::find_add_hashed(const char *str,
				    size_t str_len,
				    hash_t hash)
{
	sbucket_idx_t idx = m_hash[hash % m_hash_entries];
	
	while (idx >= 0) {
		if (m_idx[idx].str().compare(0, str_len, str) == 0) {
			tracepoint(tp_sisdel, tp_sbucket_find, hash, str, str_len, idx);
			return idx;
		}
		idx = m_idx[idx].hash_next();
	}

	/*
	 * Not found, add.
	 */

	idx = m_idx.size();

	{
		idx_entry_t new_entry(str, str_len);
		m_idx.push_back(std::move(new_entry));
	}

	/* Update hash entry */
	sbucket_idx_t hash_idx = m_hash[hash % m_hash_entries];

	if (hash_idx < 0) {
		/* First entry in hash array */
		m_hash[hash % m_hash_entries] = idx;
	} else {	
		/* Move to end of hash chain */
		while (m_idx[hash_idx].hash_next() >= 0)
			hash_idx = m_idx[hash_idx].hash_next();

		/* Add entry to hash chain */
		m_idx[hash_idx].hash_next(idx);
	}

	tracepoint(tp_sisdel, tp_sbucket_add, hash, str, str_len, idx);

	return idx;
}

sbucket_idx_t sbucket_t::find_add(const char *str)
{
	hash_t hash = 0;
	size_t str_len = 0;
	
	for (; *str != '\0'; str++) {
		hash = hash_next(*str, hash);
		str_len++;
	}
	
	hash = hash_finish(hash);

	return find_add_hashed(str, str_len, hash);
}

const char *sbucket_t::operator[](sbucket_idx_t idx) {
	const char * const str = m_idx[idx].str().c_str();
	tracepoint(tp_sisdel, tp_sbucket_index, idx, str);
	return str;
}
