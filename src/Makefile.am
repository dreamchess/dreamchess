SUBDIRS = libs dreamer include ui_sdlgl gamegui_dialogs audio
noinst_HEADERS = board.h comm.h debug.h dir.h dreamchess.h fen.h history.h \
	ui.h credits.h
EXTRA_DIST = win32.rc win32.ico beos.rdef
bin_PROGRAMS = dreamchess
INCLUDES = -I$(top_srcdir)/src/include
AM_CFLAGS = $(CFLAGS) @DATADIR@ @SDL_CFLAGS@
BUILT_SOURCES = pgn_parser.h
AM_YFLAGS = -d

dreamchess_SOURCES = board.c history.c ui.c comm_unix.c comm_sdlthd.c \
	debug.c dreamchess.c fen.c main.c main_sdlthd.c dir.c comm.c \
	comm_win32.c credits.c theme.c pgn_parser.y pgn_scanner.l

dreamchess_LDADD = @WIN32_RC@ libs/libsan.a ui_sdlgl/libui_sdlgl.a \
	libs/gamegui/libgamegui.a gamegui_dialogs/libgamegui_dialogs.a \
	audio/libaudio.a @DREAMCHESS_LIBS@ @SDL_LIBS@

dreamchess_DEPENDENCIES = @WIN32_RC@ libs/libsan.a ui_sdlgl/libui_sdlgl.a \
	libs/gamegui/libgamegui.a gamegui_dialogs/libgamegui_dialogs.a \
	audio/libaudio.a @DREAMCHESS_LIBS@

win32_rc.$(OBJEXT):
	@WINDRES@ win32.rc win32_rc.$(OBJEXT)

beos.rsrc:
	@RC@ beos.rdef

if BEOS_RC
dreamchess$(EXEEXT): $(dreamchess_OBJECTS) $(dreamchess_DEPENDENCIES) beos.rsrc
	@rm -f dreamchess$(EXEEXT)
	$(LINK) $(dreamchess_LDFLAGS) $(dreamchess_OBJECTS) $(dreamchess_LDADD) $(LIBS)
	@XRES@ -o $@ beos.rsrc
endif
