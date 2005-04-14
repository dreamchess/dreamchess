/*  DreamChess
**  Copyright (C) 2004  The DreamChess project
**
**  This program is free software; you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation; either version 2 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdlib.h>
#include <string.h>

/* Pointer to message that's being received.
** Note: message refers to a series of characters followed by a newline.
*/
static char *msg;

char *msgbuf_process(char *buf)
{
    int buf_len = strlen(buf);

    if (buf_len > 0)
    {
        /* Characters from buf to end-1 will be added to the message. */
        char *end = strchr(buf, '\n');
        int in_size, i;

        if (end)
            /* Include newline in message. */
            end++;
        else
            /* No newline, add everything to the message. */
            end = buf + buf_len;

        /* Size of data (in bytes) that will be added to the message. */
        in_size = end - buf;

        if (msg)
        {
            /* We already have part of a message, realloc it and append
            ** data.
            */
            int len = strlen(msg);
            msg = (char *) realloc(msg, len + in_size + 1);
            strncpy(msg + len, buf, in_size);
            msg[len + in_size] = '\0';
        }
        else
        {
            /* Allocate a new string for the message. */
            msg = (char *) malloc(in_size + 1);
            strncpy(msg, buf, in_size);
            msg[in_size] = '\0';
        }

        /* Move the remaining data (if any) and a '\0' to the front of the
        ** buffer.
        */
        for (i = 0; i < buf_len - in_size + 1; i++)
            buf[i] = end[i];

        end = strchr(msg, '\n');
        if (end)
        {
            char *retval = msg;

            /* Full message has been received. Chop off the newline. */
            *end = '\0';
            msg = NULL;
            return retval;
        }
    }

    return NULL;
}

void msgbuf_exit()
{
    if (msg)
        free(msg);
}
