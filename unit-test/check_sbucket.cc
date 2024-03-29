/*
  This file implements the test for the string bucket interface.
  
  SPDX-License-Identifier: MIT

*/

#include <random>
#include <algorithm>
#include <catch2/catch.hpp>
#include "environment.hh"

TEST_CASE("sbucket") {
	constexpr size_t m_max_size = 1024;
	// Number of strings. Becomes * 2 since we have two variants
	constexpr size_t m_nr_strings = 1024;

	// The actual strings with random characters
	std::array<std::string, m_nr_strings> m_strings[2];

	// Hash is only used for m_string[0]
	std::array<hash_t, m_nr_strings> m_hash;

	environment_t m_env;

	std::default_random_engine r;
	std::normal_distribution<float> n(5.0, 5.0);
	std::uniform_int_distribution<unsigned> u(32, 126);

	// Variant 0 is for the find_add_hashed test case
	// Variant 1 is for the find_add test case
	for (size_t variant = 0; variant < 2; variant++) {
		for (size_t i = 0; i < m_nr_strings; i++) {
			std::string test_str;
			if (i == 0)
				test_str = "token1";
			else if (i == 1)
				test_str = "token2";
			else {

				// Determine length of the string
				const size_t size = std::min(
						static_cast<size_t>(n(r)), m_max_size);

				// Hash is only used for variant 0
				for (size_t c = 0; c < size; c++) {
					// Random character
					const char ch = static_cast<char>(u(r));
					test_str.push_back(ch);
				}
			}
			// Store test string
			m_strings[variant][i] = test_str;

			// Calculate hash
			if (variant == 0) {
				hash_t hash = 0;
				for (size_t c = 0; c < test_str.length(); c++) {
					// Random character
					const char ch = static_cast<char>(u(r));
					if (variant == 0)
						hash = hash_next(ch, hash);
				}
				m_hash[i] = hash_finish(hash);
			}
		}
	}

	// Test find_add_hashed(), also uses operator[] for verification.
	// Uses variant 0 of the strings generated by the setup.
	SECTION( "find_add_hashed" ) {
		std::array<string_idx_t, m_nr_strings> str_idx;

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

	// Test find_add(), also uses operator[] for verification.
	// Uses variant 1 of the strings generated by the setup.
	SECTION("find_add") {
		std::array<string_idx_t, m_nr_strings> str_idx;

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

}

