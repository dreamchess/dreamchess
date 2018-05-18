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

#ifndef GAMEGUI_HBOX_H
#define GAMEGUI_HBOX_H

#include <gamegui/system.h>
#include <gamegui/box.h>

#define GG_HBOX(W) GG_CHECK_CAST(W, gg_hbox_get_class_id(), gg_hbox_t)

#define GG_HBOX_DATA \
    GG_BOX_DATA

typedef struct gg_hbox
{
    GG_HBOX_DATA
}
gg_hbox_t;

gg_class_id gg_hbox_get_class_id(void);

void gg_hbox_render(gg_widget_t *widget, int x, int y, int focus);

int gg_hbox_input(gg_widget_t *widget, gg_event_t event);

void gg_hbox_get_requested_size(gg_widget_t *widget, int *width, int *height);

void gg_hbox_set_size(gg_widget_t *widget, int width, int height);

gg_rect_t gg_hbox_get_focus_pos(gg_widget_t *widget);

int gg_hbox_set_focus_pos(gg_widget_t *widget, int x , int y);

void gg_hbox_init(gg_hbox_t *hbox, int spacing);

gg_widget_t *gg_hbox_create(int spacing);

#endif
