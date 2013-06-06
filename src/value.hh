/*

Base class for language identities. This type is used to denote "any type".

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

#ifndef VALUE_H
#define VALUE_H

enum class type_t {
	t_none = 0,
	t_new = 1,
	t_namespace = 1,
	t_restriction = 2,
	t_function = 4,
	t_string = 8,
	t_integer = 16,
	t_float = 32,
	t_aggregate = 64,
	t_unit = 128
};

class value_t {
public:
	virtual ~value_t() = 0;

	static const type_t type_any_valid =
		type_namespace | type_restriction | type_function
		| type_string  | type_integer | type_float | type_aggregate
		| type_unit;
	
private:
	type_t m_type;
};

#endif // VALUE_H
