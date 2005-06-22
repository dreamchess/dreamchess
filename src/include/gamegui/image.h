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

#define GG_IMAGE(W) GG_CHECK_CAST(W, gg_image_get_class_id(), gg_image_t)

#define GG_IMAGE_DATA \
    GG_ALIGN_DATA \
    void *image;

typedef struct gg_image
{
    GG_IMAGE_DATA
}
gg_image_t;

gg_class_id gg_image_get_class_id();

void gg_image_render(gg_widget_t *widget, int x, int y, int focus);

void gg_image_init(gg_image_t *image, void *texture);

gg_widget_t *gg_image_create(void *texture);

#endif /* GAMEGUI_IMAGE_H */
