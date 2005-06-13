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
    w_rect_t source, dest;

    if (phase < 0.5f)
        factor = 1.0f + IMAGE_SCALE * phase * 2;
    else
        factor = 1.0f + IMAGE_SCALE * ((1.0f - phase) * 2);

    dest.width = image->width;
    dest.height = image->height;

    if (focus != FOCUS_NONE)
    {
        dest.width *= factor;
        dest.height *= factor;
    }

    dest.x = x + image->xalign * (image->width_a - image->width) - (dest.width - image->width) / 2;
    dest.y = y + (1.0f - image->yalign) * (image->height_a - image->height) - (dest.height - image->height) / 2;

    source.x = 0;
    source.y = 0;
    source.width = image->width;
    source.height = image->width;

    w_system_draw_image(image->image, source, dest, GG_MODE_SCALE, GG_MODE_SCALE);
}

void w_image_init(w_image_t *image, void *texture)
{
    w_align_init((w_align_t *) image);

    image->render = w_image_render;
    image->id = w_image_get_class_id();
    image->image = texture;
    image->enabled = 1;
    w_system_get_image_size(texture, &image->width, &image->height);
}

/** @brief Creates an image widget. */
w_widget_t *w_image_create(void *texture)
{
    w_image_t *image = malloc(sizeof(w_image_t));

    w_image_init(image, texture);

    return W_WIDGET(image);
}
