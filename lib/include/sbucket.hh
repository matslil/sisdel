/*
  SPDX-License-Identifier: MIT

 */

#ifndef SBUCKET_H
#define SBUCKET_H

/**
 * @file
 * This file describes the string bucket interface. String buckets
 * are containers with strings that are stored with hash codes.
 * It is used to provide a unique integer value for each unique string,
 * making string handling easier.
 */

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

/**
 * String bucket, give unique strings unique IDs.
 * Each unique string is given a string ID, string_idx_t. This makes
 * string comparisons as easy as comparing two integers. Storing a string
 * only requires storing its string_idx_t.
 * This makes string handling more effective both in time and space.
 * @par
 * Tracepoints use tp_sisdel as tracepoint provider, using LTTng as
 * tracepoint framework.
 *
 * @note You can not remove strings, since the life time of string_idx_t
 *       is the life time of an sbucket object.
 *
 * @todo Should use Unicode strings with uchar32_t character type.
 */
class sbucket {
public:
	/**
	 * Default and only constructor.
	 */
	sbucket();

	/**
	 * Destructor.
	 */
	~sbucket();

	/**
	 * Translate given hashed string to string_idx_t, adding it if needed.
	 * This is the buffer version where a part of a string can be
	 * used. str parameter points as the beginning, and the str_len
	 * determines how many bytes to use.
	 * @par
	 * It is expected that the caller has calculated the hash value
	 * for the given string. This makes it more effective since there
	 * is no need for going through the string once more only to
	 * calculate the hash.
	 *
	 * @returns The string_idx_t corresponding to the given string.
	 *
	 * @todo str should be of type uchar32_t.
	 */
	string_idx_t find_add_hashed(
		const char *str, /**< Start of the string to be translated. */
		size_t str_len,  /**< String size in bytes. */
		hash_t hash      /**< Hash value for the string. */
		);

	/**
	 * Translate given hashed string to string_idx_t, adding if needed.
	 * Use the whole given string and translate it to a string_idx_t,
	 * adding it if it is unique.
	 * @par
	 * It is expected that the caller has calculated the hash value
	 * for the given string. This makes it more effective since there
	 * is no need for going through the string once more only to
	 * calculate the hash.
	 *
	 * @returns The string_idx_t corresponding to the given string.
	 *
	 * @todo str should be of 32-bit Unicode string type.
	 */
	string_idx_t find_add_hashed(
		const std::string& str, /**< String to be translated. */
		hash_t hash             /**< String hash value. */
		)
		{ return find_add_hashed(str.c_str(), str.length(), hash); }

	/**
	 * Translate given hashed string to string_idx_t, adding if needed.
	 * Use the whole given string and translate it to a string_idx_t,
	 * adding it if it is unique.
	 * @par
	 * This function will calculate the hash value and then use
	 * find_add_hashed() function.
	 *
	 * @returns The string_idx_t corresponding to the given string.
	 *
	 * @todo str should be of 32-bit Unicode string type.
	 */
	string_idx_t find_add(
		const char *str /**< String to be translated. */
		);

	/**
	 * Return string content given its string_idx_t value.
	 *
	 * @returns String corresponding to the given string_idx_t.
	 *
	 * @todo Returned string should be Unicode uchar32_t string.
	 */
	const char *operator[](string_idx_t idx) const;

	/**
	 * Copy constructor.
	 */
	sbucket(const sbucket &other) = default;

	/**
	 * Move constructor.
	 */
	sbucket(sbucket &&tmp) = default;

	/**
	 * Copy operator.
	 */
	sbucket& operator=(const sbucket &other) = default;

	/**
	 * Move operator.
	 */
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
