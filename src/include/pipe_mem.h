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

#ifndef _PIPE_MEM_H
#define _PIPE_MEM_H

void pipe_mem_init(char **pipe);
void pipe_mem_send(char **pipe, const char *msg);
char *pipe_mem_poll(char **pipe);
void pipe_mem_exit(char **pipe);

#if 0
void pipe_init(int in, int out);
/* Initializes the I/O library with file descriptors for input and output.
** Parameters: (int) in: File descriptor to use for input.
**             (int) out: File descriptor to use for output.
** Returns   : (void)
*/

void pipe_exit(void);
/* Exits the I/O library cleanly.
** Parameters: (void)
** Returns   : (void)
*/

void pipe_send(char *m);
/* Sends a message through the I/O library.
** Parameters: (char *) m: Message to be sent.
** Returns   : (void)
*/

int pipe_poll(void);
/* Polls the I/O library for input.
** Parameters: (void)
** Returns   : 1, if a message is ready to be received with pipe_receive().
**             0, otherwise.
*/

char *pipe_receive(void);
/* Receives a message through the I/O library. May only be called after
** io_pipe_poll() returns 1.
** Parameters: (void)
** Returns   : (char *), Message that was read from the input file descriptor.
*/

#endif
#endif /* _PIPE_MEM_H */
