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

#include "status.h"
#include "scope.h"
#include "sisdel_tracepoints.h"
#include <stdio.h>
#include <errno.h>

enum Status report_out_of_memory(struct Scope *scope, size_t alloc_size)
{
	fprintf(stderr, "Error allocating %zu bytes\n", alloc_size);
	tracepoint(tp_sisdel, tp_status_out_of_memory, alloc_size);
	return status_out_of_memory;
}


enum Status report_fileop_failed(struct Scope *scope,
				 const char *file_name,
				 SbucketIdx lang_name,
				 int errno_value,
				 const char *operation) {
	fprintf(stderr, "%s: Error while opening file (%s): %s\n",
		file_name, operation, strerror(errno));
	return status_fileop_failed;
}

