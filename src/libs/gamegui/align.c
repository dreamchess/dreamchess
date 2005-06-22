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

#include <gamegui/align.h>

gg_class_id gg_align_get_class_id()
{
    GG_CHILD(gg_widget_get_class_id())
}

void gg_align_init(gg_align_t *align)
{
    gg_widget_init((gg_widget_t *) align);

    align->id = gg_align_get_class_id();
    align->xalign = 0.0f;
    align->yalign = 0.0f;
}

void gg_align_set_alignment(gg_align_t *align, float xalign, float yalign)
{
    align->xalign = xalign;
    align->yalign = yalign;
}
