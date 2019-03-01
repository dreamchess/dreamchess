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

#ifndef GAMEGUI_SIGNAL_H
#define GAMEGUI_SIGNAL_H

#include <gamegui/queue.h>
#include <gamegui/system.h>

typedef int gg_signal_t;

gg_signal_t gg_signal_lookup(gg_class_id class, char *name);
int gg_signal_register(gg_class_id class, char *name);
void gg_signal_init(void);
void gg_signal_exit(void);

#endif
