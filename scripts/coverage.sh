#!/bin/sh -eu

# Copyright (C) 2013-2017 Mats G. Liljegren
# SPDX-License-Identifier: Apache-2.0

# This file is part of Sisdel.

# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy
# of the License at

# http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations
# under the License.

#
# Run this script from a cmake build directory, after having built everything.
# This script will run "make test", and present code coverage based on the
# execution.
#

err () {
    printf "%s" "$*" > /dev/stderr

    exit 1
}

[ -f CMakeCache.txt ] || err Run this script from a cmake build directory.

lcov --directory . --zerocounters
make test
genhtml coverage.info

echo "Use 'firefox -safe-mode index.html' to view results"
