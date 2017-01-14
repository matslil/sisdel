/*
    This file implements the unit test for the mmap_file_t class
  
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

#include <gtest/gtest.h>
#include "mmap_file.hh"

namespace {
	TEST(test_mmap_file, file_not_found) {
		environment_t m_env;
		EXPECT_THROW(mmap_file_t file(m_env, "non-existing-file"), std::system_error);
	}

#define MY_EXPECT_EQ(a,b) do { SCOPED_TRACE("MY_EXPECT_EQ"); expect_eq(a,b); } while(0)
	
	template <typename T>
	inline void expect_eq(const T t1, const T t2)
	{
		EXPECT_EQ(t1, t2);
	}
	
	TEST(test_mmap_file, skip) {
		environment_t env;
		mmap_file_t file(env, "check_mmap_file.data");

		// Check that we are at the beginning of the file
		const std::string line1("# Some test data used for the check_mmap_file unit test");
		MY_EXPECT_EQ(file.get_position().str(), line1);
		
		// Test skip_until(chr)
		file.skip_until('\n');
		
		// Test skip(chr)
		// Skip all new-lines, expect two of them
		MY_EXPECT_EQ(static_cast<size_t>(2), file.skip('\n'));

		// Check that we are at line 3
		const std::string line3("adf lkjdsf asdfkdsafjh sakdfhkjashfd");
		MY_EXPECT_EQ(file.get_position().str(), line3);
		MY_EXPECT_EQ(static_cast<size_t>(3),
			     file.get_position().line());
		MY_EXPECT_EQ(static_cast<size_t>(1),
			     file.get_position().column());

		// Test skip(str)
		// Check that skipping all characters on current line takes us
		// to the new-line character
		MY_EXPECT_EQ(line3.length(), file.skip(line3.c_str()));
		MY_EXPECT_EQ(static_cast<size_t>(3),
			     file.get_position().line());
		MY_EXPECT_EQ(line3.length()+1, file.get_position().column());
	}
}

int main(int argc, char *argv[]) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
