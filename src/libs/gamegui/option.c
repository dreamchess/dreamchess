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

#include <gamegui/option.h>
#include <gamegui/label.h>
#include <gamegui/align.h>

static gg_colour_t col_dark_red =
    {
        0.7f, 0.0f, 0.0f, 1.0f
    };

static gg_colour_t col_light_blue =
{
    0.55f, 0.75f, 0.95f, 1.0f
};

static gg_colour_t col_black =
    {
        0.0f, 0.0f, 0.0f, 1.0f
    };

static gg_colour_t col_white =
    {
        1.0f, 1.0f, 1.0f, 1.0f
    };

static gg_colour_t col_grey =
    {
        0.5f, 0.5f, 0.5f, 1.0f
    };

static gg_colour_t col_texthighlight =
    {
        0.25f, 0.45f, 0.75f, 1.0f
    };

static gg_colour_t col_text =
    {
        1.0f, 1.0f, 1.0f, 1.0f
    };

gg_class_id gg_option_get_class_id()
{
    GG_CHILD(gg_select_get_class_id())
}

#define OPTION_ARROW_LEFT "\253 "
#define OPTION_ARROW_RIGHT " \273"

/** Implements widget::render for option widgets. */
void gg_option_render(gg_widget_t *widget, int x, int y, int focus)
{
    gg_option_t *option = GG_OPTION(widget);
    gg_widget_t *child;
    int xx, yy;
    int border_l;
    int border_r;
    int height;

    gg_system_get_string_size(OPTION_ARROW_LEFT, &border_l, &height);
    gg_system_get_string_size(OPTION_ARROW_RIGHT, &border_r, NULL);

    yy = y + option->height_a / 2 - height / 2;

    if (option->sel == -1)
        return;
    if (option->sel > 0)
    {
        if (focus != GG_FOCUS_NONE)
            gg_system_draw_string(OPTION_ARROW_LEFT, x, yy, &col_texthighlight, 1, 0);
        else
            gg_system_draw_string(OPTION_ARROW_LEFT, x, yy, &col_text, 0, 0);
    }
    else
        gg_system_draw_string(OPTION_ARROW_LEFT, x, yy, &col_grey, 0, 0);

    xx = x + border_l;

    child = gg_container_get_child(GG_CONTAINER(widget), option->sel);
    child->render(child, xx, y, focus);
    xx = x + option->width_a - border_r;

    if (option->sel < gg_container_get_size(GG_CONTAINER(widget)) - 1)
    {
        if (focus != GG_FOCUS_NONE)
            gg_system_draw_string(OPTION_ARROW_RIGHT, xx, yy, &col_texthighlight, 1, 0);
        else
            gg_system_draw_string(OPTION_ARROW_RIGHT, xx, yy, &col_text, 0, 0);
    }
    else
        gg_system_draw_string(OPTION_ARROW_RIGHT, xx, yy, &col_grey, 0, 0);
}

/** Implements widget::input for option widgets. */
int gg_option_input(gg_widget_t *widget, gg_event_t event)
{
    gg_option_t *option = GG_OPTION(widget);
    gg_select_t *select = GG_SELECT(widget);

    if (option->sel == -1)
        return 0;

    if (event.type == GG_EVENT_KEY && event.key == GG_KEY_RIGHT)
    {
        if (gg_select_next(select, 0, 0))
        {
            if (option->func)
                option->func(widget, option->func_data);
        }

        return 1;
    }
    if (event.type == GG_EVENT_KEY && event.key == GG_KEY_LEFT)
    {
        if (gg_select_prev(select, 0, 0))
        {
            if (option->func)
                option->func(widget, option->func_data);
        }

        return 1;
    }
    if (event.type == GG_EVENT_MOUSE && event.mouse.type == GG_MOUSE_BUTTON_DOWN
            && event.mouse.button == 0)
    {
        int border_l;
        int border_r;

        gg_system_get_string_size(OPTION_ARROW_LEFT, &border_l, NULL);
        gg_system_get_string_size(OPTION_ARROW_RIGHT, &border_r, NULL);

        if (event.mouse.x < border_l && gg_select_prev(select, 0, 0))
            if (option->func)
                option->func(widget, option->func_data);

        if (event.mouse.x >= widget->width_a - border_l &&
                gg_select_next(select, 0, 0))
            if (option->func)
                option->func(widget, option->func_data);

        return 1;
    }

    return 0;
}

void gg_option_set_size(gg_widget_t *widget, int width, int height)
{
    gg_container_t *container = GG_CONTAINER(widget);
    int border_l;
    int border_r;
    int i;

    gg_system_get_string_size(OPTION_ARROW_LEFT, &border_l, NULL);
    gg_system_get_string_size(OPTION_ARROW_RIGHT, &border_r, NULL);

    for (i = 0; i < gg_container_get_size(container); i++)
    {
        gg_widget_t *child = gg_container_get_child(container, i);
        child->set_size(child, width - border_l - border_r, height);
    }

    gg_set_size(widget, width, height);
}

void gg_option_init(gg_option_t *option)
{
    gg_select_init((gg_select_t *) option);

    option->render = gg_option_render;
    option->input = gg_option_input;
    option->set_size = gg_option_set_size;
    option->id = gg_option_get_class_id();
}

/** @brief Creates an option widget.
 *
 *  An option widget consists of a label and a set of options. The label is
 *  rendered to the left of the currently selected option. The input handler
 *  allows for cycling through the available options.
 *
 *  @return The created widget.
 */
gg_widget_t *gg_option_create()
{
    gg_option_t *option = malloc(sizeof(gg_option_t));

    gg_option_init(option);

    return GG_WIDGET(option);
}

/** @brief Appends an option to the option widget's option list.
 *
 *  @param widget The option widget.
 *  @param string The option to append.
 */
void gg_option_append(gg_option_t *option, gg_widget_t *child)
{
    int width, child_height;
    int height;
    int border_l;
    int border_r;

    child->get_requested_size(child, &width, &child_height);

    gg_system_get_string_size(OPTION_ARROW_LEFT, &border_l, &height);
    gg_system_get_string_size(OPTION_ARROW_RIGHT, &border_r, NULL);

    width += border_l + border_r;

    if (child_height > height)
        height = child_height;

    gg_container_append(GG_CONTAINER(option), child);

    if (width > option->width)
        option->width = width;

    if (height > option->height)
        option->height = height;

    if (gg_container_get_size(GG_CONTAINER(option)) == 2)
        option->enabled = 1;

    if (option->sel == -1)
        option->sel = 0;
}

void gg_option_append_label(gg_option_t *option, char *text, float xalign, float yalign)
{
    gg_widget_t *label = gg_label_create(text);
    gg_align_set_alignment(GG_ALIGN(label), xalign, yalign);
    gg_option_append(option, label);
}

/** @brief Returns the index of the selected option of an option widget.
 *
 *  @param widget The option widget.
 *  @return Index of the selected option.
 */
int gg_option_get_selected(gg_option_t *option)
{
    return option->sel;
}

void gg_option_set_selected(gg_option_t *option, int index)
{
    option->sel=index;
}

/** @brief Sets option widget callback.
 *
 *  @param widget The option widget.
 *  @param callback Function that should be called when an option is
 *                  selected.
 */
void gg_option_set_callback(gg_option_t *option, void (* callback) (gg_widget_t *, void *), void *func_data)
{
    option->func = callback;
    option->func_data = func_data;
}
