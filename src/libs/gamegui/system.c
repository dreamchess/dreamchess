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

#include <stdio.h>
#include <stdlib.h>

#include <gamegui/system.h>
#include <gamegui/widget.h>

static int classes = 0;
static w_class_id *parent_class = NULL;
static w_driver_t *driver;

w_widget_t *cast_error(char *file, int line, char *type)
{
    fprintf(stderr, "Fatal error (%s:L%d): Widget is not of type %s.\n", file, line, type);
    exit(1);
    return NULL;
}

w_class_id w_register_class(w_class_id parent)
{
    parent_class = realloc(parent_class, (classes + 1) * sizeof(w_class_id));

    parent_class[classes] = parent;

    return classes++;
}

int w_check_cast(w_widget_t *widget, w_class_id id)
{
    w_class_id parent = parent_class[widget->id];

    if (widget->id == id)
        return 1;

    while ((parent != CLASS_ID_NONE) && (parent != id))
        parent = parent_class[parent];

    return parent != CLASS_ID_NONE;
}

void w_system_init(w_driver_t *d)
{
    driver = d;
}

void w_system_draw_rect(int x, int y, int width, int height, w_colour_t *colour)
{
    driver->draw_rect(x, y, width, height, colour);
}

void w_system_draw_filled_rect(int x, int y, int width, int height, w_colour_t *colour)
{
    driver->draw_filled_rect(x, y, width, height, colour);
}

void w_system_draw_image(void *image, w_rect_t source, w_rect_t dest)
{

    driver->draw_image(image, source, dest);
}

void w_system_draw_char(int c, int x, int y, w_colour_t *colour)
{
    driver->draw_char(c, x, y, colour);
}

void w_system_get_image_size(void *image, int *width, int *height)
{
    driver->get_image_size(image, width, height);
}

void w_system_get_char_size(int c, int *width, int *height)
{
    driver->get_char_size(c, width, height);
}

void w_system_get_string_size(unsigned char *s, int *width, int *height)
{
    int i;

    if (width)
        *width = 0;
    if (height)
        *height = 0;

    for (i = 0; i < strlen(s); i++)
    {
        int c_width, c_height;
        driver->get_char_size(s[i], &c_width, &c_height);

        if (width)
            *width += c_width;
        if (height && (c_height > *height))
            *height = c_height;
    }
}

void w_system_draw_string(unsigned char *s, int x, int y, w_colour_t *colour, int bounce, float align)
{
    int i;
    Uint32 ticks = SDL_GetTicks();

    if (align != 0.0f)
    {
        int width;

        w_system_get_string_size(s, &width, NULL);

        x -= width * align;
    }

    for (i = 0; i < strlen(s); i++)
    {
        int width;
        int y_off = 0;

        if (bounce)
        {
            float phase = ((ticks % (1000 / BOUNCE_SPEED)) / (float) (1000 / BOUNCE_SPEED));

            if (phase < 0.5)
                y_off = phase * 2 * (BOUNCE_AMP + 1);
            else
                y_off = ((1.0 - phase) * 2) * (BOUNCE_AMP + 1);
        }

        driver->get_char_size(s[i], &width, NULL);
        driver->draw_char(s[i], x, y + y_off, colour);
        x += width;

        ticks += 1000 / BOUNCE_SPEED / BOUNCE_LEN;
    }
}
