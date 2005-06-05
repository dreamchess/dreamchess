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

#include <gamegui/hbox.h>

w_class_id w_hbox_get_class_id()
{
    CHILD(w_box_get_class_id())
}

void w_hbox_render(w_widget_t *widget, int x, int y, int focus)
{
    w_box_t *box = W_BOX(widget);
    int nr = 0;

    while (nr < w_container_get_size(W_CONTAINER(widget)))
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
        x += child->width_a;
        x += box->spacing;
        nr++;
    }
}

int w_hbox_input(w_widget_t *widget, ui_event_t event)
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

    if (event == UI_EVENT_LEFT)
    {
        retval = w_select_prev(select, 1, 1);
        child = w_container_get_child(W_CONTAINER(widget), select->sel);
        x = child->width_a - 1;
    }

    if (event == UI_EVENT_RIGHT)
    {
        retval = w_select_next(select, 1, 1);
        child = w_container_get_child(W_CONTAINER(widget), select->sel);
        x = 0;
    }

    if (retval)
    {
        child->set_focus_pos(child, x, y);
        return retval;
    }

    return 0;
}

void w_hbox_get_requested_size(w_widget_t *widget, int *width, int *height)
{
    w_container_t *container = W_CONTAINER(widget);
    w_box_t *box = W_BOX(widget);
    int size = w_container_get_size(container);
    int i;

    widget->width = (size - 1) * box->spacing;
    widget->height = 0;
    widget->enabled = 0;

    for (i = 0; i < size; i++)
    {
        int child_width, child_height;
        w_widget_t *child = w_container_get_child(container, i);

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

    w_widget_get_requested_size(widget, width, height);
}

void w_hbox_set_size(w_widget_t *widget, int width, int height)
{
    w_box_t *box = W_BOX(widget);
    int i;

    for (i = 0; i < w_container_get_size(W_CONTAINER(widget)); i++)
    {
        w_widget_t *child = w_container_get_child(W_CONTAINER(widget), i);
        int item_width;

        child->get_requested_size(child, &item_width, NULL);
        child->set_size(child, item_width, height);
    }

    w_set_size(widget, width, height);
}

void w_hbox_get_focus_pos(w_widget_t *widget, int *x , int *y)
{
    w_box_t *box = W_BOX(widget);
    w_container_t *container = W_CONTAINER(widget);
    w_widget_t *child;
    int nr = 0;

    assert(box->sel != -1);

    child = w_container_get_child(container, box->sel);
    child->get_focus_pos(child, x, y);

    while (nr < box->sel)
    {
        w_widget_t *sibling = w_container_get_child(container, nr);

        *x += child->width_a;
        nr++;
    }

    *x += box->sel * box->spacing;
}

int w_hbox_set_focus_pos(w_widget_t *widget, int x , int y)
{
    w_box_t *box = W_BOX(widget);
    w_container_t *container = W_CONTAINER(widget);
    int cur_x = 0;
    int prev = box->sel;

    box->sel = -1;

    while (w_select_next(W_SELECT(widget), 0, 0))
    {
        w_widget_t *child = w_container_get_child(container, box->sel);

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

void w_hbox_init(w_hbox_t *hbox, int spacing)
{
    w_box_init((w_box_t *) hbox, spacing);

    hbox->render = w_hbox_render;
    hbox->input = w_hbox_input;
    hbox->get_requested_size = w_hbox_get_requested_size;
    hbox->set_size = w_hbox_set_size;
    hbox->get_focus_pos = w_hbox_get_focus_pos;
    hbox->set_focus_pos = w_hbox_set_focus_pos;
}

/** @brief Creates a horizontal box widget.
 *
 *  A horizontal box widget contains other widgets.
 *
 *  @return The created widget.
 */
w_widget_t *w_hbox_create(int spacing)
{
    w_hbox_t *hbox = malloc(sizeof(w_hbox_t));

    w_hbox_init(hbox, spacing);

    return W_WIDGET(hbox);
}
