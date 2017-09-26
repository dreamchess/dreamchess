# Copyright (c) 2014 Andrew Kelley
# This file is MIT licensed.
# See http://opensource.org/licenses/MIT

# TINYXML2_FOUND
# TINYXML2_INCLUDE_DIR
# TINYXML2_LIBRARIES

find_path(TINYXML2_INCLUDE_DIR NAMES tinyxml2.h)
find_library(TINYXML2_LIBRARY_DEBUG NAMES tinyxml2)
find_library(TINYXML2_LIBRARY_RELEASE NAMES tinyxml2)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(TINYXML2 DEFAULT_MSG TINYXML2_LIBRARY_DEBUG TINYXML2_LIBRARY_RELEASE TINYXML2_INCLUDE_DIR)

mark_as_advanced(TINYXML2_INCLUDE_DIR TINYXML2_LIBRARY_DEBUG TINYXML_LIBRARY_RELEASE)
