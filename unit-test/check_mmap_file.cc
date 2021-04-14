/*
  This file implements the unit test for the mmap_file_t class
  
  SPDX-License-Identifier: MIT

 */

#include <catch2/catch.hpp>
#include "mmap_file.hh"

TEST_CASE("test_mmap_file:file_not_found") {
	environment_t m_env;
	REQUIRE_THROWS_AS(mmap_file_t(m_env, "non-existing-file"), std::system_error);
}

TEST_CASE("test_mmap_file:skip") {
	environment_t env;
	mmap_file_t file(env, "check_mmap_file.data");

	// Check that we are at the beginning of the file
	const std::string line1("# Some test data used for the check_mmap_file unit test");
	REQUIRE(file.get_position().str() == line1);

	// Test skip_until(chr)
	file.skip_until('\n');

	// Test skip(chr)
	// Skip all new-lines, expect two of them
	REQUIRE(static_cast<size_t>(2) == file.skip('\n'));

	// Check that we are at line 3
	const std::string line3("adf lkjdsf asdfkdsafjh sakdfhkjashfd");
	REQUIRE(file.get_position().str() == line3);
	REQUIRE(static_cast<size_t>(3) ==
			file.get_position().line());
	REQUIRE(static_cast<size_t>(1) ==
			file.get_position().column());

	// Test skip(str)
	// Check that skipping all characters on current line takes us
	// to the new-line character
	REQUIRE(line3.length() == file.skip(line3.c_str()));
	REQUIRE(static_cast<size_t>(3) ==
			file.get_position().line());
	REQUIRE(line3.length()+1 == file.get_position().column());
}

