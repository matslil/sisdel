/*
    chec_sbucket.c

    This file implements the test for the string bucket interface.
  
    Copyright 2012-2017 Mats Liljegren.

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

			// Variant 0 is for the find_add_hashed test case
			// Variant 1 is for the find_add test case
			for (size_t variant = 0; variant < 2; variant++) {
				for (size_t i = 0; i < m_nr_strings; i++) {
					// Determine length of the string
					const size_t size = std::min(
						static_cast<size_t>(n(r)), m_max_size);

					// Hash is only used for variant 0
					hash_t hash = 0;
					for (size_t c = 0; c < size; c++) {
						// Random character
						const char ch = static_cast<char>(u(r));
						// Store character into string
						m_strings[variant][i].push_back(ch);
						if (variant == 0)
							hash = hash_next(ch, hash);
					}
					if (variant == 0) {
						hash = hash_finish(hash);
						m_hash[i] = hash;
					}
				}
			}
		}

		// Number of strings. Becomes * 2 since we have two variants
		static const size_t m_nr_strings = 32 * 1024;

		// The actual strings with random characters
		std::array<std::string, m_nr_strings> m_strings[2];

		// Hash is only used for m_string[0]
		std::array<hash_t, m_nr_strings> m_hash;

		environment_t m_env;
	};

	TEST_F(test_sbucket, find_add_hashed) {
		std::array<sbucket_idx_t, m_nr_strings> str_idx;

		for (size_t i = 0; i < m_nr_strings; i++) {
			str_idx[i] = 
				m_env.sbucket().find_add_hashed(
					m_strings[0][i].c_str(),
					m_strings[0][i].size(),
					m_hash[i]);
		}

		for (size_t i = 0; i < m_nr_strings; i++) {
			EXPECT_EQ(str_idx[i],
				  m_env.sbucket().find_add_hashed(
					  m_strings[0][i].c_str(),
					  m_strings[0][i].size(),
					  m_hash[i])) << "i = " << i;
		}
				  
		for (size_t i = 0; i < m_nr_strings; i++) {
			EXPECT_STREQ(
				m_env.sbucket()[str_idx[i]],
				m_strings[0][i].c_str()) << "i = " << i
							 << ", str_idx[i] = "
							 << str_idx[i];
		}
	}

	TEST_F(test_sbucket, find_add) {
		std::array<sbucket_idx_t, m_nr_strings> str_idx;

		for (size_t i = 0; i < m_nr_strings; i++) {
			str_idx[i] = 
				m_env.sbucket().find_add(
					m_strings[1][i].c_str());
		}

		for (size_t i = 0; i < m_nr_strings; i++) {
			EXPECT_EQ(str_idx[i],
				  m_env.sbucket().find_add(
					  m_strings[1][i].c_str()))
				<< "i = " << i;
		}
				  
		for (size_t i = 0; i < m_nr_strings; i++) {
			EXPECT_STREQ(
				m_env.sbucket()[str_idx[i]],
				m_strings[1][i].c_str())
				<< "i = " << i << ", str_idx[i] = "
				<< str_idx[i];
		}
	}

} // Anonymous namespace

int main(int argc, char *argv[]) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
