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

w_class_id w_bin_get_class_id()
{
    CHILD(w_container_get_class_id())
}

w_widget_t *w_bin_get_child(w_bin_t *bin)
{
    return w_container_get_child(W_CONTAINER(bin), 0);
}

void w_bin_set_size(w_widget_t *widget, int width, int height)
{
    w_widget_t *child = w_bin_get_child(W_BIN(widget));

    if (child)
        child->set_size(child, width, height);

    w_set_size(widget, width, height);
}

int w_bin_set_focus_pos(w_widget_t *widget, int x, int y)
{
    w_widget_t *child = w_bin_get_child(W_BIN(widget));

    if (child)
        return child->set_focus_pos(child, x, y);

    return 0;
}

void w_bin_init(w_bin_t *bin, w_widget_t *child)
{
    w_container_init((w_container_t *) bin);

    bin->set_size = w_bin_set_size;
    bin->set_focus_pos = w_bin_set_focus_pos;
    bin->id = w_bin_get_class_id();
    w_container_append(W_CONTAINER(bin), child);
}
