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

#include <gamegui/option.h>
#include <gamegui/align.h>

static w_colour_t col_dark_red =
    {
        0.7f, 0.0f, 0.0f, 1.0f
    };

static w_colour_t col_black =
    {
        0.0f, 0.0f, 0.0f, 1.0f
    };

static w_colour_t col_grey =
    {
        0.5f, 0.5f, 0.5f, 1.0f
    };

w_class_id w_option_get_class_id()
{
    CHILD(w_select_get_class_id())
}

#define OPTION_ARROW_LEFT "\253 "
#define OPTION_ARROW_RIGHT " \273"

/** Implements widget::render for option widgets. */
void w_option_render(w_widget_t *widget, int x, int y, int focus)
{
    w_option_t *option = W_OPTION(widget);
    w_widget_t *child;
    int xx, yy;
    int border_l;
    int border_r;
    int height;

    w_system_get_string_size(OPTION_ARROW_LEFT, &border_l, &height);
    w_system_get_string_size(OPTION_ARROW_RIGHT, &border_r, NULL);

    yy = y + option->height_a / 2 - height / 2;

    if (option->sel == -1)
        return;
    if (option->sel > 0)
    {
        if (focus != FOCUS_NONE)
            w_system_draw_string(OPTION_ARROW_LEFT, x, yy, &col_dark_red, 1, 0);
        else
            w_system_draw_string(OPTION_ARROW_LEFT, x, yy, &col_black, 0, 0);
    }
    else
        w_system_draw_string(OPTION_ARROW_LEFT, x, yy, &col_grey, 0, 0);

    xx = x + border_l;

    child = w_container_get_child(W_CONTAINER(widget), option->sel);
    child->render(child, xx, y, focus);
    xx = x + option->width_a - border_r;

    if (option->sel < w_container_get_size(W_CONTAINER(widget)) - 1)
    {
        if (focus != FOCUS_NONE)
            w_system_draw_string(OPTION_ARROW_RIGHT, xx, yy, &col_dark_red, 1, 0);
        else
            w_system_draw_string(OPTION_ARROW_RIGHT, xx, yy, &col_black, 0, 0);
    }
    else
        w_system_draw_string(OPTION_ARROW_RIGHT, xx, yy, &col_grey, 0, 0);
}

/** Implements widget::input for option widgets. */
int w_option_input(w_widget_t *widget, ui_event_t event)
{
    w_option_t *option = W_OPTION(widget);
    w_select_t *select = W_SELECT(widget);

    if (option->sel == -1)
        return 0;

    if (event == UI_EVENT_RIGHT)
    {
        if (w_select_next(select, 0, 0))
        {
            if (option->func)
                option->func(widget, option->func_data);
        }

        return 1;
    }
    if (event == UI_EVENT_LEFT)
    {
        if (w_select_prev(select, 0, 0))
        {
            if (option->func)
                option->func(widget, option->func_data);
        }

        return 1;
    }

    return 0;
}

void w_option_set_size(w_widget_t *widget, int width, int height)
{
    w_option_t *option = W_OPTION(widget);
    w_container_t *container = W_CONTAINER(widget);
    int border_l;
    int border_r;
    int i;

    w_system_get_string_size(OPTION_ARROW_LEFT, &border_l, NULL);
    w_system_get_string_size(OPTION_ARROW_RIGHT, &border_r, NULL);

    for (i = 0; i < w_container_get_size(container); i++)
    {
        w_widget_t *child = w_container_get_child(container, i);
        child->set_size(child, width - border_l - border_r, height);
    }

    w_set_size(widget, width, height);
}

void w_option_init(w_option_t *option)
{
    w_select_init((w_select_t *) option);

    option->render = w_option_render;
    option->input = w_option_input;
    option->set_size = w_option_set_size;
    option->id = w_option_get_class_id();
}

/** @brief Creates an option widget.
 *
 *  An option widget consists of a label and a set of options. The label is
 *  rendered to the left of the currently selected option. The input handler
 *  allows for cycling through the available options.
 *
 *  @return The created widget.
 */
w_widget_t *w_option_create()
{
    w_option_t *option = malloc(sizeof(w_option_t));

    w_option_init(option);

    return W_WIDGET(option);
}

/** @brief Appends an option to the option widget's option list.
 *
 *  @param widget The option widget.
 *  @param string The option to append.
 */
void w_option_append(w_option_t *option, w_widget_t *child)
{
    int width, child_height;
    int height;
    int border_l;
    int border_r;

    child->get_requested_size(child, &width, &child_height);

    w_system_get_string_size(OPTION_ARROW_LEFT, &border_l, &height);
    w_system_get_string_size(OPTION_ARROW_RIGHT, &border_r, NULL);

    width += border_l + border_r;

    if (child_height > height)
        height = child_height;

    w_container_append(W_CONTAINER(option), child);

    if (width > option->width)
        option->width = width;

    if (height > option->height)
        option->height = height;

    if (w_container_get_size(W_CONTAINER(option)) == 2)
        option->enabled = 1;

    if (option->sel == -1)
        option->sel = 0;
}

void w_option_append_label(w_option_t *option, char *text, float xalign, float yalign)
{
    w_widget_t *label = w_label_create(text);
    w_align_set_alignment(W_ALIGN(label), xalign, yalign);
    w_option_append(option, label);
}

/** @brief Returns the index of the selected option of an option widget.
 *
 *  @param widget The option widget.
 *  @return Index of the selected option.
 */
int w_option_get_selected(w_option_t *option)
{
    return option->sel;
}

/** @brief Sets option widget callback.
 *
 *  @param widget The option widget.
 *  @param callback Function that should be called when an option is
 *                  selected.
 */
void w_option_set_callback(w_option_t *option, void (* callback) (w_widget_t *, void *), void *func_data)
{
    option->func = callback;
    option->func_data = func_data;
}
