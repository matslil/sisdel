/*
  Translate strings into string indexes. For each unique string, there is a
  correspondingly unique index. This makes string comparisons as simple as
  comparing indexes.
  
  SPDX-License-Identifier: MIT

 */

#include <utility>
#include "sbucket.hh"
#include "hash.hh"

/*
 * Public function implementation
 */

sbucket::sbucket()
	: m_entry()
{
	m_buckets.fill(-1);
}

sbucket::~sbucket()
{
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
	return str;
}
