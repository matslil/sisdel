/*

Parsing scope.

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

#ifndef SCOPE_H
#define SCOPE_H

#include "__cplusplus.h"

#include "file.h"
#include "sbucket.h"

class parser_t;

enum IdentifierType {
	IDENTIFIER_SCOPE
};

struct IdentifierCommon {
	enum IdentifierType type;
	struct FilePosition defined_at;
	SbucketIdx name;
	union Identifier *parent;
	union Identifier *next;
};

union Identifier {
	struct IdentifierCommon common;
};	

struct Scope {
	union Identifier *namespace;
	union Identifier *context;
	struct ScopeParser *parser;
};

class scope_t {
	scope_t(const string_idx_t scope_name) : name(scope_name) {}

	const string_idx_t name;
};

class parser_scope_t : public scope_t {
	friend class parser_t;

public:
	
private:
	file_t m_file;
};

class current_scope_t {
private:
	std::stack<unique_ptr<scope_t *>> m_scope;
};

thread_local current_scope_t current_scope;

#endif /* SCOPE_H */
