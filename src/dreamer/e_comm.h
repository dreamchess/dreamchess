/*  DreamChess
**  Copyright (C) 2003-2005  The DreamChess project
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

#ifndef E_COMM_H
#define E_COMM_H

void e_comm_init();
/* Initializes the I/O library for communication with an xboard ui.
** Parameters: (void)
** Returns   : (void)
*/

void e_comm_exit();
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

char *e_comm_poll();
/* Polls the I/O library for input from the xboard ui.
** Parameters: (void)
** Returns   : (char *), Message that was received from the I/O library (if
**                 any).
**             NULL, otherwise.
*/

#endif /* E_COMM_H */
