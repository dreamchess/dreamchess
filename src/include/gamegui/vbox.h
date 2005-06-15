/*  DreamChess
 *  Copyright (C) 2005  The DreamChess project
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef GAMEGUI_VBOX_H
#define GAMEGUI_VBOX_H

#include <gamegui/system.h>
#include <gamegui/box.h>

#define W_VBOX(W) CHECK_CAST(W, w_vbox_get_class_id(), w_vbox_t)

#define W_VBOX_DATA \
    W_BOX_DATA

typedef struct w_vbox
{
    W_VBOX_DATA
}
w_vbox_t;

w_class_id w_vbox_get_class_id();

void w_vbox_render(w_widget_t *widget, int x, int y, int focus);

int w_vbox_input(w_widget_t *widget, ui_event_t event);

void w_vbox_get_requested_size(w_widget_t *widget, int *width, int *height);

void w_vbox_set_size(w_widget_t *widget, int width, int height);

void w_vbox_get_focus_pos(w_widget_t *widget, int *x , int *y);

int w_vbox_set_focus_pos(w_widget_t *widget, int x , int y);

void w_vbox_init(w_vbox_t *vbox, int spacing);

w_widget_t *w_vbox_create(int spacing);

#endif /* GAMEGUI_VBOX_H */