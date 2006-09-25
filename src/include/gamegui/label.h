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

#ifndef GAMEGUI_LABEL_H
#define GAMEGUI_LABEL_H

#include <gamegui/system.h>
#include <gamegui/align.h>

#define GG_LABEL(W) GG_CHECK_CAST(W, gg_label_get_class_id(), gg_label_t)

#define GG_LABEL_DATA \
    GG_ALIGN_DATA \
    char *label; \
    gg_colour_t colour; \
    gg_colour_t bgcolour; \
    int bouncy;

typedef struct gg_label
{
    GG_LABEL_DATA
}
gg_label_t;

gg_class_id gg_label_get_class_id();

void gg_label_render(gg_widget_t *widget, int x, int y, int focus);

void gg_label_set_bouncy(gg_label_t *label, int bouncy);

void gg_label_set_colour(gg_label_t *label, gg_colour_t *colour, gg_colour_t *bgcolour);

void gg_label_destroy(gg_widget_t *widget);

void gg_label_init(gg_label_t *label, char *text);

gg_widget_t *gg_label_create(char *string);

#endif /* GAMEGUI_LABEL_H */
