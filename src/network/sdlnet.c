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

#ifdef WITH_NET_SDL

#include "debug.h"
#include "network.h"
#include <SDL_net.h>

struct net_socket {
	IPaddress ip;
	TCPsocket sd;
	SDLNet_SocketSet set;
};

int net_init() {
	return SDL_Init(0) || SDLNet_Init();
}

net_socket *net_open_socket(const char *host, unsigned int port) {
	struct net_socket *s = malloc(sizeof(struct net_socket));
	if (SDLNet_ResolveHost(&s->ip, host, port)) {
		DBG_WARN("failed to resolve %s", host);
		goto error2;
	}

	s->set = SDLNet_AllocSocketSet(1);
	if (!s->set) {
		DBG_WARN("failed to create socket set");
		goto error2;
	}

	if (!(s->sd = SDLNet_TCP_Open(&s->ip))) {
		DBG_WARN("failed to open TCP socket to %s", host);
		goto error1;
	}

	SDLNet_TCP_AddSocket(s->set, s->sd);
	return s;

error1:
	SDLNet_FreeSocketSet(s->set);
error2:
	free(s);
	return NULL;
}

int net_read_socket(net_socket *s, char *buf, unsigned int len) {
	int numready = SDLNet_CheckSockets(s->set, 0);

	if (numready == -1) {
		DBG_WARN("error polling socket");
		return -1;
	}

	if (numready == 0)
		return 0;

	int read = SDLNet_TCP_Recv(s->sd, buf, len);
	if (read <= 0)
		return -1; // Socket closed
	return read;
}

int net_write_socket(net_socket *s, char *buf, unsigned int len) {
	return SDLNet_TCP_Send(s->sd, buf, len);
}

void net_close_socket(net_socket *s) {
	SDLNet_TCP_Close(s->sd);
	SDLNet_FreeSocketSet(s->set);
}

#endif
