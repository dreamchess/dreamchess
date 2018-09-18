/*  DreamChess
**
**  DreamChess is the legal property of its developers, whose names are too
**  numerous to list here. Please refer to the AUTHORS.txt file distributed
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

#include <gamegui/image.h>

/** Focussed image scale value. */
#define IMAGE_SCALE -0.3f
/** Focussed image enlargement speed in enlargements per second. */
#define IMAGE_SPEED 2.0f

static gg_colour_t col_white =
    {
        1.0f, 1.0f, 1.0f, 1.0f
    };

gg_class_id gg_image_get_class_id(void)
{
    GG_CHILD(gg_align_get_class_id())
}

/** Implements widget::render for image widgets. */
void gg_image_render(gg_widget_t *widget, int x, int y, int focus)
{
    gg_image_t *image = GG_IMAGE(widget);
    unsigned int ticks = gg_system_get_ticks();
    float phase = ((ticks % (int) (1000 / IMAGE_SPEED)) / (float) (1000 / IMAGE_SPEED));
    float factor;
    gg_rect_t source, dest;

    if (phase < 0.5f)
        factor = 1.0f + IMAGE_SCALE * phase * 2;
    else
        factor = 1.0f + IMAGE_SCALE * ((1.0f - phase) * 2);

    dest.width = image->width;
    dest.height = image->height;

    if (focus != GG_FOCUS_NONE)
    {
        dest.width *= factor;
        dest.height *= factor;
    }

    dest.x = x + image->xalign * (image->width_a - image->width) - (dest.width - image->width) / 2;
    dest.y = y + (1.0f - image->yalign) * (image->height_a - image->height) - (dest.height - image->height) / 2;

    source.x = 0;
    source.y = 0;
    source.width = image->width;
    source.height = image->height;

    gg_system_draw_image(image->image, source, dest, GG_MODE_SCALE, GG_MODE_SCALE, &col_white);
}

void gg_image_init(gg_image_t *image, void *texture)
{
    gg_align_init((gg_align_t *) image);

    image->render = gg_image_render;
    image->id = gg_image_get_class_id();
    image->image = texture;
    image->enabled = 1;
    gg_system_get_image_size(texture, &image->width, &image->height);
}

/** @brief Creates an image widget. */
gg_widget_t *gg_image_create(void *texture)
{
    gg_image_t *image = malloc(sizeof(gg_image_t));

    gg_image_init(image, texture);

    return GG_WIDGET(image);
}
