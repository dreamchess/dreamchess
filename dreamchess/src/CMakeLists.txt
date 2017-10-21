find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(PUGIXML REQUIRED)

BISON_TARGET(PgnParser pgn_parser.y ${CMAKE_CURRENT_BINARY_DIR}/pgn_parser.cpp)
FLEX_TARGET(PgnScanner pgn_scanner.l ${CMAKE_CURRENT_BINARY_DIR}/pgn_scanner.cpp)
ADD_FLEX_BISON_DEPENDENCY(PgnScanner PgnParser)

if(MINGW)
    set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>") 
endif(MINGW)

find_path(GLM_INCLUDE_DIR glm/glm.hpp)
if (GLM_INCLUDE_DIR)
    include_directories(${GLM_INCLUDE_DIR})
else()
    message(FATAL_ERROR "Unable to find GLM!")
endif()

add_executable(dreamchess
    ChessBoard.cpp
    ChessBoard.h
    DreamChess.cpp
    DreamChess.h
    MoveList.cpp
    MoveList.h
    GameConfig.h
    GameConfig.cpp
    History.cpp
    History.h
    System.cpp
    System.h
    Texture.h
    Texture.cpp
    Model.h
    Model.cpp
    #Scene.h
    #Scene.cpp
    TitleScene.cpp
    TitleScene.h
    ResourcePool.h
    ResourcePool.cpp
    Image.h
    Image.cpp
    Object.h
    Object.cpp
    Mesh.h
    Mesh.cpp
    Game.h
    Game.cpp
    Group.h
    Group.cpp
    Main.cpp
    #${BISON_PgnParser_OUTPUTS}
    #${FLEX_PgnScanner_OUTPUTS}
)

if(WIN32)
    target_sources(dreamchess PRIVATE #[[EngineCommWin.cpp]] win32.rc)
    set_source_files_properties(win32.rc PROPERTIES OBJECT_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/win32.ico;${CMAKE_CURRENT_SOURCE_DIR}/dpi.manifest")
else()
    target_sources(dreamchess PRIVATE #[[EngineCommUnix.cpp]])
endif()

if(MSVC)
    target_sources(dreamchess PRIVATE msvc/getopt.c msvc/getopt.h)
endif()
#[[
target_sources(dreamchess
    PRIVATE
    audio/playlist.cpp
    audio/playlist.h
    audio/sdlmixer.cpp
    audio/audio.h
)

source_group("Source Files\\audio" REGULAR_EXPRESSION "/audio/.*.cpp$")
source_group("Header Files\\audio" REGULAR_EXPRESSION /audio/.*.h$)
]]
#[[
target_sources(dreamchess
    PRIVATE
    dialogs/configuration.cpp
    dialogs/ingame_dialog.cpp
    dialogs/message_dialog.cpp
    dialogs/quit_dialog.cpp
    dialogs/resolution.cpp
    dialogs/styles.cpp
    dialogs/system_dialog.cpp
    dialogs/time.cpp
    dialogs/dialogs.h
)

source_group("Source Files\\dialogs" REGULAR_EXPRESSION /dialogs/.*.cpp$)
source_group("Header Files\\dialogs" REGULAR_EXPRESSION /dialogs/.*.h$)


target_sources(dreamchess
    PRIVATE
    gamegui/action.cpp
    gamegui/align.cpp
    gamegui/bin.cpp
    gamegui/box.cpp
    gamegui/clipping.cpp
    gamegui/container.cpp
    gamegui/dialog.cpp
    gamegui/edit.cpp
    gamegui/entry.cpp
    gamegui/frame.cpp
    gamegui/hbox.cpp
    gamegui/image.cpp
    gamegui/label.cpp
    gamegui/option.cpp
    gamegui/scrollbarv.cpp
    gamegui/select.cpp
    gamegui/seperatorh.cpp
    gamegui/seperatorv.cpp
    gamegui/signal.cpp
    gamegui/system.cpp
    gamegui/vbox.cpp
    gamegui/viewport.cpp
    gamegui/widget.cpp
    gamegui/gamegui.h
    gamegui/action.h
    gamegui/align.h
    gamegui/bin.h
    gamegui/box.h
    gamegui/clipping.h
    gamegui/container.h
    gamegui/dialog.h
    gamegui/edit.h
    gamegui/entry.h
    gamegui/frame.h
    gamegui/hbox.h
    gamegui/image.h
    gamegui/label.h
    gamegui/option.h
    gamegui/queue.h
    gamegui/scrollbarv.h
    gamegui/select.h
    gamegui/seperatorh.h
    gamegui/seperatorv.h
    gamegui/signal.h
    gamegui/system.h
    gamegui/vbox.h
    gamegui/viewport.h
    gamegui/widget.h
)

source_group("Source Files\\gamegui" REGULAR_EXPRESSION /gamegui/.*.cpp$)
source_group("Header Files\\gamegui" REGULAR_EXPRESSION /gamegui/.*.h$)
]]

#[[
target_sources(dreamchess
    PRIVATE
    gui/colours.cpp
    gui/credits.cpp
    gui/draw_scene.cpp
    gui/draw_shape.cpp
    gui/gamegui_driver.cpp
    gui/get_move.cpp
    gui/ingame_ui.cpp
    gui/load_data.cpp
    gui/promote_dialog.cpp
    gui/saveload_dialog.cpp
    gui/system.cpp
    gui/text.cpp
    gui/texture.cpp
    gui/theme.cpp
    gui/title_dialog.cpp
    gui/transitions.cpp
    gui/ui_sdlgl_3d.cpp
    gui/ui_sdlgl_3d.h
    gui/ui_sdlgl.cpp
    gui/ui_sdlgl.h
    gui/victory_dialog.cpp
    gui/vkeyboard.cpp
    gui/xmlsavefile.cpp
)

source_group("Source Files\\gui" REGULAR_EXPRESSION /gui/.*.cpp$)
source_group("Header Files\\gui" REGULAR_EXPRESSION /gui/.*.h$)
]]

if(UNIX)
    target_compile_definitions(dreamchess PRIVATE -DDATADIR="${CMAKE_INSTALL_FULL_DATADIR}/dreamchess")
endif()

target_include_directories(dreamchess
    PRIVATE
    ${SDL2_INCLUDE_DIR}
    ${SDL2_IMAGE_INCLUDE_DIR}
    ${SDL2_MIXER_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIR}
    ${PUGIXML_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
#[[    ${CMAKE_CURRENT_SOURCE_DIR}/gamegui]]
    ${CMAKE_BINARY_DIR}
)

target_link_libraries(dreamchess
    common
    ${GLEW_LIBRARY}
    ${SDL2_IMAGE_LIBRARY}
    ${SDL2_MIXER_LIBRARY}
    ${SDL2_LIBRARY}
    ${OPENGL_LIBRARY}
    ${PUGIXML_LIBRARY}
)

if(WIN32)
    target_link_libraries(dreamchess shlwapi)
endif()

if(MSVC)
    set_target_properties(dreamchess PROPERTIES LINK_FLAGS_DEBUG "/MANIFEST:NO")
    set_target_properties(dreamchess PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /MANIFEST:NO")
    set_target_properties(dreamchess PROPERTIES LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:WINDOWS /MANIFEST:NO")
    set_target_properties(dreamchess PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:WINDOWS /MANIFEST:NO")
elseif(MINGW)
    set_target_properties(dreamchess PROPERTIES LINK_FLAGS_DEBUG "-mconsole")
endif()

if(WIN32)
    install(TARGETS dreamchess DESTINATION .)
    install(DIRECTORY ../data DESTINATION .)
elseif(UNIX)
    install(TARGETS dreamchess DESTINATION ${CMAKE_INSTALL_BINDIR})
    install(FILES ../man/dreamchess.6 DESTINATION ${CMAKE_INSTALL_MANDIR}/man6)
    install(FILES ../desktop/dreamchess.desktop DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)
    install(FILES ../desktop/48/dreamchess.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/apps)
    install(FILES ../desktop/256/dreamchess.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/256x256/apps)
    install(DIRECTORY ../data/ DESTINATION ${CMAKE_INSTALL_DATADIR}/dreamchess)
endif()
