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

#ifndef GAMEGUI_ACTION_H
#define GAMEGUI_ACTION_H

#include <gamegui/system.h>
#include <gamegui/bin.h>

#define W_ACTION(W) CHECK_CAST(W, w_action_get_class_id(), w_action_t)

#define W_ACTION_DATA \
    W_BIN_DATA \
    void (* func) (w_widget_t *widget, void *data); \
    void *func_data;

typedef struct w_action
{
    W_ACTION_DATA
}
w_action_t;

w_class_id w_action_get_class_id();

void w_action_render(w_widget_t *widget, int x, int y, int focus);

int w_action_input(w_widget_t *widget, ui_event_t event);

void w_action_init(w_action_t *action, w_widget_t *widget);

w_widget_t *w_action_create(w_widget_t *widget);

w_widget_t *w_action_create_with_label(char *text, float xalign, float yalign);

void w_action_set_callback(w_action_t *action, void (* callback) (w_widget_t *, void *), void *func_data);

#endif /* GAMEGUI_ACTION_H */
