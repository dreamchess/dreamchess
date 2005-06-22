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

#define GG_WIDGET(W) GG_CHECK_CAST(W, gg_widget_get_class_id(), gg_widget_t)

#define GG_WIDGET_DATA \
    void (* render) (struct gg_widget *widget, int x, int y, int focus); \
    int (* input) (struct gg_widget *widget, ui_event_t event); \
    void (* set_size) (struct gg_widget *widget, int width, int height); \
    void (* get_requested_size) (struct gg_widget *widget, int *width, int *height); \
    void (* get_focus_pos) (struct gg_widget *widget, int *x, int *y); \
    int (* set_focus_pos) (struct gg_widget *widget, int x, int y); \
    void (* destroy) (struct gg_widget *widget); \
    gg_class_id id; \
    struct gg_widget *parent; \
    int enabled; \
    int width; \
    int height; \
    int width_f; \
    int height_f; \
    int width_a; \
    int height_a;

typedef struct gg_widget
{
    GG_WIDGET_DATA
}
gg_widget_t;

gg_class_id gg_widget_get_class_id();

void gg_widget_destroy(gg_widget_t *widget);

void gg_set_requested_size(gg_widget_t *widget, int width, int height);

void gg_widget_get_requested_size(gg_widget_t *widget, int *width, int *height);

void gg_set_size(gg_widget_t *widget, int width, int height);

void gg_get_focus_pos(gg_widget_t *widget, int *x, int *y);

int gg_set_focus_pos(gg_widget_t *widget, int x, int y);

void gg_widget_init(gg_widget_t *widget);

#endif /* GAMEGUI_WIDGET_H */
