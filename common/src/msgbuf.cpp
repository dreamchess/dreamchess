/*  DreamChess
**
**  DreamChess is the legal property of its developers, whose names are too
**  numerous to list here. Please refer to the AUTHORS.txt file distributed
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

#include <cstdlib>
#include <cstring>

#include "msgbuf.h"

/* Pointer to message that's being received.
** Note: message refers to a series of characters followed by a newline.
*/
static char *msg;

char *msgbuf_process(char *buf)
{
    std::size_t buf_len = std::strlen(buf);

    if (buf_len > 0)
    {
        /* Characters from buf to end-1 will be added to the message. */
        char *end = std::strchr(buf, '\n');

        if (end)
            /* Include newline in message. */
            end++;
        else
            /* No newline, add everything to the message. */
            end = buf + buf_len;

        /* Size of data (in bytes) that will be added to the message. */
        std::size_t in_size = end - buf;

        if (msg)
        {
            /* We already have part of a message, realloc it and append
            ** data.
            */
            std::size_t len = std::strlen(msg);
            msg = (char *) std::realloc(msg, len + in_size + 1);
            std::strncpy(msg + len, buf, in_size);
            msg[len + in_size] = '\0';
        }
        else
        {
            /* Allocate a new string for the message. */
            msg = (char *) std::malloc(in_size + 1);
            std::strncpy(msg, buf, in_size);
            msg[in_size] = '\0';
        }

        /* Move the remaining data (if any) and a '\0' to the front of the
        ** buffer.
        */
        for (std::size_t i = 0; i < buf_len - in_size + 1; i++)
            buf[i] = end[i];

        end = std::strchr(msg, '\n');
        if (end)
        {
            char *retval = msg;

            /* Full message has been received. Chop off the newline. */
            *end = '\0';

            /* Chop off carriage return if it is present. */
            end = std::strrchr(msg, '\r');
            if (end && *end == '\r')
                *end = '\0';

            msg = nullptr;
            return retval;
        }
    }

    return nullptr;
}

void msgbuf_exit(void)
{
    if (msg)
        std::free(msg);

    msg = nullptr;
}
