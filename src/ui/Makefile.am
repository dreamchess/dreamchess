noinst_LIBRARIES = libui.a
AM_CXXFLAGS = $(CXXFLAGS) @DATADIR@ @SDL_CFLAGS@
INCLUDES = -I$(top_srcdir)/src/include -I$(top_srcdir)/src -I$(top_srcdir)/src/ui/include
libui_a_SOURCES = screen.cpp entity.cpp scene.cpp input.cpp mesh.cpp \
    model.cpp texture.cpp camera.cpp light.cpp box.cpp chess_board.cpp resource.cpp \
    game/game.cpp title/title.cpp title/title_camera.cpp script.cpp
