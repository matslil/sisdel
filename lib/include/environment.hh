/*
  Defines the execution environment for Sisdel.

  SPDX-License-Identifier: MIT

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
