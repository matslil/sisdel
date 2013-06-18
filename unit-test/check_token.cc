/*
    This file contains a test for tokenizer_t class.
  
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

#include <gtest/gtest.h>
#include "environment.hh"
#include "token.hh"


void PrintTo(const token_t& token, ::std::ostream* os) {
	token.print(*os);
}

template<typename T, size_t size>
size_t array_size(T(&)[size])
{
	return size;
}

namespace {
	class test_token : public ::testing::Test {
	public:
		token_t make_identifier(
			const char *name,
			size_t line,
			size_t column,
			std::error_code ec = success_code)
			{
				token_t token(m_env);
				
				token.set_pos(line, column).set_identifier(
					m_env.sbucket().find_add(
						name, strlen(name)),
					ec);
				return token;
			}
		
		token_t make_integer(
			intmax_t value,
			const char *unit,
			size_t line,
			size_t column,
			std::error_code ec = error::make_error_code(
				error::no_error))
			{
				token_t token(m_env);

				token.set_pos(line, column).set_integer_constant(
					value,
					m_env.sbucket().find_add(
						unit, strlen(unit)),
					ec);
				return token;
			}

		token_t make_float(
			double value,
			const char *unit,
			size_t line,
			size_t column,
			std::error_code ec = error::make_error_code(
				error::no_error))
			{
				token_t token(m_env);
				
				token.set_pos(line, column).set_float_constant(
					value,
					m_env.sbucket().find_add(
						unit, strlen(unit)),
					ec);
				return token;
			}

		token_t make_string(
			const char *str,
			const char *unit,
			size_t line,
			size_t column,
			std::error_code ec = error::make_error_code(
				error::no_error))
			{
				token_t token(m_env);
				
				token.set_pos(line, column).set_string_constant(
					m_env.sbucket().find_add(
						str, strlen(str)),
					m_env.sbucket().find_add(
						unit, strlen(unit)),
					ec);
				return token;
			}

		void check_parser(const char *the_code,
				  const token_t expected_tokens[],
				  size_t nr_tokens)
			{
				tokenizer_t tokenizer(m_env, the_code,
						      strlen(the_code));
		
				for (size_t idx = 0; idx < nr_tokens; idx++) {
					EXPECT_EQ(tokenizer.next(),
						  expected_tokens[idx])
						<< "iteration " << idx;
				}
			}

	protected:
		virtual void SetUp() {
		}

		environment_t m_env;
		static const std::error_code success_code;
	};

	TEST_F(test_token, test_identifiers) {
		const char * const the_code =
			"identifier-number-1 identifier-number-2";
		const token_t expected_tokens[] = {
			make_identifier("identifier-number-1", 1, 1),
			make_identifier("identifier-number-2", 1, 21)
		};

		check_parser(the_code, expected_tokens,
			     array_size(expected_tokens));
	}

	TEST_F(test_token, test_continuation) {
		const char * const the_code =
			"some tokens\n... that are continued\n";
		const token_t expected_tokens[]  = {
			make_identifier("some", 1, 1),
			make_identifier("tokens", 1, 6),
			make_identifier("that", 2, 5),
			make_identifier("are", 2, 10),
			make_identifier("continued", 2, 14)
		};

		check_parser(the_code, expected_tokens,
			     array_size(expected_tokens));
	}

	TEST_F(test_token, test_continuation2) {
		const char * const the_code =
			"some tokens\n # Hmmm ...\n  \t... that are continued\n";
		const token_t expected_tokens[]  = {
			make_identifier("some", 1, 1),
			make_identifier("tokens", 1, 6),
			make_identifier("that", 3, 8),
			make_identifier("are", 3, 13),
			make_identifier("continued", 3, 17)
		};

		check_parser(the_code, expected_tokens,
			     array_size(expected_tokens));
	}

	const std::error_code test_token::success_code;
} // Anonymous namespace

int main(int argc, char *argv[]) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
