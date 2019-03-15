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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "dir.h"
#include "debug.h"

static FILE *dbg_file;

void dbg_init(void) {
	ch_userdir();
	dbg_file = fopen("dreamchess.log", "w");
}

void dbg_exit(void) {
	if (dbg_file) {
		fclose(dbg_file);
		dbg_file = NULL;
	}
}

#ifdef HAVE_VARARGS_MACROS
void dbg_error(char *file, int line, const char *fmt, ...)
#else
void dbg_error(const char *fmt, ...)
#endif
{
	if (dbg_file) {
		va_list ap;

		fputs("ERROR: ", dbg_file);

		va_start(ap, fmt);
		vfprintf(dbg_file, fmt, ap);
		va_end(ap);

#ifdef HAVE_VARARGS_MACROS
		fprintf(dbg_file, " (%s:%d)", file, line);
#endif

		fputs("\n", dbg_file);
		fflush(dbg_file);
	}
}

#ifdef HAVE_VARARGS_MACROS
void dbg_warn(char *file, int line, const char *fmt, ...)
#else
void dbg_warn(const char *fmt, ...)
#endif
{
	if (dbg_file) {
		va_list ap;

		fputs("WARNING: ", dbg_file);

		va_start(ap, fmt);
		vfprintf(dbg_file, fmt, ap);
		va_end(ap);

#ifdef HAVE_VARARGS_MACROS
		fprintf(dbg_file, " (%s:%d)", file, line);
#endif

		fputs("\n", dbg_file);
		fflush(dbg_file);
	}
}

#ifdef HAVE_VARARGS_MACROS
void dbg_log(char *file, int line, const char *fmt, ...)
#else
void dbg_log(const char *fmt, ...)
#endif
{
	if (dbg_file) {
		va_list ap;

		va_start(ap, fmt);
		vfprintf(dbg_file, fmt, ap);
		va_end(ap);

#ifdef HAVE_VARARGS_MACROS
		fprintf(dbg_file, " (%s:%d)", file, line);
#endif

		fputs("\n", dbg_file);
		fflush(dbg_file);
	}
}
