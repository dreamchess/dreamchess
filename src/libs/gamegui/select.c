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

gg_class_id gg_select_get_class_id()
{
    GG_CHILD(gg_container_get_class_id())
}

void gg_select_init(gg_select_t *select)
{
    gg_container_init((gg_container_t *) select);

    select->id = gg_select_get_class_id();
    select->sel = -1;
}

int gg_select_prev(gg_select_t *select, int input, int enabled)
{
    int sel = select->sel - 1;

    while (sel >= 0)
    {
        gg_widget_t *child = gg_container_get_child(GG_CONTAINER(select), sel);

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

int gg_select_next(gg_select_t *select, int input, int enabled)
{
    int sel = select->sel + 1;
    int size = gg_container_get_size(GG_CONTAINER(select));

    while (sel < size)
    {
        gg_widget_t *child = gg_container_get_child(GG_CONTAINER(select), sel);

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
