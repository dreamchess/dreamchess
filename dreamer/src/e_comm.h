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

#ifndef DREAMER_E_COMM_H
#define DREAMER_E_COMM_H

void e_comm_init(void);
/* Initializes the I/O library for communication with an xboard ui.
** Parameters: (void)
** Returns   : (void)
*/

void e_comm_exit(void);
/* Exits the I/O library cleanly.
** Parameters: (void)
** Returns   : (void)
*/

void e_comm_send(const char *fmt, ...);
/* Sends a message to the xboard ui.
** Parameters: (char *) fmt, ...: Message to be sent.
** Returns   : (void)
*/

void e_comm_send_str(const char *m);
/* Sends a message to the xboard ui.
** Parameters: (char *) m: Message to be sent.
** Returns   : (void)
*/

char *e_comm_poll(void);
/* Polls the I/O library for input from the xboard ui.
** Parameters: (void)
** Returns   : (char *), Message that was received from the I/O library (if
**                 any).
**             NULL, otherwise.
*/

#endif
