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

#ifndef GAMEGUI_IMAGE_H
#define GAMEGUI_IMAGE_H

#include <gamegui/system.h>
#include <gamegui/align.h>

#define W_IMAGE(W) CHECK_CAST(W, w_image_get_class_id(), w_image_t)

#define W_IMAGE_DATA \
    W_ALIGN_DATA \
    void *image;

typedef struct w_image
{
    W_IMAGE_DATA
}
w_image_t;

w_class_id w_image_get_class_id();

void w_image_render(w_widget_t *widget, int x, int y, int focus);

void w_image_init(w_image_t *image, void *texture);

w_widget_t *w_image_create(void *texture);

#endif /* GAMEGUI_IMAGE_H */
