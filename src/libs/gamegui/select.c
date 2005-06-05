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

#include <gamegui/select.h>

w_class_id w_select_get_class_id()
{
    CHILD(w_container_get_class_id())
}

void w_select_init(w_select_t *select)
{
    w_container_init((w_container_t *) select);

    select->id = w_select_get_class_id();
    select->sel = -1;
}

int w_select_prev(w_select_t *select, int input, int enabled)
{
    int sel = select->sel - 1;

    while (sel >= 0)
    {
        w_widget_t *child = w_container_get_child(W_CONTAINER(select), sel);

        if ((enabled && !child->enabled)
                || (input && !child->input))
            sel--;
        else
            break;
    }

    if (sel >= 0)
    {
        select->sel = sel;
        return 1;
    }

    return 0;
}

int w_select_next(w_select_t *select, int input, int enabled)
{
    int sel = select->sel + 1;
    int size = w_container_get_size(W_CONTAINER(select));

    while (sel < size)
    {
        w_widget_t *child = w_container_get_child(W_CONTAINER(select), sel);

        if ((enabled && !child->enabled)
                || (input && !child->input))
            sel++;
        else
            break;
    }

    if (sel < size)
    {
        select->sel = sel;
        return 1;
    }

    return 0;
}
