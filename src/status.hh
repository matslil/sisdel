/*

Error return codes for Sisdel functions.

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

#ifndef STATUS_H
#define STATUS_H

#include "__cplusplus.h"

C_START

enum Status {
	/* Success codes */
	status_success = 0,
	status_end_of_file = 1,

	/* Fail codes */
	status_out_of_memory = -1,
	status_fileop_failed = -2
};

C_END

#include <stdio.h>
#include "sbucket.h"

C_START

struct Scope;

enum Status report_out_of_memory(struct Scope *scope, size_t alloc_size);

enum Status report_fileop_failed(struct Scope *scope,
				 const char *file_name,
				 SbucketIdx lang_name,
				 int errno_value,
				 const char *operation);


C_END

#endif /* STATUS_H */
