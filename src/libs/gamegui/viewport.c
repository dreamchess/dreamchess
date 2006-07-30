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

#include <stdlib.h>

#include <gamegui/viewport.h>
#include <gamegui/clipping.h>

static void scroll_to_focus_pos(gg_viewport_t *viewport)
{
    gg_widget_t *child = gg_bin_get_child(GG_BIN(viewport));
    int xmin = viewport->xscroll * (child->width_a - viewport->width_a);
    int xmax = xmin + viewport->width_a;
    int ymin = viewport->yscroll * (child->height_a - viewport->height_a);
    int ymax = ymin + viewport->height_a;

    gg_rect_t rect = child->get_focus_pos(child);

    if ((rect.x < xmin) || (rect.width > viewport->width_a))
        viewport->xscroll = rect.x / (float) (child->width_a - viewport->width_a);
    else if ((rect.x + rect.width) > xmax)
        viewport->xscroll = (rect.x + rect.width - viewport->width_a)
                            / (float) (child->width_a - viewport->width_a);

    if ((rect.y < ymin) || (rect.height > viewport->height_a))
        viewport->yscroll = rect.y / (float) (child->height_a - viewport->height_a);
    else if ((rect.y + rect.height) > ymax)
        viewport->yscroll = (rect.y + rect.height - viewport->height_a)
                            / (float) (child->height_a - viewport->height_a);
}

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
    int xoff = viewport->xscroll * (child->width_a - widget->width_a);
    int yoff = viewport->yscroll * (child->height_a - widget->height_a);

    rect.x = x;
    rect.y = y;
    rect.width = widget->width_a;
    rect.height = widget->height_a;

    gg_clipping_adjust(&rect);
    child->render(child, x - xoff, y - yoff, focus);
    gg_clipping_undo();
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

void gg_viewport_set_size(gg_widget_t *widget, int width, int height)
{
    gg_widget_t *child = gg_bin_get_child(GG_BIN(widget));

    if (child) {
        int child_w, child_h;

        child->get_requested_size(child, &child_w, &child_h);

        if (width > child_w)
            child_w = width;

        if (height > child_h)
            child_h = height;

        child->set_size(child, child_w, child_h);
    }

    gg_set_size(widget, width, height);
    scroll_to_focus_pos(GG_VIEWPORT(widget));
}

gg_rect_t gg_viewport_get_focus_pos(gg_widget_t *widget)
{
    gg_viewport_t *viewport = GG_VIEWPORT(widget);
    gg_widget_t *child = gg_bin_get_child(GG_BIN(widget));
    int xoff = viewport->xscroll * (child->width_a - widget->width_a);
    int yoff = viewport->yscroll * (child->height_a - widget->height_a);
    gg_rect_t rect = child->get_focus_pos(child);

    rect.x -= xoff;
    rect.y -= yoff;

    return rect;
}

int gg_viewport_set_focus_pos(gg_widget_t *widget, int x, int y)
{
    gg_viewport_t *viewport = GG_VIEWPORT(widget);
    gg_widget_t *child = gg_bin_get_child(GG_BIN(widget));
    int xoff = viewport->xscroll * (child->width_a - widget->width_a);
    int yoff = viewport->yscroll * (child->height_a - widget->height_a);

    return child->set_focus_pos(child, x + xoff, y + yoff);
}

int gg_viewport_input(gg_widget_t *widget, gg_event_t event)
{
    int retval;
    gg_viewport_t *viewport = GG_VIEWPORT(widget);
    gg_widget_t *child = gg_bin_get_child(GG_BIN(widget));
    int xmin = viewport->xscroll * (child->width_a - widget->width_a);
    int xmax = xmin + viewport->width_a;
    int ymin = viewport->yscroll * (child->height_a - widget->height_a);
    int ymax = ymin + viewport->height_a;

    if (event.type == GG_EVENT_MOUSE)
    {
        event.mouse.x += xmin;
        event.mouse.y += ymin;
    }

    retval = gg_bin_input(widget, event);

    if (event.type == GG_EVENT_KEY)
        scroll_to_focus_pos(GG_VIEWPORT(widget));

    return retval;
}

void gg_viewport_init(gg_viewport_t *viewport, gg_widget_t *widget, int width, int height)
{
    gg_bin_init((gg_bin_t *) viewport, widget);

    viewport->render = gg_viewport_render;
    viewport->set_size = gg_viewport_set_size;
    viewport->get_focus_pos = gg_viewport_get_focus_pos;
    viewport->set_focus_pos = gg_viewport_set_focus_pos;
    viewport->input = gg_viewport_input;
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
