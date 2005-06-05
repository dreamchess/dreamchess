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

#include <gamegui/label.h>

static w_colour_t col_dark_red =
    {
        0.7f, 0.0f, 0.0f, 1.0f
    };

static w_colour_t col_black =
    {
        0.0f, 0.0f, 0.0f, 1.0f
    };

w_class_id w_label_get_class_id()
{
    CHILD(w_align_get_class_id())
}

/** Implements widget::render for text widgets. */
void w_label_render(w_widget_t *widget, int x, int y, int focus)
{
    w_label_t *label = W_LABEL(widget);

    x += label->xalign * (label->width_a - label->width);
    y += (1.0f - label->yalign) * (label->height_a - label->height);

    if (focus != FOCUS_NONE)
        w_system_draw_string(label->label, x, y, &col_dark_red, label->bouncy, 0);
    else
        w_system_draw_string(label->label, x, y, &col_black, 0, 0);
}

void w_label_set_bouncy(w_label_t *label, int bouncy)
{
    label->bouncy = bouncy;
}

/** @brief Destroys a text widget.
 *
 *  @param widget The text widget.
 */
void w_label_destroy(w_widget_t *widget)
{
    w_label_t *label = W_LABEL(widget);

    if (label->label)
        free(label->label);

    w_widget_destroy(widget);
}

void w_label_init(w_label_t *label, char *text)
{
    w_align_init((w_align_t *) label);

    label->render = w_label_render;
    label->destroy = w_label_destroy;
    label->id = w_label_get_class_id();
    label->label = strdup(text);
    label->bouncy = 0;
    w_system_get_string_size(text, &label->width, &label->height);
    label->height +=  BOUNCE_AMP;
}

/** @brief Creates a text widget.
 *
 *  A text widget consists of a single label. This widget has no input
 *  functionality.
 *
 *  @param string The text for the widget.
 *  @return The created text widget.
 */
w_widget_t *w_label_create(char *string)
{
    w_label_t *label = malloc(sizeof(w_label_t));

    w_label_init(label, string);

    return W_WIDGET(label);
}
