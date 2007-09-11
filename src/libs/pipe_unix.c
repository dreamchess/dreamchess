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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/select.h>

#include "msgbuf.h"

#define BUF_LEN 1024

/* Static buffer. */
static char buf[BUF_LEN];

static fd_set in_set;
static int fd_in, fd_out;

void pipe_unix_init(int in, int out)
{
    fd_in = in;
    fd_out = out;
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
    FD_ZERO(&in_set);
    FD_SET(fd_in, &in_set);

    /* Repeat until no more data is available, or a full message has been
    ** received.
    */
    while (1)
    {
        struct timeval timeout;
        char *msg;

        if ((msg = msgbuf_process(buf)))
            return msg;

        timeout.tv_sec = 0;
        timeout.tv_usec = 0;

        /* Poll for data. */
        if (select(fd_in + 1, &in_set, NULL, NULL, &timeout) == 1)
        {
            int len = strlen(buf);
            int bytes = read(fd_in, buf + len, BUF_LEN - len - 1);

            if (bytes < 0)
            {
                fprintf(stderr, "%s, L%d: %s\n", __FILE__, __LINE__,
                        strerror(errno));
                exit(-1);
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
        else
            /* No data available. */
            break;

    }
    return NULL;
}
