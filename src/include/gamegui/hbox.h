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

#ifndef GAMEGUI_HBOX_H
#define GAMEGUI_HBOX_H

#include <gamegui/system.h>
#include <gamegui/box.h>

#define W_HBOX(W) CHECK_CAST(W, w_hbox_get_class_id(), w_hbox_t)

#define W_HBOX_DATA \
    W_BOX_DATA

typedef struct w_hbox
{
    W_HBOX_DATA
}
w_hbox_t;

w_class_id w_hbox_get_class_id();

void w_hbox_render(w_widget_t *widget, int x, int y, int focus);

int w_hbox_input(w_widget_t *widget, ui_event_t event);

void w_hbox_get_requested_size(w_widget_t *widget, int *width, int *height);

void w_hbox_set_size(w_widget_t *widget, int width, int height);

void w_hbox_get_focus_pos(w_widget_t *widget, int *x , int *y);

int w_hbox_set_focus_pos(w_widget_t *widget, int x , int y);

void w_hbox_init(w_hbox_t *hbox, int spacing);

w_widget_t *w_hbox_create(int spacing);

#endif /* GAMEGUI_HBOX_H */
