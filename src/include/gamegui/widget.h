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

#ifndef GAMEGUI_WIDGET_H
#define GAMEGUI_WIDGET_H

#include <gamegui/system.h>
/* FIXME */
#include <../ui.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#define W_WIDGET(W) CHECK_CAST(W, w_widget_get_class_id(), w_widget_t)

#define W_WIDGET_DATA \
    void (* render) (struct w_widget *widget, int x, int y, int focus); \
    int (* input) (struct w_widget *widget, ui_event_t event); \
    void (* set_size) (struct w_widget *widget, int width, int height); \
    void (* get_requested_size) (struct w_widget *widget, int *width, int *height); \
    void (* get_focus_pos) (struct w_widget *widget, int *x, int *y); \
    int (* set_focus_pos) (struct w_widget *widget, int x, int y); \
    void (* destroy) (struct w_widget *widget); \
    w_class_id id; \
    struct w_widget *parent; \
    int enabled; \
    int width; \
    int height; \
    int width_f; \
    int height_f; \
    int width_a; \
    int height_a;

typedef struct w_widget
{
    W_WIDGET_DATA
}
w_widget_t;

w_class_id w_widget_get_class_id();

void w_widget_destroy(w_widget_t *widget);

void w_set_requested_size(w_widget_t *widget, int width, int height);

void w_widget_get_requested_size(w_widget_t *widget, int *width, int *height);

void w_set_size(w_widget_t *widget, int width, int height);

void w_get_focus_pos(w_widget_t *widget, int *x, int *y);

int w_set_focus_pos(w_widget_t *widget, int x, int y);

void w_widget_init(w_widget_t *widget);

#endif /* GAMEGUI_WIDGET_H */
