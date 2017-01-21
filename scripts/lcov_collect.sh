#!/bin/sh -eu

# Copyright (C) 2017 Mats G. Liljegren
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

cd ..
lcov --directory . --capture --output-file coverage.info.new
if [ -f coverage.info ]; then
    lcov --directory . --add-tracefile coverage.info --add-tracefile coverage.info.new --output-file coverage.info
    rm coverage.info.new
else
    mv coverage.info.new coverage.info
fi
