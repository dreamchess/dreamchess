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

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "config.h"
#include "debug.h"

static int dbg_level = 1;

/* Debug levels:
** 0: Silent
** 1: Errors
** 2: Errors + Warnings
** 3: Errors + Warnings + Log
*/

void dbg_set_level(int level)
{
    dbg_level = level;
}

#ifdef HAVE_VARARGS_MACROS
void dbg_error(char *file, int line, const char *fmt, ...)
#else
void dbg_error(const char *fmt, ...)
#endif
{
    va_list ap;

    if (dbg_level < 1)
        return;

#ifdef HAVE_VARARGS_MACROS
    fprintf(stderr, "%s:%d: ", file, line);
#endif
    fprintf(stderr, "error: ");

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    fprintf(stderr, "\n");
}

#ifdef HAVE_VARARGS_MACROS
void dbg_warn(char *file, int line, const char *fmt, ...)
#else
void dbg_warn(const char *fmt, ...)
#endif
{
    va_list ap;

    if (dbg_level < 2)
        return;

#ifdef HAVE_VARARGS_MACROS
    printf("%s:%d: ", file, line);
#endif
    printf("warning: ");

    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);

    printf("\n");
}

#ifdef HAVE_VARARGS_MACROS
void dbg_log(char *file, int line, const char *fmt, ...)
#else
void dbg_log(const char *fmt, ...)
#endif
{
    va_list ap;

    if (dbg_level < 3)
        return;

#ifdef HAVE_VARARGS_MACROS
    printf("%s:%d: ", file, line);
#endif

    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);

    printf("\n");
}
