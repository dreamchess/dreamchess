cmake_minimum_required(VERSION 3.2)
project(DreamChess)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_DREAMCHESS "Build DreamChess GUI" ON)
option(BUILD_DREAMER "Build Dreamer chess engine" ON)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

if(UNIX)
    include(GNUInstallDirs)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CheckIncludeFiles)
check_include_files(getopt.h HAVE_GETOPT_H)
include(CheckFunctionExists)
check_function_exists(getopt_long HAVE_GETOPT_LONG)
check_function_exists(usleep HAVE_USLEEP)
try_compile(HAVE_C99_VARARGS_MACROS ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/test_c99_varargs.c)
try_compile(HAVE_GNUC_VARARGS_MACROS ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/test_gnuc_varargs.c)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

add_subdirectory(common/src)

if(BUILD_DREAMCHESS)
    add_subdirectory(dreamchess/src)
endif()

if(BUILD_DREAMER)
    add_subdirectory(dreamer/src)
endif()
