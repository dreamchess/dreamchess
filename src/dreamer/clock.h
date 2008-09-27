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

#include <sys/time.h>

#ifndef CLOCK_H
#define CLOCK_H

#define CLOCK_DOWN (1 << 0)
#define CLOCK_RUNNING (1 << 1)

typedef struct
{
	int flags;
	int val; /* Current value */
	struct timeval start_time; /* Time when clock was started */
} clock;

int clock_get(clock *c);
void clock_set(clock *c, int val);
void clock_start(clock *c);
void clock_stop(clock *c);
void clock_init(clock *c, int down);

#endif /* CLOCK_H */
