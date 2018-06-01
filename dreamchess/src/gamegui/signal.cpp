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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gamegui/widget.h>

typedef struct signal {
	gg_class_id class_id;
	const char *name;
} signal_t;

static signal_t *signals;
static int signals_nr;
static int signals_max;

gg_signal_t gg_signal_lookup(gg_class_id class_id, const char *name)
{
	int i;

	for (i = 0; i < signals_nr; i++) {
		/* Check for name in parent classes as well */
		if (!strcmp(name, signals[i].name) &&
			((signals[i].class_id == class_id) || gg_is_parent(signals[i].class_id, class_id))) {
			return i;
		}
	}

	return -1;
}

int gg_signal_register(gg_class_id class_id, const char *name)
{
	if (gg_signal_lookup(class_id, name) != -1)
		return -1;

	signals_nr++;
	if (signals_nr > signals_max) {
		signals_max = signals_nr * 2;
		signals = (signal_t *)realloc(signals, sizeof(signal_t) * signals_max);
	}

	signals[signals_nr - 1].class_id = class_id;
	signals[signals_nr - 1].name = name;

	return signals_nr - 1;
}

void gg_signal_init(void)
{
}

void gg_signal_exit(void)
{
	free(signals);
}
