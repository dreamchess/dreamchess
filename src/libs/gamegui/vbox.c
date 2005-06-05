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

#include <assert.h>

#include <gamegui/vbox.h>

w_class_id w_vbox_get_class_id()
{
    CHILD(w_box_get_class_id())
}

void w_vbox_render(w_widget_t *widget, int x, int y, int focus)
{
    w_box_t *box = W_BOX(widget);
    int nr = w_container_get_size(W_CONTAINER(widget));

    y += box->height_a - box->height;

    while (--nr >= 0)
    {
        int focus_child;
        w_widget_t *child = w_container_get_child(W_CONTAINER(widget), nr);

        if (focus == FOCUS_ALL)
            focus_child = FOCUS_ALL;
        else if (focus == FOCUS_ONE)
            focus_child = (box->sel == nr ? FOCUS_ONE : FOCUS_NONE);
        else
            focus_child = 0;

        child->render(child, x, y, focus_child);
        y += child->height_a;
        y += box->spacing;
    }
}

int w_vbox_input(w_widget_t *widget, ui_event_t event)
{
    w_select_t *select = W_SELECT(widget);
    w_widget_t *child;
    int retval = 0, x, y;

    if (select->sel == -1)
        return 0;

    child = w_container_get_child(W_CONTAINER(widget), select->sel);

    if (child->input(child, event))
        return 1;

    child->get_focus_pos(child, &x, &y);

    if (event == UI_EVENT_UP)
    {
        retval = w_select_prev(select, 1, 1);
        child = w_container_get_child(W_CONTAINER(widget), select->sel);
        y = 0;
    }

    if (event == UI_EVENT_DOWN)
    {
        retval = w_select_next(select, 1, 1);
        child = w_container_get_child(W_CONTAINER(widget), select->sel);
        y = child->height_a - 1;
    }

    if (retval)
    {
        child->set_focus_pos(child, x, y);
        return retval;
    }

    return 0;
}

void w_vbox_get_requested_size(w_widget_t *widget, int *width, int *height)
{
    w_container_t *container = W_CONTAINER(widget);
    w_box_t *box = W_BOX(widget);
    int size = w_container_get_size(container);
    int i;

    int nr = w_container_get_size(container);

    widget->width = 0;
    widget->height = (size - 1) * box->spacing;
    widget->enabled = 0;

    for (i = 0; i < size; i++)
    {
        int child_width, child_height;
        w_widget_t *child = w_container_get_child(container, i);

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

    w_widget_get_requested_size(widget, width, height);
}

void w_vbox_set_size(w_widget_t *widget, int width, int height)
{
    w_box_t *box = W_BOX(widget);
    int i;

    for (i = 0; i < w_container_get_size(W_CONTAINER(widget)); i++)
    {
        w_widget_t *child = w_container_get_child(W_CONTAINER(widget), i);
        int item_height;

        child->get_requested_size(child, NULL, &item_height);
        child->set_size(child, width, item_height);
    }

    w_set_size(widget, width, height);
}

void w_vbox_get_focus_pos(w_widget_t *widget, int *x , int *y)
{
    w_box_t *box = W_BOX(widget);
    w_container_t *container = W_CONTAINER(widget);
    int size = w_container_get_size(container);
    w_widget_t *child;
    int nr = w_container_get_size(container) - 1;

    assert(box->sel != -1);

    child = w_container_get_child(container, box->sel);
    child->get_focus_pos(child, x, y);

    while (nr > box->sel)
    {
        w_widget_t *sibling = w_container_get_child(container, nr);
        *y += sibling->height_a;
        nr--;
    }

    *y += (size - box->sel - 1) * box->spacing;
}

int w_vbox_set_focus_pos(w_widget_t *widget, int x , int y)
{
    w_box_t *box = W_BOX(widget);
    w_container_t *container = W_CONTAINER(widget);
    int size = w_container_get_size(container);
    int cur_y = box->height_a - box->height;
    int prev = box->sel;

    box->sel = size;

    while (w_select_prev(W_SELECT(widget), 0, 0))
    {
        w_widget_t *child = w_container_get_child(container, box->sel);

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

void w_vbox_init(w_vbox_t *vbox, int spacing)
{
    w_box_init((w_box_t *) vbox, spacing);

    vbox->render = w_vbox_render;
    vbox->input = w_vbox_input;
    vbox->get_requested_size = w_vbox_get_requested_size;
    vbox->set_size = w_vbox_set_size;
    vbox->get_focus_pos = w_vbox_get_focus_pos;
    vbox->set_focus_pos = w_vbox_set_focus_pos;
}

/** @brief Creates a vertical box widget.
 *
 *  A vertical box widget contains other widgets.
 *
 *  @return The created widget.
 */
w_widget_t *w_vbox_create(int spacing)
{
    w_vbox_t *vbox = malloc(sizeof(w_vbox_t));

    w_vbox_init(vbox, spacing);

    return W_WIDGET(vbox);
}
