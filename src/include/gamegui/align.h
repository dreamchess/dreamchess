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

#ifndef GAMEGUI_ALIGN_H
#define GAMEGUI_ALIGN_H

#include <gamegui/system.h>
#include <gamegui/widget.h>

#define W_ALIGN(W) CHECK_CAST(W, w_align_get_class_id(), w_align_t)

#define W_ALIGN_DATA \
    W_WIDGET_DATA \
    float xalign; \
    float yalign;

typedef struct w_align
{
    W_ALIGN_DATA
}
w_align_t;

w_class_id w_align_get_class_id();

void w_align_init(w_align_t *align);

void w_align_set_alignment(w_align_t *align, float xalign, float yalign);

#endif /* GAMEGUI_ALIGN_H */
