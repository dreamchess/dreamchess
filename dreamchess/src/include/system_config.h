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

#ifndef DREAMCHESS_SYSTEM_CONFIG_H
#define DREAMCHESS_SYSTEM_CONFIG_H

#include "options.h"

void config_init(void);
void config_save(void);
option_t *config_get_option(const char *name);
char *config_backup(void);
int config_restore(char *string);
void config_set_failsafe_video(void);

typedef struct config_resolution {
	int w, h;
} config_resolution_t;

#endif
