/*  DreamChess
**  Copyright (C) 2004  The DreamChess project
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

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "comm.h"
#include "debug.h"

void comm_send(const char *fmt, ...)
{
    /* Code adapted from example in PRINTF(3) */
    /* Guess we need no more than 100 bytes. */
    int n, size = 100;
    char *p;
    va_list ap;
    p = malloc(size);
    while (1)
    {
        /* Try to print in the allocated space. */
        va_start(ap, fmt);
        n = vsnprintf(p, size, fmt, ap);
        va_end(ap);
        /* If that worked, return the string. */
        if (n > -1 && n < size)
            break;
        /* Else try again with more space. */
        if (n > -1)
            /* glibc 2.1, set size to precisely what's needed. */
            size = n+1;
        else
            /* glibc 2.0, try with twice the old size. */
            size *= 2;
        p = realloc(p, size);
    }

    comm_send_str(p);
    if (p[strlen(p) - 1] == '\n')
        p[strlen(p) - 1] = 0;
    DBG_LOG("message to engine: '%s'", p);
    free(p);
}
