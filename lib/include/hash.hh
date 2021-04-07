/*
  This file contains hash functions for hashing short strings.
  The algorithm is Bob Jenkin's one-at-a-time hash, which produces good
  hash values for short strings.

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
