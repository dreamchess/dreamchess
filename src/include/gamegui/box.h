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

#ifndef GAMEGUI_BOX_H
#define GAMEGUI_BOX_H

#include <gamegui/system.h>
#include <gamegui/select.h>

#define W_BOX(W) CHECK_CAST(W, w_box_get_class_id(), w_box_t)

#define W_BOX_DATA \
    W_SELECT_DATA \
    int spacing;

typedef struct w_box
{
    W_BOX_DATA
}
w_box_t;

w_class_id w_box_get_class_id();

void w_box_init(w_box_t *box, int spacing);

#endif /* GAMEGUI_BOX_H */
