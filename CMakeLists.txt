cmake_minimum_required(VERSION 3.7)
project(DreamChess)

option(BUILD_DREAMCHESS "Build DreamChess GUI" ON)
option(BUILD_DREAMER "Build Dreamer chess engine" ON)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

include(GNUInstallDirs)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CheckIncludeFiles)
check_include_files(getopt.h HAVE_GETOPT_H)
include(CheckFunctionExists)
check_function_exists(getopt_long HAVE_GETOPT_LONG)
check_function_exists(usleep HAVE_USLEEP)
try_compile(HAVE_C99_VARARGS_MACROS ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/test_c99_varargs.c)
try_compile(HAVE_GNUC_VARARGS_MACROS ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/test_gnuc_varargs.c)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

add_subdirectory(src/common)

if(BUILD_DREAMCHESS)
    add_subdirectory(src/dreamchess)
endif()

if(BUILD_DREAMER)
    add_subdirectory(src/dreamer)
endif()
