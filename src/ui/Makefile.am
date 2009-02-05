noinst_LIBRARIES = libui.a
AM_CXXFLAGS = $(CXXFLAGS) @DATADIR@ @SDL_CFLAGS@ @LUA_CFLAGS@
INCLUDES = -I$(top_srcdir)/src/include -I$(top_srcdir)/src -I$(top_srcdir)/src/ui/include -I$(top_srcdir)/src/libs/engine
libui_a_SOURCES = entity.cpp scene.cpp mesh.cpp \
    model.cpp texture.cpp camera.cpp light.cpp box.cpp entity_group.cpp resource.cpp \
    game.cpp title.cpp title_camera.cpp script.cpp luatest.cpp \
    luatest_wrap.cpp

luatest_wrap.cpp: luatest.i luatest.h
	$(SWIG) -lua -o $@ $<
