/*

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

#ifndef NAMESPACE_H
#define NAMESPACE_H

#include <map>
#include "sbucket.hh"
#include "value.hh"

class namespace_t : public value_t {
public:
	namespace_t();
	namespace_t(std::shared_ptr<namespace_t> parent);
	virtual ~namespace_t();

	std::shared_ptr<value_t> operator[](sbucket_idx_t name);

private:
	std::shared_ptr<namespace_t> m_parent;
	std::map<sbucket_idx_t, std::shared_ptr<value_t> > m_identifier;
};

#endif // NAMESPACE_H
