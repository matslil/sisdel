/*
  This file contains hash functions for hashing short strings.
  The algorithm is Bob Jenkin's one-at-a-time hash, which produces good
  hash values for short strings.

  SPDX-License-Identifier: MIT
  */

#ifndef HASH_H
#define HASH_H

#include <stdint.h>

typedef uint32_t hash_t;

static inline hash_t hash_next(char chr, hash_t hash)
{
	hash += (hash_t) chr;
	hash += (hash << 10);
	hash ^= (hash >> 6);
	return hash;
}

template <typename T>
static inline hash_t hash_next(T data, hash_t hash)
{
	const char * const byte = reinterpret_cast<const char*>(&data);
	for (size_t idx = 0; idx < sizeof(T); idx++)
		hash = hash_next(byte[idx], hash);
	return hash;
}

static inline hash_t hash_finish(hash_t hash)
{
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return hash;
}


#endif /* HASH_H */
