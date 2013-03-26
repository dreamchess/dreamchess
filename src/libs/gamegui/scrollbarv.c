/*  DreamChess
**
**  DreamChess is the legal property of its developers, whose names are too
**  numerous to list here. Please refer to the COPYRIGHT file distributed
**  with this source distribution.
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <string.h>

#include <gamegui/scrollbarv.h>

#define SCROLLBARV_WIDTH 16

static gg_colour_t col_grey = {
    0.5f, 0.5f, 0.5f, 1.0f
};

static gg_colour_t col_texthighlight = {
    0.55f, 0.65f, 0.95f, 1.0f
};

static gg_colour_t col_text = {
    1.0f, 1.0f, 1.0f, 1.0f
};

gg_class_id gg_scrollbarv_get_class_id(void)
{
GG_CHILD(gg_widget_get_class_id())
}

#if 0
static int string_width(char *s, int n)
{
    int retval;
    char c = s[n];

    s[n] = 0;
    gg_system_get_string_size(s, &retval, NULL);
    s[n] = c;
    return retval;
}
#endif

void gg_scrollbarv_render(gg_widget_t * widget, int x, int y, int focus)
{
    gg_scrollbarv_t *scrollbarv = GG_SCROLLBARV(widget);
/*    gg_rect_t rect; */
    gg_colour_t *colour;
/*    struct gg_scrollbarv_line *line; */

    switch (focus)
    {
    case GG_FOCUS_DISABLED:
        colour = &col_grey;
        break;
    case GG_FOCUS_ONE:
    case GG_FOCUS_ALL:
        colour = &col_texthighlight;
        break;
    case GG_FOCUS_NONE:
        colour = &col_text;
    }

	/* TODO Fix temporary hack */
	if (!widget->enabled)
		colour = &col_grey;
	
    gg_system_draw_rect(x, y, scrollbarv->width_a, scrollbarv->height_a, colour);
}

int gg_scrollbarv_input(gg_widget_t * widget, gg_event_t event)
{
    return 1;
}

void gg_scrollbarv_init(gg_scrollbarv_t * scrollbarv, int height)
{
    gg_widget_init((gg_widget_t *) scrollbarv);

    scrollbarv->render = gg_scrollbarv_render;
    scrollbarv->input = gg_scrollbarv_input;
    scrollbarv->id = gg_scrollbarv_get_class_id();
    scrollbarv->enabled = 1;
    scrollbarv->width = SCROLLBARV_WIDTH;
    scrollbarv->height = height;
}

gg_widget_t *gg_scrollbarv_create(int height)
{
    gg_scrollbarv_t *scrollbarv = malloc(sizeof(gg_scrollbarv_t));

    gg_scrollbarv_init(scrollbarv, height);

    return GG_WIDGET(scrollbarv);
}

