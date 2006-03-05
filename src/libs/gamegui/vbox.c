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

#include <gamegui/vbox.h>

gg_class_id gg_vbox_get_class_id()
{
    GG_CHILD(gg_box_get_class_id())
}

void gg_vbox_set_selected(gg_widget_t *widget, int index )
{
    gg_select_t *select = GG_SELECT(widget);
    select->sel=index;
}

void gg_vbox_render(gg_widget_t *widget, int x, int y, int focus)
{
    gg_box_t *box = GG_BOX(widget);
    int nr = gg_container_get_size(GG_CONTAINER(widget));

    y += box->height_a - box->height;

    while (--nr >= 0)
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
        y += child->height_a;
        y += box->spacing;
    }
}

int gg_vbox_input(gg_widget_t *widget, gg_event_t event)
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
        int nr = gg_container_get_size(GG_CONTAINER(widget));

        event.mouse.y -= box->height_a - box->height;
        while (--nr > select->sel)
        {
            gg_widget_t *child = gg_container_get_child(GG_CONTAINER(widget), nr);

            event.mouse.y -= child->height_a;
            event.mouse.y -= box->spacing;
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
    x = focus.x + focus.width / 2;

    if (event.type == GG_EVENT_KEY && event.key == GG_KEY_UP)
    {
        retval = gg_select_prev(select, 1, 1);
        child = gg_container_get_child(GG_CONTAINER(widget), select->sel);
        y = 0;
    }

    if (event.type == GG_EVENT_KEY && event.key == GG_KEY_DOWN)
    {
        retval = gg_select_next(select, 1, 1);
        child = gg_container_get_child(GG_CONTAINER(widget), select->sel);
        y = child->height_a - 1;
    }

    if (retval)
    {
        child->set_focus_pos(child, x, y);
        return retval;
    }

    return 0;
}

void gg_vbox_get_requested_size(gg_widget_t *widget, int *width, int *height)
{
    gg_container_t *container = GG_CONTAINER(widget);
    gg_box_t *box = GG_BOX(widget);
    int size = gg_container_get_size(container);
    int i;

    widget->width = 0;
    widget->height = (size - 1) * box->spacing;
    widget->enabled = 0;

    for (i = 0; i < size; i++)
    {
        int child_width, child_height;
        gg_widget_t *child = gg_container_get_child(container, i);

        child->get_requested_size(child, &child_width, &child_height);

        if (child_width > widget->width)
            widget->width = child_width;

        widget->height += child_height;

        if (child->enabled && child->input)
        {
            if (box->sel == -1)
                box->sel = i;
            widget->enabled = 1;
        }
    }

    gg_widget_get_requested_size(widget, width, height);
}

void gg_vbox_set_size(gg_widget_t *widget, int width, int height)
{
    int i;

    for (i = 0; i < gg_container_get_size(GG_CONTAINER(widget)); i++)
    {
        gg_widget_t *child = gg_container_get_child(GG_CONTAINER(widget), i);
        int item_height;

        child->get_requested_size(child, NULL, &item_height);
        child->set_size(child, width, item_height);
    }

    gg_set_size(widget, width, height);
}

gg_rect_t gg_vbox_get_focus_pos(gg_widget_t *widget)
{
    gg_box_t *box = GG_BOX(widget);
    gg_container_t *container = GG_CONTAINER(widget);
    int size = gg_container_get_size(container);
    gg_widget_t *child;
    gg_rect_t focus;
    int nr = gg_container_get_size(container) - 1;

    assert(box->sel != -1);

    child = gg_container_get_child(container, box->sel);
    focus = child->get_focus_pos(child);

    while (nr > box->sel)
    {
        gg_widget_t *sibling = gg_container_get_child(container, nr);
        focus.y += sibling->height_a;
        nr--;
    }

    focus.y += (size - box->sel - 1) * box->spacing;

    return focus;
}

int gg_vbox_set_focus_pos(gg_widget_t *widget, int x , int y)
{
    gg_box_t *box = GG_BOX(widget);
    gg_container_t *container = GG_CONTAINER(widget);
    int size = gg_container_get_size(container);
    int cur_y = box->height_a - box->height;
    int prev = box->sel;

    box->sel = size;

    while (gg_select_prev(GG_SELECT(widget), 0, 0))
    {
        gg_widget_t *child = gg_container_get_child(container, box->sel);

        cur_y += child->height_a;
        if (cur_y >= y)
        {
            if (!child->input || !child->enabled || !
                child->set_focus_pos(child, x, child->height_a - (cur_y - y)))
                break;
            else
                return 1;
        }
        cur_y += box->spacing;
    }

    box->sel = prev;
    return 0;
}

void gg_vbox_init(gg_vbox_t *vbox, int spacing)
{
    gg_box_init((gg_box_t *) vbox, spacing);

    vbox->id = gg_vbox_get_class_id();
    vbox->render = gg_vbox_render;
    vbox->input = gg_vbox_input;
    vbox->get_requested_size = gg_vbox_get_requested_size;
    vbox->set_size = gg_vbox_set_size;
    vbox->get_focus_pos = gg_vbox_get_focus_pos;
    vbox->set_focus_pos = gg_vbox_set_focus_pos;
}

/** @brief Creates a vertical box widget.
 *
 *  A vertical box widget contains other widgets.
 *
 *  @return The created widget.
 */
gg_widget_t *gg_vbox_create(int spacing)
{
    gg_vbox_t *vbox = malloc(sizeof(gg_vbox_t));

    gg_vbox_init(vbox, spacing);

    return GG_WIDGET(vbox);
}
