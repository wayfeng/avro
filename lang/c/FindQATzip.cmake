#
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.
#
# Tries to find Snappy headers and libraries.
#
# Usage of this module as follows:
#
#  find_package(QATzip)
#
# Variables used by this module, they can change the default behaviour and need
# to be set before calling find_package:
#
#  QZ_ROOT_DIR  Set this variable to the root installation of
#               QATzip if the module has problems finding
#               the proper installation path.
#
# Variables defined by this module:
#
#  QZ_FOUND              System has QATzip libs/headers
#  QZ_LIBRARIES          The QATzip libraries
#  QZ_INCLUDE_DIR        The location of QATzip headers

find_path(QZ_INCLUDE_DIR
    NAMES qatzip.h
    HINTS ${QZ_ROOT_DIR}/include)

find_library(QZ_LIBRARIES
    NAMES qatzip
    HINTS ${QZ_ROOT_DIR}/lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(QATzip DEFAULT_MSG
    QZ_LIBRARIES
    QZ_INCLUDE_DIR)

mark_as_advanced(
    QZ_ROOT_DIR
    QZ_LIBRARIES
    QZ_INCLUDE_DIR)
