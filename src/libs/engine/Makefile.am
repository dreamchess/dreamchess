noinst_LIBRARIES = libengine.a
AM_CXXFLAGS = $(CXXFLAGS) @DATADIR@ @SDL_CFLAGS@ @LUA_CFLAGS@
INCLUDES = -I$(top_srcdir)/src/include -I$(top_srcdir)/src -I$(top_srcdir)/src/ui/include
libengine_a_SOURCES = xml.cpp input.cpp screen.cpp test.cpp engine_wrap.cpp

engine_wrap.cpp: engine.i xml.h screen.h input.h
	$(SWIG) -lua -o $@ $<

