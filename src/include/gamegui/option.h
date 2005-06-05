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

#ifndef GAMEGUI_OPTION_H
#define GAMEGUI_OPTION_H

#include <gamegui/system.h>
#include <gamegui/select.h>

#define W_OPTION(W) CHECK_CAST(W, w_option_get_class_id(), w_option_t)

#define W_OPTION_DATA \
    W_SELECT_DATA \
    void (* func) (w_widget_t *widget, void *data); \
    void *func_data;

typedef struct w_option
{
    W_OPTION_DATA
}
w_option_t;

void w_option_render(w_widget_t *widget, int x, int y, int focus);

int w_option_input(w_widget_t *widget, ui_event_t event);

void w_option_set_size(w_widget_t *widget, int width, int height);

void w_option_init(w_option_t *option);

w_widget_t *w_option_create();

void w_option_append(w_option_t *option, w_widget_t *child);

void w_option_append_label(w_option_t *option, char *text, float xalign, float yalign);

int w_option_get_selected(w_option_t *option);

void w_option_set_callback(w_option_t *option, void (* callback) (w_widget_t *, void *), void *func_data);

#endif /* GAMEGUI_OPTION_H */
