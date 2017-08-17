BISON_TARGET(PgnParserBook pgn_parser.y ${CMAKE_CURRENT_BINARY_DIR}/pgn_parser.c COMPILE_FLAGS "-p pgn_")
FLEX_TARGET(PgnScannerBook pgn_scanner.l ${CMAKE_CURRENT_BINARY_DIR}/pgn_scanner.c COMPILE_FLAGS "-P pgn_")
ADD_FLEX_BISON_DEPENDENCY(PgnScannerBook PgnParserBook)

add_executable(dreamer
    board.c
    board.h
    commands.c
    commands.h
    dreamer.c
    dreamer.h
    e_comm.c
    e_comm.h
    eval.c
    eval.h
    gen_chess_moves.c
    hashing.c
    hashing.h
    history.c
    history.h
    main.c
    makebook.c
    makebook.h
    move_data.h
    move.c
    move.h
    repetition.c
    repetition.h
    search.c
    search.h
    timer.c
    timer.h
    transposition.c
    transposition.h
    ${BISON_PgnParserBook_OUTPUTS}
    ${FLEX_PgnScannerBook_OUTPUTS}
)

if(WIN32)
    target_sources(dreamer PRIVATE e_comm_win32.c)
else()
    target_sources(dreamer PRIVATE e_comm_unix.c)
endif()

target_link_libraries(dreamer common)

target_include_directories(dreamer
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
)

if(WIN32)
    install(TARGETS dreamer DESTINATION .)
elseif(UNIX)
    install(TARGETS dreamer DESTINATION ${CMAKE_INSTALL_BINDIR})
    install(FILES ../man/dreamer.6 DESTINATION ${CMAKE_INSTALL_MANDIR}/man6)
endif()
