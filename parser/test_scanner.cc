/*
  This file implements the unit test for the tokenizer_t class.
  
  SPDX-License-Identifier: MIT

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
