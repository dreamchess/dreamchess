noinst_HEADERS = board.h dreamer.h eval.h history.h move.h repetition.h \
	commands.h hashing.h e_comm.h move_data.h search.h transposition.h

INCLUDES = -I$(top_srcdir)/src/include
AM_CFLAGS = $(CFLAGS) @DREAMER_SDL_CFLAGS@

bin_PROGRAMS = @DREAMER@
EXTRA_PROGRAMS = dreamer
dreamer_SOURCES = main.c
dreamer_LDADD = libdreamer.a @DREAMER_LIBS@

noinst_LIBRARIES = libdreamer.a
nodist_libdreamer_a_SOURCES = move_data.c
libdreamer_a_SOURCES = dreamer.c e_comm_unix.c commands.c board.c \
	hashing.c move.c search.c repetition.c transposition.c \
	eval.c history.c e_comm_sdlthd.c e_comm_win32.c \
	e_comm.c

noinst_PROGRAMS = genchessmoves
genchessmoves_SOURCES = gen_chess_moves.c
gen_chess_moves.o: gen_chess_moves.c
	@CC_FOR_BUILD@ -c $<
genchessmoves$(EXEEXT): $(genchessmoves_OBJECTS)
	@rm -f genchessmoves$(EXEEXT)
	@CC_FOR_BUILD@ -o $@ $(genchessmoves_OBJECTS)
MOSTLYCLEANFILES = move_data.c

move_data.c: genchessmoves$(EXEEXT)
	./genchessmoves$(EXEEXT)
