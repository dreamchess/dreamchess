/*  DreamChess
 *  Copyright (C) 2006  The DreamChess project
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
#include <string.h>

#include <gamegui/seperatorh.h>

#define GG_SEP_SPACINGV 5
#define GG_SEP_SPACINGH 10
#define GG_SEP_WIDTH 1

static gg_colour_t col_black =
    {
        0.0f, 0.0f, 0.0f, 1.0f
    };

gg_class_id gg_seperatorh_get_class_id()
{
    GG_CHILD(gg_widget_get_class_id())
}

void gg_seperatorh_render(gg_widget_t *widget, int x, int y, int focus)
{
    gg_seperatorh_t *sep = GG_SEPERATORH(widget);
    int width = sep->width_a - GG_SEP_SPACINGH * 2;

    if (width <= 0)
        return;

    gg_system_draw_filled_rect(x + GG_SEP_SPACINGH,
                               y + sep->height_a / 2 - GG_SEP_WIDTH / 2,
                               width, GG_SEP_WIDTH, &col_black);
}

void gg_seperatorh_init(gg_seperatorh_t *sep)
{
    gg_widget_init((gg_widget_t *) sep);

    sep->render = gg_seperatorh_render;
    sep->id = gg_seperatorh_get_class_id();
    sep->width = 0;
    sep->height = GG_SEP_WIDTH + GG_SEP_SPACINGV * 2;
}

gg_widget_t *gg_seperatorh_create()
{
    gg_seperatorh_t *sep = malloc(sizeof(gg_seperatorh_t));

    gg_seperatorh_init(sep);

    return GG_WIDGET(sep);
}
