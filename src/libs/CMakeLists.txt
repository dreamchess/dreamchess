include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GIT_SHA1)
string(SUBSTRING ${GIT_SHA1} 0 7 GIT_SHA1)
git_get_exact_tag(GIT_TAG)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/git_rev.c.in" "${CMAKE_CURRENT_BINARY_DIR}/git_rev.c" @ONLY)

BISON_TARGET(SanParser san_parse.y ${CMAKE_CURRENT_BINARY_DIR}/san_parse.c COMPILE_FLAGS "-p san")

set(COMMON_SRCS
    ${BISON_SanParser_OUTPUTS}
    msgbuf.c
    ${CMAKE_CURRENT_BINARY_DIR}/git_rev.c
)

if(WIN32)
    list(APPEND COMMON_SRCS pipe_win32.c)
else()
    list(APPEND COMMON_SRCS pipe_unix.c)
endif()

add_library(common STATIC ${COMMON_SRCS})

if(ENGINE_ONLY)
    return()
endif()

add_subdirectory(gamegui)
