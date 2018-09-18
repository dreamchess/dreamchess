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

#include <windows.h>
#include <stdio.h>

#include "e_comm.h"
#include "pipe_win32.h"

void e_comm_init(void)
{
    HANDLE h_in, h_out;
    DWORD mode;

    h_in = GetStdHandle(STD_INPUT_HANDLE);
    h_out = GetStdHandle(STD_OUTPUT_HANDLE);

    if ((h_out == INVALID_HANDLE_VALUE) || (h_in == INVALID_HANDLE_VALUE))
    {
        fprintf(stderr, "Error: could not get handles for standard input and output.");
        exit(1);
    }

    pipe_win32_init(h_in, h_out, GetConsoleMode(h_in, &mode) != 0);
}

void e_comm_exit(void)
{
    pipe_win32_exit();
}

void e_comm_send_str(const char *str)
{
    pipe_win32_send(str);
}

char *e_comm_poll(void)
{
    int error;
    char *retval = pipe_win32_poll(&error);

    if (error)
        exit(1);

    return retval;
}
