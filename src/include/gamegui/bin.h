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

#ifndef GAMEGUI_BIN_H
#define GAMEGUI_BIN_H

#include <gamegui/system.h>
#include <gamegui/container.h>

#define GG_BIN(W) GG_CHECK_CAST(W, gg_bin_get_class_id(), gg_bin_t)

#define GG_BIN_DATA \
    GG_CONTAINER_DATA

typedef struct gg_bin
{
    GG_BIN_DATA
}
gg_bin_t;

gg_class_id gg_bin_get_class_id();

gg_widget_t *gg_bin_get_child(gg_bin_t *bin);

void gg_bin_set_size(gg_widget_t *widget, int width, int height);

int gg_bin_set_focus_pos(gg_widget_t *widget, int x, int y);

void gg_bin_init(gg_bin_t *bin, gg_widget_t *child);

#endif /* GAMEGUI_BIN_H */
