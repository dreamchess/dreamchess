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

#include <gamegui/image.h>

/** Focussed image scale value. */
#define IMAGE_SCALE -0.3f
/** Focussed image enlargement speed in enlargements per second. */
#define IMAGE_SPEED 2.0f

static w_colour_t col_white =
    {
        1.0f, 1.0f, 1.0f, 1.0f
    };

w_class_id w_image_get_class_id()
{
    CHILD(w_align_get_class_id())
}

/** Implements widget::render for image widgets. */
void w_image_render(w_widget_t *widget, int x, int y, int focus)
{
    w_image_t *image = W_IMAGE(widget);
    int w = image->width;
    int h = image->height;
    Uint32 ticks = SDL_GetTicks();
    float phase = ((ticks % (int) (1000 / IMAGE_SPEED)) / (float) (1000 / IMAGE_SPEED));
    float factor;

    if (phase < 0.5f)
        factor = 1.0f + IMAGE_SCALE * phase * 2;
    else
        factor = 1.0f + IMAGE_SCALE * ((1.0f - phase) * 2);

    x += image->xalign * (image->width_a - image->width);
    y += (1.0f - image->yalign) * (image->height_a - image->height);


    if (focus != FOCUS_NONE)
    {
        w *= factor;
        h *= factor;
    }

/*    draw_texture(image->image, x - (w - image->width)/2, y - (h - image->height)/2, w, h, 1.0f, &col_white);*/
}

void w_image_init(w_image_t *image, void *texture)
{
    w_align_init((w_align_t *) image);

    image->render = w_image_render;
    image->id = w_image_get_class_id();
    image->image = texture;
    image->enabled = 1;
/*
    image->width = texture->width;
    image->height = texture->height;
*/
}

/** @brief Creates an image widget. */
w_widget_t *w_image_create(void *texture)
{
    w_image_t *image = malloc(sizeof(w_image_t));

    w_image_init(image, texture);

    return W_WIDGET(image);
}
