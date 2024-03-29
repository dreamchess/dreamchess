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

#ifndef DREAMCHESS_DEBUG_H
#define DREAMCHESS_DEBUG_H

#include "config.h"

#define DBG_ERROR(...)                                                                                                 \
	do {                                                                                                               \
		dbg_error(__FILE__, __LINE__, __VA_ARGS__);                                                                    \
	} while (0)
#define DBG_WARN(...)                                                                                                  \
	do {                                                                                                               \
		dbg_warn(__FILE__, __LINE__, __VA_ARGS__);                                                                     \
	} while (0)
#define DBG_LOG(...)                                                                                                   \
	do {                                                                                                               \
		dbg_log(__FILE__, __LINE__, __VA_ARGS__);                                                                      \
	} while (0)

void dbg_init(void);
void dbg_exit(void);
void dbg_error(char *file, int line, const char *fmt, ...);
void dbg_warn(char *file, int line, const char *fmt, ...);
void dbg_log(char *file, int line, const char *fmt, ...);

#endif
