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

static gg_colour_t col_dark_red =
    {
        0.7f, 0.0f, 0.0f, 1.0f
    };

static gg_colour_t col_black =
    {
        0.0f, 0.0f, 0.0f, 1.0f
    };

gg_class_id gg_label_get_class_id()
{
    GG_CHILD(gg_align_get_class_id())
}

/** Implements widget::render for text widgets. */
void gg_label_render(gg_widget_t *widget, int x, int y, int focus)
{
    gg_label_t *label = GG_LABEL(widget);

    x += label->xalign * (label->width_a - label->width);
    y += (1.0f - label->yalign) * (label->height_a - label->height);

    if (focus != GG_FOCUS_NONE)
        gg_system_draw_string(label->label, x, y, &col_dark_red, label->bouncy, 0);
    else
        gg_system_draw_string(label->label, x, y, &col_black, 0, 0);
}

void gg_label_set_bouncy(gg_label_t *label, int bouncy)
{
    label->bouncy = bouncy;
}

/** @brief Destroys a text widget.
 *
 *  @param widget The text widget.
 */
void gg_label_destroy(gg_widget_t *widget)
{
    gg_label_t *label = GG_LABEL(widget);

    if (label->label)
        free(label->label);

    gg_widget_destroy(widget);
}

void gg_label_init(gg_label_t *label, char *text)
{
    gg_align_init((gg_align_t *) label);

    label->render = gg_label_render;
    label->destroy = gg_label_destroy;
    label->id = gg_label_get_class_id();
    label->label = strdup(text);
    label->bouncy = 0;
    gg_system_get_string_size(text, &label->width, &label->height);
    label->height += GG_BOUNCE_AMP;
}

/** @brief Creates a text widget.
 *
 *  A text widget consists of a single label. This widget has no input
 *  functionality.
 *
 *  @param string The text for the widget.
 *  @return The created text widget.
 */
gg_widget_t *gg_label_create(char *string)
{
    gg_label_t *label = malloc(sizeof(gg_label_t));

    gg_label_init(label, string);

    return GG_WIDGET(label);
}
