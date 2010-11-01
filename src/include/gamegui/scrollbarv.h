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

#ifndef GAMEGUI_SCROLLBARV_H
#define GAMEGUI_SCROLLBARV_H

#include <gamegui/system.h>
#include <gamegui/widget.h>

#define GG_SCROLLBARV(W) GG_CHECK_CAST(W, gg_scrollbarv_get_class_id(), gg_scrollbarv_t)

#define GG_SCROLLBARV_DATA \
    GG_WIDGET_DATA

typedef struct gg_scrollbarv
{
    GG_SCROLLBARV_DATA
}
gg_scrollbarv_t;

gg_class_id gg_scrollbarv_get_class_id();

void gg_scrollbarv_render(gg_widget_t * widget, int x, int y, int focus);

int gg_scrollbarv_input(gg_widget_t * widget, gg_event_t event);

void gg_scrollbarv_init(gg_scrollbarv_t * scrollbarv, int height);

gg_widget_t *gg_scrollbarv_create(int height);

#endif /* GAMEGUI_SCROLLBARV_H */
