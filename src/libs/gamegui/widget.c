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

/** @file
 *  @brief Widget class implementation.
 */

#include <stdlib.h>

#include <gamegui/system.h>
#include <gamegui/widget.h>

gg_class_id gg_widget_get_class_id()
{
    GG_CHILD(GG_CLASS_ID_NONE)
}

void gg_widget_destroy(gg_widget_t *widget)
{
    free(widget);
}

void gg_set_requested_size(gg_widget_t *widget, int width, int height)
{
    widget->width_f = width;
    widget->height_f = height;
}

void gg_widget_get_requested_size(gg_widget_t *widget, int *width,
                                  int *height)
{
    if (width)
    {
        if (widget->width_f > widget->width)
            *width = widget->width_f;
        else
            *width = widget->width;
    }

    if (height)
    {
        if (widget->height_f > widget->height)
            *height = widget->height_f;
        else
            *height = widget->height;
    }
}

void gg_set_size(gg_widget_t *widget, int width, int height)
{
    widget->width_a = width;
    widget->height_a = height;
}

gg_rect_t gg_get_focus_pos(gg_widget_t *widget)
{
    gg_rect_t rect;

    rect.x = 0;
    rect.y = 0;
    rect.width = widget->width_a;
    rect.height = widget->height_a;

    return rect;
}

int gg_set_focus_pos(gg_widget_t *widget, int x, int y)
{
    return 1;
}

void gg_widget_init(gg_widget_t *widget)
{
    widget->render = NULL;
    widget->input = NULL;
    widget->destroy = gg_widget_destroy;
    widget->get_requested_size = gg_widget_get_requested_size;
    widget->set_size = gg_set_size;
    widget->get_focus_pos = gg_get_focus_pos;
    widget->set_focus_pos = gg_set_focus_pos;
    widget->id = gg_widget_get_class_id();
    widget->enabled = 0;
    widget->width = widget->height = 0;
    widget->width_f = widget->height_f = -1;
    widget->width_a = widget->height_a = 0;
    widget->parent = NULL;
}
