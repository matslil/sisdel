/*
  This file implements the unit test for the mmap_file_t class
  
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

