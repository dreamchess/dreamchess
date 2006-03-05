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

#include <stdlib.h>

#include <gamegui/viewport.h>
#include <gamegui/clipping.h>

gg_class_id gg_viewport_get_class_id()
{
    GG_CHILD(gg_bin_get_class_id())
}

/** Implements widget::render for viewport widgets. */
void gg_viewport_render(gg_widget_t *widget, int x, int y, int focus)
{
    gg_viewport_t *viewport = GG_VIEWPORT(widget);
    gg_widget_t *child = gg_bin_get_child(GG_BIN(widget));
    gg_rect_t rect;
    int xoff = 0;
    int yoff = 0;

    if (child->width_a > widget->width_a)
        xoff = viewport->xscroll * (child->width_a - widget->width_a);

    if (child->height_a > widget->height_a)
        yoff = viewport->yscroll * (child->height_a - widget->height_a);

    rect.x = x;
    rect.y = y;
    rect.width = widget->width_a;
    rect.height = widget->height_a;

    gg_clipping_adjust(&rect);
    child->render(child, x - xoff, y - yoff, focus);
    gg_clipping_undo();
}

void gg_viewport_init(gg_viewport_t *viewport, gg_widget_t *widget, int width, int height)
{
    gg_bin_init((gg_bin_t *) viewport, widget);

    viewport->render = gg_viewport_render;
    viewport->id = gg_viewport_get_class_id();
    viewport->xscroll = 0;
    viewport->yscroll = 0;
    viewport->width = width;
    viewport->height = height;
}

gg_widget_t *gg_viewport_create(gg_widget_t *widget, int width, int height)
{
    gg_viewport_t *viewport = malloc(sizeof(gg_viewport_t));

    gg_viewport_init(viewport, widget, width, height);

    return GG_WIDGET(viewport);
}

void gg_viewport_set_scroll_xpos(gg_viewport_t *viewport, float scroll)
{
    gg_widget_t *child = gg_bin_get_child(GG_BIN(viewport));

    if (child->width_a > viewport->width_a)
        viewport->xscroll = scroll;
}

void gg_viewport_set_scroll_ypos(gg_viewport_t *viewport, float scroll)
{
    gg_widget_t *child = gg_bin_get_child(GG_BIN(viewport));

    if (child->height_a > viewport->height_a)
        viewport->yscroll = scroll;
}
