include(GetGitRevisionDescription)
git_describe(GIT_DESC)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/git_rev.c.in" "${CMAKE_CURRENT_BINARY_DIR}/git_rev.c" @ONLY)

BISON_TARGET(SanParser san_parse.y ${CMAKE_CURRENT_BINARY_DIR}/san_parse.c COMPILE_FLAGS "-p san")

add_library(common STATIC
    ${BISON_SanParser_OUTPUTS}
    ${CMAKE_CURRENT_BINARY_DIR}/git_rev.c
    msgbuf.c
    msgbuf.h
)

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src/include)

target_sources(common
    PUBLIC
    ${CMAKE_SOURCE_DIR}/src/include/san.h
    INTERFACE
    ${INCLUDE_DIR}/git_rev.h
)

if(WIN32)
    target_sources(common
        PRIVATE
        pipe_win32.c
        PUBLIC
        ${INCLUDE_DIR}/pipe_win32.h
    )
else()
    target_sources(common
        PRIVATE
        pipe_unix.c
        PUBLIC
        ${INCLUDE_DIR}/pipe_unix.h
    )
endif()

target_include_directories(common PUBLIC ${INCLUDE_DIR})
