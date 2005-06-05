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

#ifndef GAMEGUI_SELECT_H
#define GAMEGUI_SELECT_H

#include <gamegui/system.h>
#include <gamegui/bin.h>

#define W_SELECT(W) CHECK_CAST(W, w_select_get_class_id(), w_select_t)

#define W_SELECT_DATA \
    W_CONTAINER_DATA \
    int sel;

typedef struct w_select
{
    W_SELECT_DATA
}
w_select_t;

w_class_id w_select_get_class_id();

void w_select_init(w_select_t *select);

int w_select_prev(w_select_t *select, int input, int enabled);

int w_select_next(w_select_t *select, int input, int enabled);

#endif /* GAMEGUI_SELECT_H */
