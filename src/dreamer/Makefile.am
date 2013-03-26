noinst_HEADERS = board.h dreamer.h eval.h history.h move.h repetition.h \
	commands.h hashing.h e_comm.h move_data.h search.h transposition.h \
	timer.h pgn_scanner.h

AM_CPPFLAGS = -I$(top_builddir)/src/include -I$(top_srcdir)/src/include
AM_CFLAGS = $(CFLAGS) @DREAMER_SDL_CFLAGS@
AM_LFLAGS = -P`basename $@ scanner.c` -olex.yy.c
AM_YFLAGS = -d -p `basename $@ parser.c`

bin_PROGRAMS = @DREAMER@
EXTRA_PROGRAMS = dreamer
dreamer_SOURCES = main.c
dreamer_LDADD = libdreamer.a ../libs/libsan.a @DREAMER_LIBS@

noinst_LIBRARIES = libdreamer.a
libdreamer_a_SOURCES = dreamer.c e_comm_unix.c commands.c board.c \
	gen_chess_moves.c hashing.c move.c search.c repetition.c \
	transposition.c eval.c history.c e_comm_sdlthd.c e_comm_win32.c \
	e_comm.c pgn_parser.y pgn_scanner.l makebook.c timer.c