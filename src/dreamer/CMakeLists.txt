include_directories(${CMAKE_CURRENT_SOURCE_DIR})
BISON_TARGET(PgnParserBook pgn_parser.y ${CMAKE_CURRENT_BINARY_DIR}/pgn_parser.c COMPILE_FLAGS "-p pgn_")
FLEX_TARGET(PgnScannerBook pgn_scanner.l ${CMAKE_CURRENT_BINARY_DIR}/pgn_scanner.c COMPILE_FLAGS "-P pgn_")
ADD_FLEX_BISON_DEPENDENCY(PgnScannerBook PgnParserBook)

set(DREAMER_SRCS
    board.c
    commands.c
    dreamer.c
    e_comm.c
    eval.c
    gen_chess_moves.c
    hashing.c
    history.c
    main.c
    makebook.c
    move.c
    repetition.c
    search.c
    timer.c
    transposition.c
    ${BISON_PgnParserBook_OUTPUTS}
    ${FLEX_PgnScannerBook_OUTPUTS}
)

if(WIN32)
    list(APPEND DREAMER_SRCS e_comm_win32.c)
else()
    list(APPEND DREAMER_SRCS e_comm_unix.c)
endif()

add_executable(dreamer ${DREAMER_SRCS})

target_link_libraries(dreamer
    common
)
