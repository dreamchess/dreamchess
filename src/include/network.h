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

#ifndef NETWORK_H
#define NETWORK_H

struct net_http_handle;
typedef struct net_http_handle net_http_handle;
struct net_socket;
typedef struct net_socket net_socket;
struct net_server_socket;
typedef struct net_server_socket net_server_socket;

// Generic socket functions
int net_init();
net_socket *net_connect(const char *host, unsigned int port);
net_server_socket *net_listen(unsigned int port);
net_socket *net_accept(net_server_socket *socket);
int net_read(net_socket *s, char *buf, unsigned int len);
int net_write(net_socket *s, char *buf, unsigned int len);
void net_close(net_socket *s);
void net_close_server(net_server_socket *s);

// HTTP read functions
net_http_handle *net_http_open(char *host, char *request);
int net_http_poll(net_http_handle *h);
char *net_http_close(net_http_handle *h);

#endif
