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
#include <windows.h>

#include "msgbuf.h"

#define BUF_LEN 1024

/* Static buffer. */
static char buf[BUF_LEN];

static HANDLE h_in, h_out;

void pipe_win32_init(HANDLE in, HANDLE out)
{
    h_in = in;
    h_out = out;
}

void pipe_win32_exit()
{
    msgbuf_exit();
}

void pipe_win32_send(char *m)
{
    DWORD written;

    if (!WriteFile(h_out, m, strlen(m), &written, NULL) ||
        (written < strlen(m)))
    {
        fprintf(stderr, "%s, L%d: Error writing to pipe.\n", __FILE__,
            __LINE__);
        exit(1);
    }
}

char *pipe_win32_poll(int *error)
{
    *error = 0;

    /* Repeat until no more data is available, or a full message has been
    ** received.
    */
    while (1)
    {
        DWORD bytes;
        char *msg;
        int len;

        if ((msg = msgbuf_process(buf)))
            return msg;

        len = strlen(buf);

        /* Check whether data is available. */
        if (!PeekNamedPipe(h_in, NULL, 0, NULL, &bytes, NULL))
        {
            /* Error reading pipe. */
            fprintf(stderr, "%s, L%d: Broken pipe.\n", __FILE__, __LINE__);
            *error = 1;
            return NULL;
        }

        if (bytes > 0)
        {
            /* Read data. */
            if (!ReadFile(h_in, buf + len, BUF_LEN - len - 1, &bytes, NULL))
            {
                /* Error reading pipe. */
                fprintf(stderr, "%s, L%d: Broken pipe.\n", __FILE__, __LINE__);
                *error = 1;
                return NULL;
            }

            if (bytes == 0)
            {
                /* Received EOF. */
                fprintf(stderr, "%s, L%d: Broken pipe.\n", __FILE__, __LINE__);
                *error = 1;
                return NULL;
            }

            buf[len + bytes] = '\0';
        }
        else
            /* No data available. */
            break;
    }
    return NULL;
}
