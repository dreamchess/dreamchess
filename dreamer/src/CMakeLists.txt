BISON_TARGET(PgnParserBook pgn_parser.y ${CMAKE_CURRENT_BINARY_DIR}/pgn_parser.cpp COMPILE_FLAGS "-p pgn_")
FLEX_TARGET(PgnScannerBook pgn_scanner.l ${CMAKE_CURRENT_BINARY_DIR}/pgn_scanner.cpp COMPILE_FLAGS "-P pgn_")
ADD_FLEX_BISON_DEPENDENCY(PgnScannerBook PgnParserBook)

add_executable(dreamer
    board.cpp
    board.h
    commands.cpp
    commands.h
    dreamer.cpp
    dreamer.h
    e_comm.cpp
    e_comm.h
    eval.cpp
    eval.h
    hashing.cpp
    hashing.h
    main.cpp
    makebook.cpp
    makebook.h
    move.cpp
    move.h
    repetition.cpp
    repetition.h
    search.cpp
    search.h
    timer.cpp
    timer.h
    ttable.cpp
    ttable.h
    ${BISON_PgnParserBook_OUTPUTS}
    ${FLEX_PgnScannerBook_OUTPUTS}
)

if(WIN32)
    target_sources(dreamer PRIVATE e_comm_win32.cpp)
else()
    target_sources(dreamer PRIVATE e_comm_unix.cpp)
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
