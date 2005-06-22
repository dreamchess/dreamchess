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

#define GG_BOX(W) GG_CHECK_CAST(W, gg_box_get_class_id(), gg_box_t)

#define GG_BOX_DATA \
    GG_SELECT_DATA \
    int spacing;

typedef struct gg_box
{
    GG_BOX_DATA
}
gg_box_t;

gg_class_id gg_box_get_class_id();

void gg_box_init(gg_box_t *box, int spacing);

#endif /* GAMEGUI_BOX_H */
