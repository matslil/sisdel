/*
  SPDX-License-Identifier: MIT

*/

// Header file generated by the build system based from the template file
// config.hh.in.
#include "config.hh"

// Stop lint from complaining on unused string.
#ifndef lint

/**
 * Declare what string.
 * The what string makes it possible to determine what source code that
 * built the binary.
 */
extern const char what_str[];
const char what_str[] = "@(#)Sisdel v" STR(SISDEL_VERSION_MAJOR) "." STR(SISDEL_VERSION_MINOR) ", branch " STR(SISDEL_GIT_BRANCH) ", commit " STR(SISDEL_GIT_COMMIT_HASH);
#endif /* lint */
