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
#include <string.h>

#include <gamegui/system.h>
#include <gamegui/widget.h>
#include <gamegui/clipping.h>

static int classes = 0;
static gg_class_id *parent_class = NULL;
static gg_driver_t *driver;

gg_class_id gg_register_class(gg_class_id parent)
{
    parent_class = realloc(parent_class, (classes + 1) * sizeof(gg_class_id));

    parent_class[classes] = parent;

    return classes++;
}

gg_widget_t *gg_check_cast(gg_widget_t *widget, gg_class_id id, char *file, int line, char *type)
{
    if (!widget)
    {
        fprintf(stderr, "Fatal error (%s:L%d): Widget is NULL.\n", file, line);
        exit(1);
    }

    if (widget->id != id)
    {
        gg_class_id parent = parent_class[widget->id];

        while ((parent != GG_CLASS_ID_NONE) && (parent != id))
            parent = parent_class[parent];

        if (parent == GG_CLASS_ID_NONE)
        {
            fprintf(stderr, "Fatal error (%s:L%d): Widget is not of type %s.\n", file, line, type);
            exit(1);
        }
    }

    return widget;
}

void gg_system_init(gg_driver_t *d)
{
    driver = d;
}

void gg_system_exit()
{
    gg_dialog_cleanup();

    if (parent_class)
        free(parent_class);
}

unsigned int gg_system_get_ticks()
{
    return driver->get_ticks();
}

void gg_system_draw_rect(int x, int y, int width, int height, gg_colour_t *colour)
{
    gg_rect_t dest;
    dest.x = x;
    dest.y = y;
    dest.width = width;
    dest.height = 1;
    gg_system_draw_filled_rect(dest.x, dest.y, dest.width, dest.height, colour);
    dest.y += height - 1;
    gg_system_draw_filled_rect(dest.x, dest.y, dest.width, dest.height, colour);
    dest.y = y + 1;
    dest.width = 1;
    dest.height = height - 2;
    gg_system_draw_filled_rect(dest.x, dest.y, dest.width, dest.height, colour);
    dest.x += width - 1;
    gg_system_draw_filled_rect(dest.x, dest.y, dest.width, dest.height, colour);
}

void gg_system_draw_filled_rect(int x, int y, int width, int height, gg_colour_t *colour)
{
    gg_rect_t *clip = gg_clipping_get();

    if (clip)
    {
        gg_rect_t dest, dest_c;
        dest.x = x;
        dest.y = y;
        dest.width =  width;
        dest.height = height;
        dest_c = gg_clipping_rect(&dest, clip);
        driver->draw_filled_rect(dest_c.x, dest_c.y, dest_c.width, dest_c.height,
                          colour);
    }
    else
        driver->draw_filled_rect(x, y, width, height, colour);
}

void gg_system_draw_gradient_rect(int x, int y, int width, int height,
                                  gg_colour_t *top_left, gg_colour_t *top_right,
                                  gg_colour_t *bottom_left, gg_colour_t *bottom_right)
{
    gg_rect_t *clip = gg_clipping_get();

    if (clip)
    {
        gg_rect_t dest, dest_c;
        dest.x = x;
        dest.y = y;
        dest.width =  width;
        dest.height = height;
        dest_c = gg_clipping_rect(&dest, clip);
        driver->draw_gradient_rect(dest_c.x, dest_c.y, dest_c.width, dest_c.height,
                          top_left, top_right, bottom_left, bottom_right);
    }
    else
        driver->draw_gradient_rect(x, y, width, height, top_left, top_right,
                                   bottom_left, bottom_right);
}

void gg_system_draw_image(void *image, gg_rect_t source, gg_rect_t dest, int mode_h, int mode_v, gg_colour_t *colour)
{
    gg_rect_t *clip = gg_clipping_get();

    if (clip)
    {
        gg_rect_t dest_c;
        gg_rect_t source_c;
        int x_offset;
        int y_offset;

        dest_c = gg_clipping_rect(&dest, clip);
        x_offset = dest_c.x - dest.x;
        y_offset = dest.y + dest.height - dest_c.y - dest_c.height;

        switch (mode_h)
        {
        case GG_MODE_SCALE:
            {
                /* Magnification factor. */
                float x_mag = dest.width / (float) source.width;

                source_c.x = source.x + x_offset / x_mag;
                source_c.width = dest_c.width / x_mag;
            }
            break;
        case GG_MODE_TILE:
            source_c.x = source.x + x_offset % source.width;
            /* Width is undefined for GG_MODE_TILE. */
        }

        switch (mode_v)
        {
        case GG_MODE_SCALE:
            {
                /* Magnification factor. */
                float y_mag = dest.height / (float) source.height;

                source_c.y = source.y + y_offset / y_mag;
                source_c.height = dest_c.height / y_mag;
            }
            break;
        case GG_MODE_TILE:
            source_c.y = source.y + y_offset % source.height;
            /* Height is undefined for GG_MODE_TILE. */
        }

        driver->draw_image(image, source_c, dest_c, mode_h, mode_v, colour);
    }
    else
        driver->draw_image(image, source, dest, mode_h, mode_v, colour);
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

void gg_system_get_string_size(char *s, int *width, int *height)
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

void gg_system_draw_string(char *s, int x, int y, gg_colour_t *colour, int bounce, float align)
{
    int i;
    unsigned int ticks = gg_system_get_ticks();
    gg_rect_t rect_d;
    rect_d.x = x;

    if (align != 0.0f)
    {
        int width;

        gg_system_get_string_size(s, &width, NULL);

        rect_d.x -= width * align;
    }

    for (i = 0; i < strlen((char *) s); i++)
    {
        int y_off = 0;
        void *image = driver->get_char_image(s[i]);
        gg_rect_t rect_s = {0, 0};
        gg_colour_t col_black = {0.0f, 0.0f, 0.0f, 1.0f};

        if (bounce)
        {
            float phase = ((ticks % (1000 / GG_BOUNCE_SPEED)) / (float) (1000 / GG_BOUNCE_SPEED));

            if (phase < 0.5)
                y_off = phase * 2 * (GG_BOUNCE_AMP + 1);
            else
                y_off = ((1.0 - phase) * 2) * (GG_BOUNCE_AMP + 1);
        }

        gg_system_get_image_size(image, &rect_s.width, &rect_s.height);
        rect_d.width = rect_s.width;
        rect_d.height = rect_s.height;
        rect_d.y = y + y_off;

        /* FIXME */
        rect_d.x += 2;
        rect_d.y -= 2;
        gg_colour_t col_blackshad = {0.3f, 0.3f, 0.3f, 1.0f};
        if ( colour->r != 0.0f && colour->g != 0.0f && colour->b != 0.0f  )
            gg_system_draw_image(image, rect_s, rect_d, GG_MODE_SCALE, GG_MODE_SCALE, &col_black);
        else
            gg_system_draw_image(image, rect_s, rect_d, GG_MODE_SCALE, GG_MODE_SCALE, &col_blackshad);
        rect_d.x -= 2;
        rect_d.y += 2;

        gg_system_draw_image(image, rect_s, rect_d, GG_MODE_SCALE, GG_MODE_SCALE, colour);

        rect_d.x += rect_s.width;

        ticks += 1000 / GG_BOUNCE_SPEED / GG_BOUNCE_LEN;
    }
}

gg_colour_t gg_colour(float r, float g, float b, float a)
{
    gg_colour_t col;

    col.r = r;
    col.g = g;
    col.b = b;
    col.a = a;

    return col;
}

gg_rect_t gg_rect(int x, int y, int w, int h)
{
    gg_rect_t rect;

    rect.x = x;
    rect.y = y;
    rect.width = w;
    rect.height = h;

    return rect;
}
