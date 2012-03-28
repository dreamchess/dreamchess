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
#include "http_parser.h"
#include <SDL_net.h>

int net_init() {
	return SDL_Init(0) || SDLNet_Init();
}

typedef struct {
	int len;
	char *buf;
} recv_buf;

static int on_body_cb(http_parser *parser, const char *at, size_t length) {
	recv_buf *body = parser->data;
	body->buf = realloc(body->buf, body->len + length + 1);
	memcpy(body->buf + body->len, at, length);
	body->len += length;
	body->buf[body->len] = 0;
	return 0;
}

char *net_http_get(char *host, char *request) {
	IPaddress ip;
	TCPsocket sd;
	recv_buf body;
	body.buf = NULL;
	body.len = 0;
	char *buf = NULL;

	if (SDLNet_ResolveHost(&ip, host, 80)) {
		DBG_WARN("failed to resolve %s", host);
		return NULL;
	}

	if (!(sd = SDLNet_TCP_Open(&ip))) {
		DBG_WARN("failed to open TCP socket to %s", host);
		return NULL;
	}

	const int bufsize = 1024;
	buf = malloc(bufsize);
	int len = snprintf(buf, bufsize,
	  "GET %s HTTP/1.1\r\n"
	  "Host: %s\r\n"
	  "Accept-Encoding:\r\n"
	  "Connection: close\r\n\r\n",
	  request, host);

	if (len >= bufsize || SDLNet_TCP_Send(sd, buf, len) < len) {
		DBG_WARN("failed to send \"%s\" to %s", request, host);
		goto error;
	}

	http_parser_settings settings;
	memset(&settings, 0, sizeof(http_parser_settings));
	settings.on_body = on_body_cb;
	http_parser *parser = malloc(sizeof(http_parser));
	parser->data = &body;
	http_parser_init(parser, HTTP_RESPONSE);

	int parsed;
	do {
		len = SDLNet_TCP_Recv(sd, buf, bufsize - 1);

		if (len < 0)
			len = 0;

		parsed = http_parser_execute(parser, &settings, buf, len);
	} while (len != 0 && len == parsed);

	if (parser->status_code == 200) {
		free(buf);
		SDLNet_TCP_Close(sd);
		printf("%s\n", body.buf);
		return body.buf;
	}

	DBG_WARN("failed to download \"%s\" from %s", request, host);

error:
	if (buf)
		free(buf);
	if (body.buf)
		free(body.buf);
	SDLNet_TCP_Close(sd);
	return NULL;
}

#endif
