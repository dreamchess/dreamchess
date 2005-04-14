/*  DreamChess
**  Copyright (C) 2004-2005  The DreamChess project
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#ifdef COMM_PIPE_UNIX

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>

#include "pipe_unix.h"

void e_comm_init()
{
    /* xboard ui's may send SIGINT to stop thinking or pondering.
    ** We don't need this signal, so we ignore it.
    */
    signal(SIGINT, SIG_IGN);
    
    /* Setup stdin (0) as input and stdout (1) as output. */
    pipe_unix_init(0, 1);
}

void e_comm_exit()
{
    pipe_unix_exit();
}

void e_comm_send_str(char *str)
{
    pipe_unix_send(str);
}

char *e_comm_poll()
{
    return pipe_unix_poll();
}

#endif /* COMM_PIPE_UNIX */
