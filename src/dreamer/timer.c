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

#include <assert.h>
#include <sys/time.h>
#include <stdlib.h>
#include "timer.h"

/* Borrowed from libc.info */
static int timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y)
{
	/* Perform the carry for the later subtraction by updating y. */
	if (x->tv_usec < y->tv_usec) {
		int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
		y->tv_usec -= 1000000 * nsec;
		y->tv_sec += nsec;
	}
	if (x->tv_usec - y->tv_usec > 1000000) {
		int nsec = (x->tv_usec - y->tv_usec) / 1000000;
		y->tv_usec += 1000000 * nsec;
		y->tv_sec -= nsec;
	}

	/* Compute the time remaining to wait.
		tv_usec is certainly positive. */
	result->tv_sec = x->tv_sec - y->tv_sec;
	result->tv_usec = x->tv_usec - y->tv_usec;

	/* Return 1 if result is negative. */
	return x->tv_sec < y->tv_sec;
}

static int get_time(timer *c)
{
	struct timeval tv;

	if (!(c->flags & TIMER_RUNNING))
		return 0;

	gettimeofday(&tv, NULL);
	timeval_subtract(&tv, &tv, &c->start_time);

	return tv.tv_sec * 100 + tv.tv_usec / 10000;
}

int timer_get(timer *c)
{
	return c->val + (c->flags & TIMER_DOWN? -get_time(c) : get_time(c));
}

void timer_set(timer *c, int val)
{
	timer_stop(c);
	c->val = val;
}

void timer_start(timer *c)
{
        gettimeofday(&c->start_time, NULL);
	c->flags |= TIMER_RUNNING;
}

void timer_stop(timer *c)
{	c->val += (c->flags & TIMER_DOWN? -get_time(c) : get_time(c));
	c->flags &= ~TIMER_RUNNING;
}

void timer_init(timer *c, int down)
{
	timer_set(c, 0);
	c->flags = (down? TIMER_DOWN : 0);
}

