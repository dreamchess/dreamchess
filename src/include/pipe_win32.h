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

#ifndef _PIPE_WIN32_H
#define _PIPE_WIN32_H

void pipe_win32_init(HANDLE in, HANDLE out, int console);
/* Initializes the I/O library with handles for input and output.
** Parameters: (HANDLE) in: handle to use for input.
**             (HANDLE) out: handle to use for output.
**             (int) console: 1 = console mode, 0 = pipe mode.
** Returns   : (void)
*/

void pipe_win32_exit(void);
/* Exits the I/O library cleanly.
** Parameters: (void)
** Returns   : (void)
*/

void pipe_win32_send(const char *m);
/* Sends a message through the I/O library.
** Parameters: (char *) m: Message to be sent.
** Returns   : (void)
*/

char *pipe_win32_poll(int *error);
/* Polls the I/O library for input.
** Parameters: (void)
** Returns   : (char *), Message that was read from the input handle
**                 (if any).
**             NULL, otherwise.
*/

#endif /* _PIPE_WIN32_H */
