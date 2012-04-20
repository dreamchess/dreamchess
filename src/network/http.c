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

#include "debug.h"
#include "network.h"
#include "http_parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define BUF_SIZE 1024

struct net_http_handle {
	char *data; // Parsed data
	int len; // Length of parsed data
	char buf[BUF_SIZE]; // Read buffer for HTTP
	net_socket *socket;
	http_parser_settings settings;
	http_parser *parser;
	int complete;
};

static int on_body_cb(http_parser *parser, const char *at, size_t length) {
	net_http_handle *h = parser->data;
	h->data = realloc(h->data, h->len + length + 1);
	memcpy(h->data + h->len, at, length);
	h->len += length;
	h->data[h->len] = 0;
	return 0;
}

static int on_message_complete_cb(http_parser *parser) {
	net_http_handle *h = parser->data;
	h->complete = 1;
	return 0;
}

net_http_handle *net_http_open(char *host, char *request) {
	struct net_http_handle *h = malloc(sizeof(struct net_http_handle));
	h->socket = net_connect(host, 80);
	if (!h->socket) {
		free(h);
		return NULL;
	}

	int len = snprintf(h->buf, BUF_SIZE,
	  "GET %s HTTP/1.1\r\n"
	  "Host: %s\r\n"
	  "Accept-Encoding:\r\n"
	  "Connection: close\r\n\r\n",
	  request, host);

	if (len >= BUF_SIZE || net_write(h->socket, h->buf, len) < len) {
		DBG_WARN("failed to send \"%s\" to %s", request, host);
		net_close(h->socket);
		free(h);
		return NULL;
	}

	h->data = NULL;
	h->len = 0;
	h->complete = 0;
	memset(&h->settings, 0, sizeof(http_parser_settings));
	h->settings.on_body = on_body_cb;
	h->settings.on_message_complete = on_message_complete_cb;
	h->parser = malloc(sizeof(http_parser));
	h->parser->data = h;
	http_parser_init(h->parser, HTTP_RESPONSE);
	return h;
}

int net_http_poll(net_http_handle *h) {
	int parsed, len;
	while (1) {
		len = net_read(h->socket, h->buf, BUF_SIZE - 1);
		if (len == 0) // No data yet
			break;

		if (len < 0) // Socket closed
			len = 0;

		parsed = http_parser_execute(h->parser, &h->settings, h->buf, len);
		if (len == 0 || len != parsed)
			return -1;
	}

	return 0;
}

char *net_http_close(net_http_handle *h) {
	int ok = h->parser->status_code == 200 && h->complete;
	net_close(h->socket);
	char *data = h->data;
	free(h->parser);
	free(h);

	if (ok)
		return data;

	DBG_WARN("http download failed");

	if (data)
		free(data);
	return NULL;
}
