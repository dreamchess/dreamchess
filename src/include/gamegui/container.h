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

#ifndef GAMEGUI_CONTAINER_H
#define GAMEGUI_CONTAINER_H

#include <gamegui/system.h>
#include <gamegui/widget.h>

#define GG_CONTAINER(W) GG_CHECK_CAST(W, gg_container_get_class_id(), gg_container_t)

#define GG_CONTAINER_DATA \
    GG_WIDGET_DATA \
    list_t *widget_list;

typedef struct list
{
    /** Total number of items in the list. */
    int items;

    /** The items in the list. */
    void **item;
}
list_t;

typedef struct gg_container
{
    GG_CONTAINER_DATA
}
gg_container_t;

gg_class_id gg_container_get_class_id();

void gg_container_destroy(gg_widget_t *widget);

void gg_container_init(gg_container_t *container);

void gg_container_append(gg_container_t *container, gg_widget_t *widget);

int gg_container_get_size(gg_container_t *container);

gg_widget_t *gg_container_get_child(gg_container_t *container, int index);

#endif /* GAMEGUI_CONTAINER_H */
