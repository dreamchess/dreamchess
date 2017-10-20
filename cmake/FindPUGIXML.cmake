# Copyright (c) 2014 Andrew Kelley
# This file is MIT licensed.
# See http://opensource.org/licenses/MIT

# PUGIXML_FOUND
# PUGIXML_INCLUDE_DIR
# PUGIXML_LIBRARIES

find_path(PUGIXML_INCLUDE_DIR NAMES pugixml.hpp PATH_SUFFIXES pugixml-1.8)
find_library(PUGIXML_LIBRARY_DEBUG NAMES pugixml PATH_SUFFIXES pugixml-1.8)
find_library(PUGIXML_LIBRARY_RELEASE NAMES pugixml PATH_SUFFIXES pugixml-1.8)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PUGIXML DEFAULT_MSG PUGIXML_LIBRARY_DEBUG PUGIXML_LIBRARY_RELEASE PUGIXML_INCLUDE_DIR)

mark_as_advanced(PUGIXML_INCLUDE_DIR PUGIXML_LIBRARY_DEBUG PUGIXML_LIBRARY_RELEASE)
