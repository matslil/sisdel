/*
  Defines the execution environment for Sisdel.

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

#ifndef ENVIRONMENT_HH
#define ENVIRONMENT_HH

class environment_t;

#include "sbucket.hh"

class environment_t {
public:
	environment_t() = default;
	~environment_t() = default;

	class sbucket& sbucket() { return m_sbucket; }
	const class sbucket& sbucket() const { return m_sbucket; }

	const size_t spaces_per_tab = 8;

	// I guess move should using default
	environment_t(environment_t &&) = default;
	environment_t& operator=(environment_t &&) = default;

	// Not sure how to handle copy yet
	environment_t(const environment_t &) = delete;
	environment_t& operator=(const environment_t &) = delete;

private:
	class sbucket m_sbucket;
};

#endif // ENVIRONMENT_HH
