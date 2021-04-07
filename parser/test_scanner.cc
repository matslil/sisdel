/*
  This file implements the unit test for the tokenizer_t class.
  
  Copyright 2017 Mats Liljegren.
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

#include <system_error>
#include <iostream>
#include "token.hh"

int main(int argc, const char *argv[])
{
	if (argc != 2) {
		std::cerr << "Expected file name to be parsed! Aborting.\n";
		return 1;
	}

	environment_t e;
	tokenizer_t lexer(e, argv[1]);
	
	try {
		for (const token_t* t = lexer.next();
		     t != NULL;
		     t = lexer.next()) {
			std::cout << *t;
			if (typeid(*t) == typeid(token_eol_t))
				std::cout << '\n';
			else
				std::cout << ' ';
			delete t;
		}
	}

	catch (const parser_error& e) {
		std::cerr << "\nParser error: " << e.what() << "\n";
		return 2;
	}
	
	catch (const std::system_error& e) {
		std::cerr << "\nSystem error: " << e.what() << "\n";
		return 3;
	}

	catch (const std::runtime_error& e) {
		std::cerr << "\nRuntime error: " << e.what() << "\n";
		return 4;
	}

	catch (...) {
		std::cerr << "\nUnknown error " << "\n";
		return 5;
	}

	std::cout << '\n';
	
	return 0;
}
