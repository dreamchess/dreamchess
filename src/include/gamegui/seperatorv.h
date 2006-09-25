/*  DreamChess
 *  Copyright (C) 2006  The DreamChess project
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

#ifndef GAMEGUI_SEPERATORV_H
#define GAMEGUI_SEPERATORV_H

#include <gamegui/system.h>
#include <gamegui/widget.h>

#define GG_SEPERATORV(W) GG_CHECK_CAST(W, gg_seperatorv_get_class_id(), gg_seperatorv_t)

#define GG_SEPERATORV_DATA \
    GG_WIDGET_DATA

typedef struct gg_seperatorv
{
    GG_SEPERATORV_DATA
}
gg_seperatorv_t;

gg_class_id gg_seperatorv_get_class_id();

void gg_seperatorv_render(gg_widget_t *widget, int x, int y, int focus);

void gg_seperatorv_init(gg_seperatorv_t *sep);

gg_widget_t *gg_seperatorv_create();

#endif /* GAMEGUI_SEPERATORV_H */
