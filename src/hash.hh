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
#include "error.hh"

typedef uint32_t hash_t;

class hasher_t {
public:
	hasher_t() : m_hash(0), m_finished(false) {}
	
	hasher_t& next(char chr)
		{
			if (m_finished)
				throw std::system_error(
					error::internal_error,
					error_category(),
					"attempted to update finalized hash "
					"code");
			m_hash += (hash_t) chr;
			m_hash += (m_hash << 10);
			m_hash ^= (m_hash >> 6);

			return *this;
		}

	operator hash_t()
		{
			if (!m_finished) {
				m_finished = true;
				m_hash += (m_hash << 3);
				m_hash ^= (m_hash >> 11);
				m_hash += (m_hash << 15);
			}
			return m_hash;
		}			

private:
	hash_t m_hash;
	bool   m_finished;
};

#endif /* HASH_H */
