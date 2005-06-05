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

#ifndef GAMEGUI_SYSTEM_H
#define GAMEGUI_SYSTEM_H

#define FOCUS_NONE 0
#define FOCUS_ONE 1
#define FOCUS_ALL 2

/** Bouncy text amplitude. */
#define BOUNCE_AMP 2
/** Bouncy text wave length in characters. */
#define BOUNCE_LEN 10
/** Bouncy text speed in bounces per second. */
#define BOUNCE_SPEED 3

typedef int w_class_id;

typedef struct w_colour
{
    /** Red channel. Ranges from 0.0f to 1.0f. */
    float r;

    /** Green channel. Ranges from 0.0f to 1.0f. */
    float g;

    /** Blue channel. Ranges from 0.0f to 1.0f. */
    float b;

    /** Alpha channel. Ranges from 0.0f (transparent) to 1.0f (opaque). */
    float a;
}
w_colour_t;

typedef struct w_rect
{
    int x, y;
    int width, height;
}
w_rect_t;

typedef struct w_driver
{
    void (* draw_rect) (int x, int y, int width, int height, w_colour_t *colour);
    void (* draw_filled_rect) (int x, int y, int width, int height, w_colour_t *colour);
    void (* draw_image) (void *image, w_rect_t source, w_rect_t dest);
    void (* draw_char) (int c, int x, int y, w_colour_t *colour);
    void (* get_image_size) (void *image, int *width, int *height);
    void (* get_char_size) (int c, int *width, int *height);
}
w_driver_t;

#define CLASS_ID_NONE -1

#define CAST_ERROR(C) ((C *) cast_error(__FILE__, __LINE__, #C))

#define CHECK_CAST(W, I, C) ((w_check_cast((w_widget_t *) W, I)) ? (C *) W \
    : CAST_ERROR(C))

#define CHILD(C) \
    static w_class_id class_id = CLASS_ID_NONE;\
    if (class_id == CLASS_ID_NONE) \
        class_id = w_register_class(C); \
    return class_id;

/* w_widget_t *cast_error(char *file, int line, char *type); */

w_class_id w_register_class(w_class_id parent);

/* int w_check_cast(w_widget_t *widget, w_class_id id); */

void w_system_init(w_driver_t *d);

void w_system_draw_rect(int x, int y, int width, int height, w_colour_t *colour);

void w_system_draw_filled_rect(int x, int y, int width, int height, w_colour_t *colour);

void w_system_draw_image(void *image, w_rect_t source, w_rect_t dest);

void w_system_draw_char(int c, int x, int y, w_colour_t *colour);

void w_system_get_image_size(void *image, int *width, int *height);

void w_system_get_char_size(int c, int *width, int *height);

void w_system_get_string_size(unsigned char *s, int *width, int *height);

void w_system_draw_string(unsigned char *s, int x, int y, w_colour_t *colour, int bounce, float align);

#endif /* GAMEGUI_SYSTEM_H */
