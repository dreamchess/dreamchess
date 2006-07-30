/*  DreamChess
 *  Copyright (C) 2005-2006  The DreamChess project
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

/** @file
 *  @brief Viewport widget.
 */

#ifndef GAMEGUI_VIEWPORT_H
#define GAMEGUI_VIEWPORT_H

#include <gamegui/system.h>
#include <gamegui/bin.h>

/** Typecast to viewport widget. */
#define GG_VIEWPORT(W) GG_CHECK_CAST(W, gg_viewport_get_class_id(), gg_viewport_t)

#define GG_VIEWPORT_DATA \
    GG_BIN_DATA \
    float xscroll; \
    float yscroll;

typedef struct gg_viewport
{
    GG_VIEWPORT_DATA
}
gg_viewport_t;

gg_class_id gg_viewport_get_class_id();

void gg_viewport_render(gg_widget_t *widget, int x, int y, int focus);

void gg_viewport_init(gg_viewport_t *viewport, gg_widget_t *widget, int width, int height);

void gg_viewport_set_scroll_xpos(gg_viewport_t *viewport, float scroll);

void gg_viewport_set_scroll_ypos(gg_viewport_t *viewport, float scroll);

void gg_viewport_set_size(gg_widget_t *widget, int width, int height);

gg_rect_t gg_viewport_get_focus_pos(gg_widget_t *widget);

int gg_viewport_set_focus_pos(gg_widget_t *widget, int x, int y);

int gg_viewport_input(gg_widget_t *widget, gg_event_t event);

gg_widget_t *gg_viewport_create(gg_widget_t *widget, int width, int height);

#endif /* GAMEGUI_VIEWPORT_H */
