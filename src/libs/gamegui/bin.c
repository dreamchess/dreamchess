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

#include <gamegui/bin.h>

gg_class_id gg_bin_get_class_id()
{
    GG_CHILD(gg_container_get_class_id())
}

gg_widget_t *gg_bin_get_child(gg_bin_t *bin)
{
    return gg_container_get_child(GG_CONTAINER(bin), 0);
}

void gg_bin_set_size(gg_widget_t *widget, int width, int height)
{
    gg_widget_t *child = gg_bin_get_child(GG_BIN(widget));

    if (child)
        child->set_size(child, width, height);

    gg_set_size(widget, width, height);
}

int gg_bin_set_focus_pos(gg_widget_t *widget, int x, int y)
{
    gg_widget_t *child = gg_bin_get_child(GG_BIN(widget));

    if (child)
        return child->set_focus_pos(child, x, y);

    return 0;
}

int gg_bin_input(gg_widget_t *widget, gg_event_t event)
{
    gg_widget_t *child = gg_bin_get_child(GG_BIN(widget));

    if (child)
        return child->input(child, event);

    return 0;
}

void gg_bin_init(gg_bin_t *bin, gg_widget_t *child)
{
    gg_container_init((gg_container_t *) bin);

    bin->set_size = gg_bin_set_size;
    bin->set_focus_pos = gg_bin_set_focus_pos;
    bin->input = gg_bin_input;
    bin->id = gg_bin_get_class_id();
    bin->enabled = child->enabled;
    gg_container_append(GG_CONTAINER(bin), child);
}
