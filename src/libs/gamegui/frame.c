/*  DreamChess
 *  Copyright (C) 2006  The DreamChess project
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

#include <gamegui/frame.h>

/** Spacing on each side of the rectangle. */
#define GG_FRAME_SPACING 10

/** Line width. Needs to be 1 at this time. */
#define GG_FRAME_SIZE 1

static gg_colour_t col_black =
    {
        0.0f, 0.0f, 0.0f, 1.0f
    };

gg_class_id gg_frame_get_class_id()
{
    GG_CHILD(gg_bin_get_class_id())
}

void gg_frame_render(gg_widget_t *widget, int x, int y, int focus)
{
    gg_frame_t *frame = GG_FRAME(widget);
    int width = frame->width_a - 2 * GG_FRAME_SPACING;
    int height = frame->height_a - 2 * GG_FRAME_SPACING;
    gg_widget_t *child = gg_bin_get_child(GG_BIN(widget));

    if (height <= 0 || width <= 0)
        return;

    gg_system_draw_rect(x + GG_FRAME_SPACING, y + GG_FRAME_SPACING, width,
                        height, &col_black);

    child->render(child, x + GG_FRAME_SPACING * 2 + GG_FRAME_SIZE,
                  y - GG_FRAME_SPACING * 2 - GG_FRAME_SIZE, focus);
}

gg_rect_t gg_frame_get_focus_pos(gg_widget_t *widget)
{
    gg_rect_t rect;
    gg_widget_t *child = gg_bin_get_child(GG_BIN(widget));
    rect = child->get_focus_pos(child);
    rect.x -= 2 * GG_FRAME_SPACING + GG_FRAME_SIZE;
    rect.y += 2 * GG_FRAME_SPACING + GG_FRAME_SIZE;
    return rect;
}

int gg_frame_set_focus_pos(gg_widget_t *widget, int x, int y)
{
    gg_widget_t *child = gg_bin_get_child(GG_BIN(widget));
    return child->set_focus_pos(child,
                                x - 2 * GG_FRAME_SPACING - GG_FRAME_SIZE,
                                y + 2 * GG_FRAME_SPACING + GG_FRAME_SIZE);
}

int gg_frame_input(gg_widget_t *widget, gg_event_t event)
{
    if (event.type == GG_EVENT_MOUSE)
    {
        event.mouse.x -= GG_FRAME_SPACING * 2 + GG_FRAME_SIZE;
        event.mouse.y += GG_FRAME_SPACING * 2 + GG_FRAME_SIZE;
    }
    return gg_bin_input(widget, event);
}

void gg_frame_init(gg_frame_t *frame, gg_widget_t *child)
{
    int width, height;
    child->get_requested_size(child, &width, &height);
    gg_bin_init((gg_bin_t *) frame, child);
    frame->width = width;
    frame->height = height;
    frame->render = gg_frame_render;
    frame->get_focus_pos = gg_frame_get_focus_pos;
    frame->set_focus_pos = gg_frame_set_focus_pos;
    frame->input = gg_frame_input;
    frame->id = gg_frame_get_class_id();
    frame->width += GG_FRAME_SPACING * 4 + GG_FRAME_SIZE * 2;
    frame->height += GG_FRAME_SPACING * 4 + GG_FRAME_SIZE * 2;
}

gg_widget_t *gg_frame_create(gg_widget_t *child)
{
    gg_frame_t *frame = malloc(sizeof(gg_frame_t));

    gg_frame_init(frame, child);

    return GG_WIDGET(frame);
}
