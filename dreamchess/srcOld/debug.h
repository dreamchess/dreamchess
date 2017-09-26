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

#ifndef DREAMCHESS_DEBUG_H
#define DREAMCHESS_DEBUG_H

#include "config.h"

#ifdef HAVE_C99_VARARGS_MACROS
#define DBG_ERROR(...) do {dbg_error(__FILE__, __LINE__, __VA_ARGS__);} while (0)
#define DBG_WARN(...) do {dbg_warn(__FILE__, __LINE__, __VA_ARGS__);} while (0)
#define DBG_LOG(...) do {dbg_log(__FILE__, __LINE__, __VA_ARGS__);} while (0)
#elif defined(HAVE_GNUC_VARARGS_MACROS)
#define DBG_ERROR(args...) do {dbg_error(__FILE__, __LINE__, args);} while (0)
#define DBG_WARN(args...) do {dbg_warn(__FILE__, __LINE__, args);} while (0)
#define DBG_LOG(args...) do {dbg_log(__FILE__, __LINE__, args);} while (0)
#else
#define DBG_ERROR dbg_error
#define DBG_WARN dbg_warn
#define DBG_LOG dbg_log
#endif

#if defined(HAVE_C99_VARARGS_MACROS) || defined(HAVE_GNUC_VARARGS_MACROS)
#define HAVE_VARARGS_MACROS
#endif

void dbg_set_level(int level);
#ifdef HAVE_VARARGS_MACROS
void dbg_error(const char *file, int line, const char *fmt, ...);
void dbg_warn(const char *file, int line, const char *fmt, ...);
void dbg_log(const char *file, int line, const char *fmt, ...);
#else
void dbg_error(const char *fmt, ...);
void dbg_warn(const char *fmt, ...);
void dbg_log(const char *fmt, ...);
#endif

#endif
