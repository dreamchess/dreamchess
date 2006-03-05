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
#include <assert.h>

#include <gamegui/hbox.h>

gg_class_id gg_hbox_get_class_id()
{
    GG_CHILD(gg_box_get_class_id())
}

void gg_hbox_render(gg_widget_t *widget, int x, int y, int focus)
{
    gg_box_t *box = GG_BOX(widget);
    int nr = 0;

    while (nr < gg_container_get_size(GG_CONTAINER(widget)))
    {
        int focus_child;
        gg_widget_t *child = gg_container_get_child(GG_CONTAINER(widget), nr);

        if (focus == GG_FOCUS_ALL)
            focus_child = GG_FOCUS_ALL;
        else if (focus == GG_FOCUS_ONE)
            focus_child = (box->sel == nr ? GG_FOCUS_ONE : GG_FOCUS_NONE);
        else
            focus_child = 0;

        child->render(child, x, y, focus_child);
        x += child->width_a;
        x += box->spacing;
        nr++;
    }
}

int gg_hbox_input(gg_widget_t *widget, gg_event_t event)
{
    gg_select_t *select = GG_SELECT(widget);
    gg_box_t *box = GG_BOX(widget);
    gg_widget_t *child;
    gg_rect_t focus;
    int retval = 0, x, y;

    if (select->sel == -1)
        return 0;

    if (event.type == GG_EVENT_MOUSE)
    {
        int nr = 0;
        while (nr < select->sel)
        {
            gg_widget_t *child = gg_container_get_child(GG_CONTAINER(widget), nr);

            event.mouse.x -= child->width_a;
            event.mouse.x -= box->spacing;
            nr++;
        }
    }

    child = gg_container_get_child(GG_CONTAINER(widget), select->sel);

    if (event.type == GG_EVENT_MOUSE)
    {
        if (event.mouse.x < 0 || event.mouse.x >= child->width_a ||
            event.mouse.y < 0 || event.mouse.y >= child->height_a)
            return 0;
    }

    if (child->input(child, event))
        return 1;

    focus = child->get_focus_pos(child);
    y = focus.y + focus.height / 2;

    if (event.type == GG_EVENT_KEY && event.key == GG_KEY_LEFT)
    {
        retval = gg_select_prev(select, 1, 1);
        child = gg_container_get_child(GG_CONTAINER(widget), select->sel);
        x = child->width_a - 1;
    }

    if (event.type == GG_EVENT_KEY && event.key == GG_KEY_RIGHT)
    {
        retval = gg_select_next(select, 1, 1);
        child = gg_container_get_child(GG_CONTAINER(widget), select->sel);
        x = 0;
    }

    if (retval)
    {
        child->set_focus_pos(child, x, y);
        return retval;
    }

    return 0;
}

void gg_hbox_get_requested_size(gg_widget_t *widget, int *width, int *height)
{
    gg_container_t *container = GG_CONTAINER(widget);
    gg_box_t *box = GG_BOX(widget);
    int size = gg_container_get_size(container);
    int i;

    widget->width = (size - 1) * box->spacing;
    widget->height = 0;
    widget->enabled = 0;

    for (i = 0; i < size; i++)
    {
        int child_width, child_height;
        gg_widget_t *child = gg_container_get_child(container, i);

        child->get_requested_size(child, &child_width, &child_height);

        if (child_height > widget->height)
            widget->height = child_height;

        widget->width += child_width;

        if (child->enabled && child->input)
        {
            if (box->sel == -1)
                box->sel = i;
            widget->enabled = 1;
        }
    }

    gg_widget_get_requested_size(widget, width, height);
}

void gg_hbox_set_size(gg_widget_t *widget, int width, int height)
{
    int i;

    for (i = 0; i < gg_container_get_size(GG_CONTAINER(widget)); i++)
    {
        gg_widget_t *child = gg_container_get_child(GG_CONTAINER(widget), i);
        int item_width;

        child->get_requested_size(child, &item_width, NULL);
        child->set_size(child, item_width, height);
    }

    gg_set_size(widget, width, height);
}

gg_rect_t gg_hbox_get_focus_pos(gg_widget_t *widget)
{
    gg_box_t *box = GG_BOX(widget);
    gg_container_t *container = GG_CONTAINER(widget);
    gg_widget_t *child;
    gg_rect_t focus;
    int nr = 0;

    assert(box->sel != -1);

    child = gg_container_get_child(container, box->sel);
    focus = child->get_focus_pos(child);

    while (nr < box->sel)
    {
        focus.x += child->width_a;
        nr++;
    }

    focus.x += box->sel * box->spacing;

    return focus;
}

int gg_hbox_set_focus_pos(gg_widget_t *widget, int x , int y)
{
    gg_box_t *box = GG_BOX(widget);
    gg_container_t *container = GG_CONTAINER(widget);
    int cur_x = 0;
    int prev = box->sel;

    box->sel = -1;

    while (gg_select_next(GG_SELECT(widget), 0, 0))
    {
        gg_widget_t *child = gg_container_get_child(container, box->sel);

        cur_x += child->width_a;
        if (cur_x >= x)
        {
            if (!child->input || !child->enabled ||
                    (!child->set_focus_pos(child, x - cur_x + child->width_a, y)))
                break;
            else
                return 1;
        }
        cur_x += box->spacing;
    }

    box->sel = prev;
    return 0;
}

void gg_hbox_init(gg_hbox_t *hbox, int spacing)
{
    gg_box_init((gg_box_t *) hbox, spacing);

    hbox->id = gg_hbox_get_class_id();
    hbox->render = gg_hbox_render;
    hbox->input = gg_hbox_input;
    hbox->get_requested_size = gg_hbox_get_requested_size;
    hbox->set_size = gg_hbox_set_size;
    hbox->get_focus_pos = gg_hbox_get_focus_pos;
    hbox->set_focus_pos = gg_hbox_set_focus_pos;
}

/** @brief Creates a horizontal box widget.
 *
 *  A horizontal box widget contains other widgets.
 *
 *  @return The created widget.
 */
gg_widget_t *gg_hbox_create(int spacing)
{
    gg_hbox_t *hbox = malloc(sizeof(gg_hbox_t));

    gg_hbox_init(hbox, spacing);

    return GG_WIDGET(hbox);
}
