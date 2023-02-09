find_package(SDL2 CONFIG REQUIRED)

set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
unset(CMAKE_FIND_PACKAGE_PREFER_CONFIG)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(Freetype REQUIRED)
find_package(EXPAT REQUIRED)

include(CheckLibraryExists)
check_library_exists(m cos "" HAVE_LIBM)
if(HAVE_LIBM)
    set(MATH_LIBRARY "m")
endif()

BISON_TARGET(PgnParser pgn_parser.y ${CMAKE_CURRENT_BINARY_DIR}/pgn_parser.c)
FLEX_TARGET(PgnScanner pgn_scanner.l ${CMAKE_CURRENT_BINARY_DIR}/pgn_scanner.c)
ADD_FLEX_BISON_DEPENDENCY(PgnScanner PgnParser)

if(MINGW)
    set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>") 
endif(MINGW)

add_executable(dreamchess MACOSX_BUNDLE
    board.c
    board.h
    comm.c
    comm.h
    credits.c
    credits.h
    debug.c
    debug.h
    dir.c
    dir.h
    dreamchess.c
    dreamchess.h
    fen.c
    fen.h
    history.c
    history.h
    options.c
    system_config.c
    theme.c
    ui.h
    xml.c
    xml.h
    ${BISON_PgnParser_OUTPUTS}
    ${FLEX_PgnScanner_OUTPUTS}
    include/engine.h
    include/options.h
    include/system_config.h
    include/theme.h
)

add_dependencies(dreamchess update-gmo)

set(COPYRIGHT_STRING "Copyright © 2003-${COPYRIGHT_YEAR} DreamChess developers")

set_target_properties(dreamchess PROPERTIES
    MACOSX_BUNDLE_NAME ${PROJECT_NAME}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_COPYRIGHT ${COPYRIGHT_STRING}
    MACOSX_BUNDLE_GUI_IDENTIFIER org.dreamchess.DreamChess
    MACOSX_BUNDLE_ICON_FILE DreamChess.icns
    MACOSX_BUNDLE_INFO_STRING ${COPYRIGHT_STRING}
    MACOSX_BUNDLE_LONG_VERSION_STRING "Version ${PROJECT_VERSION}"
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
)

if(APPLE OR WIN32)
    set_target_properties(dreamchess PROPERTIES OUTPUT_NAME DreamChess)
endif()

if(WIN32)
    target_sources(dreamchess PRIVATE comm_win32.c win32.rc)
    set_source_files_properties(win32.rc PROPERTIES OBJECT_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/win32.ico;${CMAKE_CURRENT_SOURCE_DIR}/dpi.manifest")
else()
    target_sources(dreamchess PRIVATE comm_unix.c)
endif()

if(MSVC)
    target_sources(dreamchess PRIVATE msvc/getopt.c msvc/getopt.h)
endif()

target_sources(dreamchess PRIVATE
    audio/playlist.c
    audio/playlist.h
    audio/sdlmixer.c
    include/audio.h
)

source_group("Source Files\\audio" REGULAR_EXPRESSION /audio/.*.c$)
source_group("Header Files\\audio" REGULAR_EXPRESSION /audio/.*.h$)

target_sources(dreamchess PRIVATE
    dialogs/configuration.c
    dialogs/ingame_dialog.c
    dialogs/message_dialog.c
    dialogs/quit_dialog.c
    dialogs/resolution.c
    dialogs/styles.c
    dialogs/system_dialog.c
    dialogs/time.c
    include/dialogs.h
)

source_group("Source Files\\dialogs" REGULAR_EXPRESSION /dialogs/.*.c$)
source_group("Header Files\\dialogs" REGULAR_EXPRESSION /dialogs/.*.h$)

target_sources(dreamchess PRIVATE
    gamegui/action.c
    gamegui/align.c
    gamegui/bin.c
    gamegui/box.c
    gamegui/clipping.c
    gamegui/container.c
    gamegui/dialog.c
    gamegui/edit.c
    gamegui/entry.c
    gamegui/frame.c
    gamegui/hbox.c
    gamegui/image.c
    gamegui/label.c
    gamegui/option.c
    gamegui/scrollbarv.c
    gamegui/select.c
    gamegui/seperatorh.c
    gamegui/seperatorv.c
    gamegui/signal.c
    gamegui/system.c
    gamegui/vbox.c
    gamegui/viewport.c
    gamegui/widget.c
    include/gamegui.h
    include/gamegui/action.h
    include/gamegui/align.h
    include/gamegui/bin.h
    include/gamegui/box.h
    include/gamegui/clipping.h
    include/gamegui/container.h
    include/gamegui/dialog.h
    include/gamegui/edit.h
    include/gamegui/entry.h
    include/gamegui/frame.h
    include/gamegui/hbox.h
    include/gamegui/image.h
    include/gamegui/label.h
    include/gamegui/option.h
    include/gamegui/queue.h
    include/gamegui/scrollbarv.h
    include/gamegui/select.h
    include/gamegui/seperatorh.h
    include/gamegui/seperatorv.h
    include/gamegui/signal.h
    include/gamegui/system.h
    include/gamegui/vbox.h
    include/gamegui/viewport.h
    include/gamegui/widget.h
)

source_group("Source Files\\gamegui" REGULAR_EXPRESSION /gamegui/.*.c$)
source_group("Header Files\\gamegui" REGULAR_EXPRESSION /include/gamegui/.*.h$)

target_sources(dreamchess PRIVATE
    gui/colours.c
    gui/credits.c
    gui/draw_scene.c
    gui/draw_shape.c
    gui/gamegui_driver.c
    gui/get_move.c
    gui/ingame_ui.c
    gui/load_data.c
    gui/opengl.c
    gui/opengl.h
    gui/promote_dialog.c
    gui/saveload_dialog.c
    gui/system.c
    gui/texture.c
    gui/theme.c
    gui/title_dialog.c
    gui/transitions.c
    gui/ui_sdlgl_3d.c
    gui/ui_sdlgl_3d.h
    gui/ui_sdlgl.c
    gui/ui_sdlgl.h
    gui/unicode.c
    gui/unicode.h
    gui/victory_dialog.c
    gui/xmlsavefile.c
    gui/freetype-gl/distance-field.c
    gui/freetype-gl/distance-field.h
    gui/freetype-gl/edtaa3func.c
    gui/freetype-gl/edtaa3func.h
    gui/freetype-gl/freetype-gl-errdef.h
    gui/freetype-gl/ftgl-utils.c
    gui/freetype-gl/ftgl-utils.h
    gui/freetype-gl/platform.c
    gui/freetype-gl/platform.h
    gui/freetype-gl/texture-atlas.c
    gui/freetype-gl/texture-atlas.h
    gui/freetype-gl/texture-font.c
    gui/freetype-gl/texture-font.h
    gui/freetype-gl/utf8-utils.c
    gui/freetype-gl/utf8-utils.h
    gui/freetype-gl/vector.c
    gui/freetype-gl/vector.h
)

source_group("Source Files\\gui" REGULAR_EXPRESSION /gui/.*.c$)
source_group("Header Files\\gui" REGULAR_EXPRESSION /gui/.*.h$)

if(UNIX)
    target_compile_definitions(dreamchess PRIVATE -DDATADIR="${CMAKE_INSTALL_FULL_DATADIR}/dreamchess" -DLOCALEDIR="${CMAKE_INSTALL_FULL_LOCALEDIR}")
endif()

target_include_directories(dreamchess PRIVATE
    ${SDL2_IMAGE_INCLUDE_DIR}
    ${SDL2_MIXER_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIR}
    ${FREETYPE_INCLUDE_DIRS}
    ${EXPAT_INCLUDE_DIR}
    ${Intl_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}
)

target_link_libraries(dreamchess PRIVATE
    common
    GLEW::GLEW
    Freetype::Freetype
    $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,$<TARGET_NAME_IF_EXISTS:SDL2_image::SDL2_image-static>>
    ${SDL2_IMAGE_LIBRARY}
    $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,$<TARGET_NAME_IF_EXISTS:SDL2_mixer::SDL2_mixer-static>>
    ${SDL2_MIXER_LIBRARY}
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    ${OPENGL_LIBRARY}
    EXPAT::EXPAT
    ${Intl_LIBRARIES}
    ${MATH_LIBRARY}
)

if(WIN32)
    target_link_libraries(dreamchess PRIVATE shlwapi)
elseif(APPLE)
    target_link_libraries(dreamchess PRIVATE "-framework CoreFoundation")
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
    set_property(INSTALL $<TARGET_FILE_NAME:dreamchess>
        PROPERTY CPACK_START_MENU_SHORTCUTS "DreamChess"
    )

    install(DIRECTORY ../data DESTINATION .)

    if(MSVC)
        install(CODE "
            file(GLOB DLLS \"$<TARGET_FILE_DIR:dreamchess>/*.dll\")
            file(INSTALL \${DLLS} DESTINATION \${CMAKE_INSTALL_PREFIX})
        ")
    endif()
elseif(APPLE)
    install(TARGETS dreamchess DESTINATION .)
    install(DIRECTORY ../data/ DESTINATION DreamChess.app/Contents/Resources)
elseif(UNIX)
    install(TARGETS dreamchess DESTINATION ${CMAKE_INSTALL_BINDIR})
    install(FILES ../man/dreamchess.6 DESTINATION ${CMAKE_INSTALL_MANDIR}/man6)
    install(FILES ../desktop/dreamchess.desktop DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)
    install(FILES ../desktop/48/dreamchess.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/apps)
    install(FILES ../desktop/256/dreamchess.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/256x256/apps)
    install(DIRECTORY ../data/ DESTINATION ${CMAKE_INSTALL_DATADIR}/dreamchess)
endif()
