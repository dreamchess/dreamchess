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

#ifndef GAMEGUI_SELECT_H
#define GAMEGUI_SELECT_H

#include <gamegui/system.h>
#include <gamegui/bin.h>

#define GG_SELECT(W) GG_CHECK_CAST(W, gg_select_get_class_id(), gg_select_t)

#define GG_SELECT_DATA \
    GG_CONTAINER_DATA \
    int sel;

typedef struct gg_select
{
    GG_SELECT_DATA
}
gg_select_t;

gg_class_id gg_select_get_class_id();

void gg_select_init(gg_select_t *select);

int gg_select_prev(gg_select_t *select, int input, int enabled);

int gg_select_next(gg_select_t *select, int input, int enabled);

#endif /* GAMEGUI_SELECT_H */
