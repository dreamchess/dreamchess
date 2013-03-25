AM_CPPFLAGS = -I$(top_srcdir)/src/include/minizip
AM_CFLAGS = $(CFLAGS) -DNOCRYPT -DNOUNCRYPT @MINIZIP_CFLAGS@
noinst_LIBRARIES = @MINIZIP_LIB@
EXTRA_LIBRARIES = libminizip.a
libminizip_a_SOURCES = zip.c unzip.c ioapi.c
