include_directories(include ${CMAKE_CURRENT_SOURCE_DIR})

option(ENGINE_ONLY "Only build the Dreamer chess engine")

find_package(BISON)
find_package(FLEX)

add_subdirectory(libs)
add_subdirectory(dreamer)

if(ENGINE_ONLY)
    return()
endif()

find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
find_package(SDL_mixer REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(MXML REQUIRED)

include_directories(${SDL_INCLUDE_DIR})

BISON_TARGET(PgnParser pgn_parser.y ${CMAKE_CURRENT_BINARY_DIR}/pgn_parser.c)
FLEX_TARGET(PgnScanner pgn_scanner.l ${CMAKE_CURRENT_BINARY_DIR}/pgn_scanner.c)
ADD_FLEX_BISON_DEPENDENCY(PgnScanner PgnParser)

set(DREAMCHESS_SRCS
    audio/playlist.c
    audio/sdlmixer.c
    gamegui_dialogs/configuration.c
    gamegui_dialogs/ingame_dialog.c
    gamegui_dialogs/message_dialog.c
    gamegui_dialogs/quit_dialog.c
    gamegui_dialogs/resolution.c
    gamegui_dialogs/styles.c
    gamegui_dialogs/system_dialog.c
    gamegui_dialogs/time.c
    ui_sdlgl/ui_sdlgl.c
    ui_sdlgl/ui_sdlgl_3d.c
    ui_sdlgl/texture.c
    ui_sdlgl/draw_shape.c
    ui_sdlgl/text.c
    ui_sdlgl/colours.c
    ui_sdlgl/system.c
    ui_sdlgl/ingame_ui.c
    ui_sdlgl/draw_scene.c
    ui_sdlgl/get_move.c
    ui_sdlgl/theme.c
    ui_sdlgl/transitions.c
    ui_sdlgl/credits.c
    ui_sdlgl/load_data.c
    ui_sdlgl/xmlsavefile.c
    ui_sdlgl/gamegui_driver.c
    ui_sdlgl/saveload_dialog.c
    ui_sdlgl/title_dialog.c
    ui_sdlgl/victory_dialog.c
    ui_sdlgl/promote_dialog.c
    ui_sdlgl/vkeyboard.c
    board.c
    history.c
    ui.c
    debug.c
    dreamchess.c
    fen.c
    main.c
    dir.c
    comm.c
    credits.c
    theme.c
    options.c
    system_config.c
    ${BISON_PgnParser_OUTPUTS}
    ${FLEX_PgnScanner_OUTPUTS}
    ${CMAKE_BINARY_DIR}/src/libs/git_rev.c
)

if(WIN32)
    list(APPEND DREAMCHESS_SRCS comm_win32.c win32.rc)
else()
    list(APPEND DREAMCHESS_SRCS comm_unix.c)
endif()

if(MINGW)
    set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>") 
endif(MINGW)

set(DREAMCHESS_LIBS
    common
    gamegui
    ${GLEW_LIBRARY}
    ${SDL_IMAGE_LIBRARY}
    ${SDL_MIXER_LIBRARY}
    ${SDL_LIBRARY}
    ${OPENGL_LIBRARY}
    ${MXML_LIBRARY}
)

if(WIN32)
    list(APPEND DREAMCHESS_LIBS shlwapi)
endif()

add_executable(dreamchess ${DREAMCHESS_SRCS})
target_link_libraries(dreamchess ${DREAMCHESS_LIBS})
