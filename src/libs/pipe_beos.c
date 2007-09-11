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

/* Implements the pipe_unix interface. */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <fcntl.h>

#include "msgbuf.h"

#define BUF_LEN 1024

/* Static buffer. */
static char buf[BUF_LEN];

static int fd_in, fd_out;

void pipe_unix_init(int in, int out)
{
    fd_in = in;
    fd_out = out;

    fcntl(fd_in, F_SETFL, O_NONBLOCK);
}

void pipe_unix_exit()
{
    msgbuf_exit();
}

void pipe_unix_send(char *m)
{
    write(fd_out, m, strlen(m));
}

char *pipe_unix_poll()
{
    /* Repeat until no more data is available, or a full message has been
    ** received.
    */
    while (1)
    {
        char *msg;
        int len, bytes;

        if ((msg = msgbuf_process(buf)))
            return msg;

        /* Poll for data. */
        len = strlen(buf);
        bytes = read(fd_in, buf + len, BUF_LEN - len - 1);

        if (bytes < 0)
        {
            if (errno != EAGAIN)
            {
                fprintf(stderr, "%s, L%d: %s\n", __FILE__, __LINE__,
                        strerror(errno));
                exit(-1);
            }
            else
            {
                /* No data available. */
                break;
            }
        }
        else if (bytes == 0)
        {
            /* Received EOF. */
            fprintf(stderr, "%s, L%d: Broken pipe.\n", __FILE__, __LINE__);
            exit(-1);
        }
        else
            buf[len + bytes] = '\0';
    }

    return NULL;
}
