cmake_minimum_required(VERSION 3.7)
project(DreamChess)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CheckIncludeFiles)
check_include_files(getopt.h HAVE_GETOPT_H)
include(CheckFunctionExists)
check_function_exists(getopt_long HAVE_GETOPT_LONG)
check_function_exists(usleep HAVE_USLEEP)
try_compile(HAVE_C99_VARARGS_MACROS ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/test_c99_varargs.c)
try_compile(HAVE_GNUC_VARARGS_MACROS ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/test_gnuc_varargs.c)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_subdirectory(src)
