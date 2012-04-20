/*  DreamChess
**
**  DreamChess is the legal property of its developers, whose names are too
**  numerous to list here. Please refer to the COPYRIGHT file distributed
**  with this source distribution.
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "config.h"
#include "network.h"
#include <stdlib.h>

#ifndef WITH_NET_SDL

int net_init() {
	return 1;
}

net_socket *net_connect(const char *host, unsigned int port) {
	return NULL;
}

net_server_socket *net_listen(unsigned int port) {
	return NULL;
}

net_socket *net_accept(net_server_socket *socket) {
	return NULL;
}

int net_read(net_socket *s, char *buf, unsigned int len) {
	return -1;
}

int net_write(net_socket *s, char *buf, unsigned int len) {
	return 0;
}

void net_close(net_socket *s) {
}

void net_close_server(net_server_socket *s) {
}

#endif
