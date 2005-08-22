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
static gg_class_id *parent_class = NULL;
static gg_driver_t *driver;

gg_widget_t *gg_cast_error(char *file, int line, char *type)
{
    fprintf(stderr, "Fatal error (%s:L%d): Widget is not of type %s.\n", file, line, type);
    exit(1);
    return NULL;
}

gg_class_id gg_register_class(gg_class_id parent)
{
    parent_class = realloc(parent_class, (classes + 1) * sizeof(gg_class_id));

    parent_class[classes] = parent;

    return classes++;
}

int gg_check_cast(gg_widget_t *widget, gg_class_id id)
{
    gg_class_id parent = parent_class[widget->id];

    if (widget->id == id)
        return 1;

    while ((parent != GG_CLASS_ID_NONE) && (parent != id))
        parent = parent_class[parent];

    return parent != GG_CLASS_ID_NONE;
}

void gg_system_init(gg_driver_t *d)
{
    driver = d;
}

unsigned int gg_system_get_ticks()
{
    return driver->get_ticks();
}

void gg_system_draw_rect(int x, int y, int width, int height, gg_colour_t *colour)
{
    driver->draw_rect(x, y, width, height, colour);
}

void gg_system_draw_filled_rect(int x, int y, int width, int height, gg_colour_t *colour)
{
    driver->draw_filled_rect(x, y, width, height, colour);
}

void gg_system_draw_image(void *image, gg_rect_t source, gg_rect_t dest, int mode_h, int mode_v)
{
    driver->draw_image(image, source, dest, mode_h, mode_v);
}

void gg_system_draw_char(int c, int x, int y, gg_colour_t *colour)
{
    driver->draw_char(c, x, y, colour);
}

void gg_system_get_image_size(void *image, int *width, int *height)
{
    driver->get_image_size(image, width, height);
}

void gg_system_get_char_size(int c, int *width, int *height)
{
    driver->get_char_size(c, width, height);
}

void gg_system_get_string_size(unsigned char *s, int *width, int *height)
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

void gg_system_draw_string(unsigned char *s, int x, int y, gg_colour_t *colour, int bounce, float align)
{
    int i;
    unsigned int ticks = gg_system_get_ticks();

    if (align != 0.0f)
    {
        int width;

        gg_system_get_string_size(s, &width, NULL);

        x -= width * align;
    }

    for (i = 0; i < strlen(s); i++)
    {
        int width;
        int y_off = 0;

        if (bounce)
        {
            float phase = ((ticks % (1000 / GG_BOUNCE_SPEED)) / (float) (1000 / GG_BOUNCE_SPEED));

            if (phase < 0.5)
                y_off = phase * 2 * (GG_BOUNCE_AMP + 1);
            else
                y_off = ((1.0 - phase) * 2) * (GG_BOUNCE_AMP + 1);
        }

        driver->get_char_size(s[i], &width, NULL);
        driver->draw_char(s[i], x, y + y_off, colour);
        x += width;

        ticks += 1000 / GG_BOUNCE_SPEED / GG_BOUNCE_LEN;
    }
}
