/*
    sbucket.c

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

#include <random>
#include <algorithm>
#include <gtest/gtest.h>
#include "environment.hh"

namespace {
	class test_sbucket : public ::testing::Test {
	protected:
		const size_t m_max_size = 1024;
		virtual void SetUp() {
			std::default_random_engine r;
			std::normal_distribution<float> n(5.0, 5.0);
			std::uniform_int_distribution<unsigned> u(32, 126);
			for (size_t i = 0; i < m_nr_strings; i++) {
				const size_t size = std::min(
					static_cast<size_t>(n(r)), m_max_size);
				hasher_t hash;
				for (size_t c = 0; c < size; c++) {
					const char ch = static_cast<char>(u(r));
					m_strings[i].push_back(ch);
					hash.next(ch);
				}
				m_hash[i] = hash;
			}
		}

		static const size_t m_nr_strings = 32 * 1024;
		std::array<std::string, m_nr_strings> m_strings;
		std::array<hash_t, m_nr_strings> m_hash;
		environment_t m_env;
	};

	TEST_F(test_sbucket, find_add_hashed) {
		std::array<sbucket_idx_t, m_nr_strings> str_idx;

		for (size_t i = 0; i < m_nr_strings; i++) {
			str_idx[i] = 
				m_env.sbucket().find_add_hashed(
					m_strings[i].c_str(),
					m_strings[i].size(),
					m_hash[i]);
		}

		for (size_t i = 0; i < m_nr_strings; i++) {
			EXPECT_EQ(str_idx[i],
				  m_env.sbucket().find_add_hashed(
					  m_strings[i].c_str(),
					  m_strings[i].size(),
					  m_hash[i]));
		}
				  
	}

} // Anonymous namespace

int main(int argc, char *argv[]) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
