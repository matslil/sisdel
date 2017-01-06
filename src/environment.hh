/*

Defines the execution environment for Sisdel.

Copyright (C) 2013 Mats G. Liljegren

This file is part of Sisdel.

Sisdel is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation, version 3.

Sisdel is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not see
<http://www.gnu.org/licenses/>.

*/

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

class environment_t;

#include "sbucket.hh"

class environment_t {
public:
	environment_t() = default;
	~environment_t() = default;

	sbucket_t& sbucket() { return m_sbucket; }
	const sbucket_t& sbucket() const { return m_sbucket; }

	const size_t spaces_per_tab = 8;

	// I guess move should using default
	environment_t(environment_t &&) = default;
	environment_t& operator=(environment_t &&) = default;

	// Not sure how to handle copy yet
	environment_t(const environment_t &) = delete;
	environment_t& operator=(const environment_t &) = delete;

private:
	sbucket_t m_sbucket;
};

#endif // ENVIRONMENT_H
