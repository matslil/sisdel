/*
    hash.h

    This file contains hash functions for hashing short strings.
    The algorithm is Bob Jenkin's one-at-a-time hash.

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

#ifndef HASH_H
#define HASH_H

#include <stdint.h>

typedef uint32_t hash_t;

static inline hash_t hash_next(char chr, hash_t hash) { hash += (hash_t) chr; hash += (hash << 10); hash ^= (hash >> 6); return hash; }

static inline hash_t hash_finish(hash_t hash) { hash += (hash << 3); hash ^= (hash >> 11); hash += (hash << 15); return hash; }

#endif /* HASH_H */
