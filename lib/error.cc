/*
  Implementation of the parser_error exception class.

  Copyright (C) 2013-2017 Mats G. Liljegren
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

#include <sstream>

#include "error.hh"

static std::string repeat_spaces(size_t nr)
{
	std::string str(nr, ' ');
	return str;
}

parser_error::parser_error(const position_t& token_start, const position_t& error_at, const std::string& msg)
{
	std::stringstream ss;
	ss << token_start << ": error: " << msg
	   << "\n        " << token_start.str()
	   << "\n        " << repeat_spaces(error_at.column() - 1) << '^';

	m_what = ss.str();
}
