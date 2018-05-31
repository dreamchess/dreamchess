cmake_minimum_required(VERSION 3.2)
project(DreamChess VERSION 0.3.0)

set(DREAMCHESS_RELEASE FALSE)
set(COPYRIGHT_YEAR 2018)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/wd4996)
endif()

option(BUILD_DREAMCHESS "Build DreamChess GUI" ON)
option(BUILD_DREAMER "Build Dreamer chess engine" ON)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

if(UNIX AND NOT APPLE)
    include(GNUInstallDirs)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(GetGitRevisionDescription)
if(NOT DREAMCHESS_RELEASE)
    get_git_head_revision(GIT_REFSPEC GIT_SHA1)
    git_local_changes(GIT_CLEAN)
    if(GIT_SHA1)
        string(SUBSTRING "${GIT_SHA1}" 0 7 GIT_SHA1_SHORT)
        if(${GIT_CLEAN} STREQUAL "CLEAN")
            set(GIT_CLEAN "")
        else()
            set(GIT_CLEAN "*")
        endif()
        set(GIT_REV "-GIT-${GIT_SHA1_SHORT}${GIT_CLEAN}")
    else()
        set(GIT_REV "-GIT")
    endif()
endif()

include(CMakeCPack)
include(CheckIncludeFiles)
check_include_files(getopt.h HAVE_GETOPT_H)
include(CheckFunctionExists)
check_function_exists(getopt_long HAVE_GETOPT_LONG)
check_function_exists(usleep HAVE_USLEEP)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

add_subdirectory(common/src)

if(BUILD_DREAMCHESS)
    add_subdirectory(dreamchess/src)
endif()

if(BUILD_DREAMER)
    add_subdirectory(dreamer/src)
endif()

set(DOC_FILES
    LICENSE.txt
    AUTHORS.txt
)

if(APPLE)
    install(FILES ${DOC_FILES} DESTINATION .)
elseif(UNIX)
    install(FILES ${DOC_FILES} DESTINATION ${CMAKE_INSTALL_DOCDIR})
else()
    install(FILES ${DOC_FILES} DESTINATION doc)
endif()

add_subdirectory(dist)
