noinst_HEADERS = http_parser.h
noinst_LIBRARIES = libnetwork.a
AM_CFLAGS = $(CFLAGS) @DATADIR@ @SDL_CFLAGS@
INCLUDES = -I$(top_srcdir)/src/include -I$(top_srcdir)/src
libnetwork_a_SOURCES = http.c http_parser.c sdlnet.c null.c
