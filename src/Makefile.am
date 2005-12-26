SUBDIRS = libs dreamer include
noinst_HEADERS = board.h comm.h dir.h dreamchess.h history.h ui.h \
	credits.h ui_sdlgl_3d.h
bin_PROGRAMS = dreamchess
INCLUDES = -I$(top_srcdir)/src/include
AM_CFLAGS = $(CFLAGS) @DATADIR@ @SDL_CFLAGS@
BUILT_SOURCES = pgn_parser.h
AM_YFLAGS = -d

dreamchess_SOURCES = ui_sdlgl.c ui_sdl.c board.c history.c ui.c \
	comm_unix.c comm_sdlthd.c dreamchess.c main.c main_sdlthd.c \
	dir.c comm.c comm_win32.c credits.c ui_sdlgl_3d.c pgn_parser.y \
	pgn_scanner.l

dreamchess_LDADD = @SDL_LIBS@ @DREAMCHESS_LIBS@ @WIN32_RC@ libs/libsan.a \
	libs/gamegui/libgamegui.a libs/mxml/libmxml.a

dreamchess_DEPENDENCIES = @WIN32_RC@ libs/libsan.a libs/gamegui/libgamegui.a
	libs/mxml/libmxml.a

win32_rc.$(OBJEXT):
	@WINDRES@ win32.rc win32_rc.$(OBJEXT)
