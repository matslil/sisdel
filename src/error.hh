/*
  This file defines the parser_error exception.

  Copyright (C) 2017 Mats G. Liljegren
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

#ifndef ERROR_HH
#define ERROR_HH


#include <exception>

#include "position.hh"
#include "sbucket.hh"
#include "environment.hh"

class parser_error : public std::exception {
public:
	parser_error(const position_t& token_start,
		     const position_t& error_at, const std::string& msg);

	const char *what() const noexcept { return m_what.c_str(); }

private:
	std::string m_what;
};

#endif // ERROR_HH
