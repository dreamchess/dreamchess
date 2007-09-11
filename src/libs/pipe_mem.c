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

#include <stdlib.h>
#include <string.h>

#include "msgbuf.h"

void pipe_mem_init(char **pipe)
{
    *pipe = malloc(1);
    *pipe[0] = '\0';
}

void pipe_mem_send(char **pipe, char *msg)
{
    *pipe = realloc(*pipe, strlen(*pipe) + strlen(msg) + 1);
    strcat(*pipe, msg);
}

char *pipe_mem_poll(char **pipe)
{
    return msgbuf_process(*pipe);
}

void pipe_mem_exit(char **pipe)
{
    free(*pipe);
    msgbuf_exit();
}
