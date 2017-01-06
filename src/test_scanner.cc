/*
    This file implements the unit test for the mmap_file_t class
  
    Copyright 2017 Mats Liljegren.

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
		for (;;) {
			const token_t t = lexer.next();
			std::cout << t;
			if (t.type() == token_t::eol)
				std::cout << '\n';
			else
				std::cout << ' ';
			if (t.type() == token_t::eof)
				break;
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
