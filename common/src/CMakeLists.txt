configure_file("${CMAKE_CURRENT_SOURCE_DIR}/git_rev.cpp.in" "${CMAKE_CURRENT_BINARY_DIR}/git_rev.cpp" @ONLY)

BISON_TARGET(SanParser san_parse.y ${CMAKE_CURRENT_BINARY_DIR}/san_parse.cpp COMPILE_FLAGS "-p san")

add_library(common STATIC
    ${BISON_SanParser_OUTPUTS}
    ${CMAKE_CURRENT_BINARY_DIR}/git_rev.cpp
    msgbuf.cpp
    msgbuf.h
)

target_sources(common
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include/san.h
    INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/include/git_rev.h
)

if(WIN32)
    target_sources(common
        PRIVATE
        pipe_win32.cpp
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include/pipe_win32.h
    )
else()
    target_sources(common
        PRIVATE
        pipe_unix.cpp
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include/pipe_unix.h
    )
endif()

target_include_directories(common PUBLIC include)
