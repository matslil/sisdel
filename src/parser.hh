/*

Parser.

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

#ifndef PARSER_HH
#define PARSER_HH

#include <map>

#include "sbucket.hh"

class value_t {
public:
	enum type_t {
		unknown_type,    // Only used to implement error recovery
		string_type,
		integer_type,
		float_type,
		namespace_type,
		function_type,
		collection_type,
		restriction_type
	};

	virtual ~value_t() {}
	virtual type_t type() const = 0;
};

class string_t : public value_t {
public:
	string_t(sbucket_idx_t str, sbucket_idx_t unit)
		: m_str(str), m_unit(unit) {}

	// From value_t
	virtual type_t type() const { return type_t::string_type; }

	// String only
	sbucket_idx_t value() const { return m_str; }
	sbucket_idx_t unit() const { return m_unit; }

private:
	sbucket_idx_t m_str;
	sbucket_idx_t m_unit;
};

class integer_t : public value_t {
	integer_t(intmax_t value, sbucket_idx_t unit)
		: m_value(value), m_unit(unit) {}

	// From value_t
	virtual type_t type() const { return type_t::integer_type; }

	// Integer only
	intmax_t value() const { return m_value; }
	sbucket_idx_t unit() const { return m_value; }

private:
	intmax_t m_value;
	sbucket_idx_t m_unit;
};

class namespace_t : public value_t {
public:
	virtual type_t type() const { return type_t::namespace_type; }
	virtual std::shared_ptr<value_t> find(sbucket_idx_t identifier)
		{ return m_value[identifier]; }
	virtual void add(sbucket_idx_t as, std::shared_ptr<value_t> value)
		{ m_value[as] = value; }

private:
	std::map<sbucket_idx_t, std::shared_ptr<value_t> > m_value;
};

class collection_t : public namespace_t {
public:
	virtual type_t type() const { return type_t::collection_type; }
};

class function_t : public namespace_t {
public:
	virtual type_t type() const { return type_t::function_type; }
};

class restriction_t : public value_t {
	
};

#endif // PARSER_HH
