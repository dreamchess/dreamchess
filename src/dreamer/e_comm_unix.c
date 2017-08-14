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
#include <stdarg.h>
#include <signal.h>

#include "e_comm.h"
#include "pipe_unix.h"

void e_comm_init(void)
{
    /* xboard ui's may send SIGINT to stop thinking or pondering.
    ** We don't need this signal, so we ignore it.
    */
    signal(SIGINT, SIG_IGN);
    
    /* Setup stdin (0) as input and stdout (1) as output. */
    pipe_unix_init(0, 1);
}

void e_comm_exit(void)
{
    pipe_unix_exit();
}

void e_comm_send_str(const char *str)
{
    pipe_unix_send(str);
}

char *e_comm_poll(void)
{
    int error;
    char *retval = pipe_unix_poll(&error);

    if (error)
        exit(1);

    return retval;
}
