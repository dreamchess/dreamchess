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

#include <gamegui/container.h>

static list_t *list_create()
{
    list_t *list = malloc(sizeof(list_t));

    list->items = 0;
    list->item = NULL;

    return list;
}

static void list_append_item(list_t *list, void *item)
{
    list->item = realloc(list->item, (list->items + 1) * sizeof(void *));
    list->item[list->items++] = item;
}

static int list_get_size(list_t *list)
{
    return list->items;
}

static void *list_get_item(list_t *list, int index)
{
    if (index >= list->items)
        return NULL;

    return list->item[index];
}

static void list_destroy(list_t *list)
{
    free(list);
}

gg_class_id gg_container_get_class_id()
{
    GG_CHILD(gg_widget_get_class_id())
}

void gg_container_destroy(gg_widget_t *widget)
{
    gg_container_t *container = GG_CONTAINER(widget);
    int i;

    for (i = 0; i < list_get_size(container->widget_list); i++)
    {
        gg_widget_t *item = GG_WIDGET(list_get_item(container->widget_list, i));
        item->destroy(item);
    }

    list_destroy(container->widget_list);
    gg_widget_destroy(widget);
}

void gg_container_init(gg_container_t *container)
{
    gg_widget_init((gg_widget_t *) container);

    container->destroy = gg_container_destroy;
    container->id = gg_container_get_class_id();
    container->widget_list = list_create();
}

void gg_container_append(gg_container_t *container, gg_widget_t *widget)
{
    list_append_item(container->widget_list, widget);
    widget->parent = GG_WIDGET(container);
}

int gg_container_get_size(gg_container_t *container)
{
    return list_get_size(container->widget_list);
}

gg_widget_t *gg_container_get_child(gg_container_t *container, int index)
{
    return list_get_item(container->widget_list, index);
}
