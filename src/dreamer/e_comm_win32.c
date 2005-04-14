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

#ifdef COMM_PIPE_WIN32

#include <windows.h>
#include <stdio.h>

#include "pipe_win32.h"

void e_comm_init()
{
    HANDLE h_in, h_out;

    h_in = GetStdHandle(STD_INPUT_HANDLE);
    h_out = GetStdHandle(STD_OUTPUT_HANDLE);

    if ((h_out == INVALID_HANDLE_VALUE) || (h_in == INVALID_HANDLE_VALUE))
    {
        fprintf(stderr, "Error: could not get handles for standard input and output.");
        exit(1);
    }

    pipe_win32_init(h_in, h_out);
}

void e_comm_exit()
{
    pipe_win32_exit();
}

void e_comm_send_str(const char *str)
{
    pipe_win32_send(str);
}

char *e_comm_poll()
{
    return pipe_win32_poll();
}

#endif /* COMM_PIPE_WIN32 */
