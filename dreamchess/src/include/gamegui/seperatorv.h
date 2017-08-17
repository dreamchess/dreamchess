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

#ifndef GAMEGUI_SEPERATORV_H
#define GAMEGUI_SEPERATORV_H

#include <gamegui/system.h>
#include <gamegui/widget.h>

#define GG_SEPERATORV(W) GG_CHECK_CAST(W, gg_seperatorv_get_class_id(), gg_seperatorv_t)

#define GG_SEPERATORV_DATA \
    GG_WIDGET_DATA

typedef struct gg_seperatorv
{
    GG_SEPERATORV_DATA
}
gg_seperatorv_t;

gg_class_id gg_seperatorv_get_class_id(void);

void gg_seperatorv_render(gg_widget_t *widget, int x, int y, int focus);

void gg_seperatorv_init(gg_seperatorv_t *sep);

gg_widget_t *gg_seperatorv_create(void);

#endif
