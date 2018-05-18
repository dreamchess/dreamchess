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

#ifndef GAMEGUI_FRAME_H
#define GAMEGUI_FRAME_H

#include <gamegui/system.h>
#include <gamegui/bin.h>

#define GG_FRAME(W) GG_CHECK_CAST(W, gg_frame_get_class_id(), gg_frame_t)

#define GG_FRAME_DATA \
    GG_BIN_DATA

typedef struct gg_frame
{
    GG_FRAME_DATA
}
gg_frame_t;

gg_class_id gg_frame_get_class_id(void);

void gg_frame_render(gg_widget_t *widget, int x, int y, int focus);

void gg_frame_init(gg_frame_t *frame, gg_widget_t *child);

gg_widget_t *gg_frame_create(gg_widget_t *child);

gg_rect_t gg_frame_get_focus_pos(gg_widget_t *widget);

int gg_frame_set_focus_pos(gg_widget_t *widget, int x, int y);

int gg_frame_input(gg_widget_t *widget, gg_event_t event);

#endif
