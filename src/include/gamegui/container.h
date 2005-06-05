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

#define W_CONTAINER(W) CHECK_CAST(W, w_container_get_class_id(), w_container_t)

#define W_CONTAINER_DATA \
    W_WIDGET_DATA \
    list_t *widget_list;

typedef struct list
{
    /** Total number of items in the list. */
    int items;

    /** The items in the list. */
    void **item;
}
list_t;

typedef struct w_container
{
    W_CONTAINER_DATA
}
w_container_t;

w_class_id w_container_get_class_id();

void w_container_destroy(w_widget_t *widget);

void w_container_init(w_container_t *container);

void w_container_append(w_container_t *container, w_widget_t *widget);

int w_container_get_size(w_container_t *container);

w_widget_t *w_container_get_child(w_container_t *container, int index);

#endif /* GAMEGUI_CONTAINER_H */
