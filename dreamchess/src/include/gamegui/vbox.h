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

#ifndef GAMEGUI_VBOX_H
#define GAMEGUI_VBOX_H

#include <gamegui/system.h>
#include <gamegui/box.h>

#define GG_VBOX(W) GG_CHECK_CAST(W, gg_vbox_get_class_id(), gg_vbox_t)

#define GG_VBOX_DATA \
    GG_BOX_DATA

typedef struct gg_vbox
{
    GG_VBOX_DATA
}
gg_vbox_t;

gg_class_id gg_vbox_get_class_id(void);

void gg_vbox_render(gg_widget_t *widget, int x, int y, int focus);

int gg_vbox_input(gg_widget_t *widget, gg_event_t event);

void gg_vbox_get_requested_size(gg_widget_t *widget, int *width, int *height);

void gg_vbox_set_size(gg_widget_t *widget, int width, int height);

gg_rect_t gg_vbox_get_focus_pos(gg_widget_t *widget);

int gg_vbox_set_focus_pos(gg_widget_t *widget, int x , int y);

void gg_vbox_init(gg_vbox_t *vbox, int spacing);

gg_widget_t *gg_vbox_create(int spacing);

void gg_vbox_set_selected(gg_widget_t *widget, int index );

#endif
