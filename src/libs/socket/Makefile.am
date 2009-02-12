EXTRA_LIBRARIES = libsocket.a
noinst_LIBRARIES = @SOCKET_LIB@
AM_CFLAGS = $(CFLAGS) @LUA_CFLAGS@
libsocket_a_SOURCES = luasocket.c timeout.c buffer.c io.c auxiliar.c \
	options.c inet.c tcp.c udp.c except.c select.c
libsocket_a_LIBADD = @SOCKET_SYSTEM@
libsocket_a_DEPENDENCIES = @SOCKET_SYSTEM@
