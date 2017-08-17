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

#ifndef DREAMER_TIMER_H
#define DREAMER_TIMER_H

#ifdef _MSC_VER
/* Temporary hack for gettimeofday() */

#include <Winsock2.h>
#undef min
#undef max

int gettimeofday(struct timeval *tp, void *tzp);
#else
#include <sys/time.h>
#endif

#define TIMER_DOWN (1 << 0)
#define TIMER_RUNNING (1 << 1)

typedef struct
{
	int flags;
	int val; /* Current value */
	struct timeval start_time; /* Time when clock was started */
} timer;

int timer_get(timer *c);
void timer_set(timer *c, int val);
void timer_start(timer *c);
void timer_stop(timer *c);
void timer_init(timer *c, int down);

#endif
