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

#ifdef COMM_SDL_THREADS

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "pipe_mem.h"
#include "main_sdlthd.h"

void e_comm_init()
{
}

void e_comm_exit()
{
}

void e_comm_send_str(char *str)
{
    SDL_mutexP(to_ui_mutex);
    pipe_mem_send(&to_ui, str);
    SDL_mutexV(to_ui_mutex);
}

char *e_comm_poll()
{
    char *retval;

    SDL_mutexP(to_engine_mutex);
    retval = pipe_mem_poll(&to_engine);
    SDL_mutexV(to_engine_mutex);
    return retval;
}

#endif /* COMM_SDL_THREADS */
