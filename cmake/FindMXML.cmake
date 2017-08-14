# - Find mxml
# Find the native MXML includes and library
#
#  MXML_INCLUDE_DIRS  - where to find mxml.h, etc.
#  MXML_LIBRARIES     - List of libraries when using mxml.
#  MXMLDOC_EXECUTABLE - Path to "mxmldoc" executable
#  MXML_VERSION       - Version string extracted from output of "mxmldoc"
#  MXML_FOUND         - True if mxml found.

#=============================================================================
# Copyright 2001-2009 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
#=============================================================================
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 
# * Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
# 
# * Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
# 
# * Neither the names of Kitware, Inc., the Insight Software Consortium,
#   nor the names of their contributors may be used to endorse or promote
#   products derived from this software without specific prior written
#   permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 
#=============================================================================

IF (MXML_INCLUDE_DIR)
  # Already in cache, be silent
  SET(MXML_FIND_QUIETLY TRUE)
ENDIF (MXML_INCLUDE_DIR)

FIND_PATH(MXML_INCLUDE_DIR mxml.h)

SET(MXML_NAMES mxml)
FIND_LIBRARY(MXML_LIBRARY NAMES ${MXML_NAMES} )
FIND_PROGRAM(MXMLDOC_EXECUTABLE NAMES mxmldoc)

# Extract version from header if not defined otherwise
IF(NOT MXML_VERSION)

  IF(MXMLDOC_EXECUTABLE)
    EXECUTE_PROCESS(COMMAND ${MXMLDOC_EXECUTABLE} OUTPUT_VARIABLE MXML_HDR)
    
    STRING(REGEX MATCH "meta name=\"creator\".*$" CREATOR ${MXML_HDR})
    STRING(REGEX REPLACE "meta name=\"creator\" content=\"Mini-XML v([^\"]*)\".*" "\\1" VERSION ${CREATOR})
  ELSE(MXMLDOC_EXECUTABLE)
    SET(VERSION "0.0")
  ENDIF(MXMLDOC_EXECUTABLE)

  SET(MXML_VERSION ${VERSION} CACHE INTERNAL "MXML version number")
    
ENDIF(NOT MXML_VERSION)

MARK_AS_ADVANCED( MXMLDOC_EXECUTABLE MXML_LIBRARY MXML_INCLUDE_DIR )

# Per-recommendation
SET(MXML_INCLUDE_DIRS "${MXML_INCLUDE_DIR}")
SET(MXML_LIBRARIES    "${MXML_LIBRARY}")

# handle the QUIETLY and REQUIRED arguments and set MXML_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MXML DEFAULT_MSG MXML_LIBRARIES MXML_INCLUDE_DIRS)

