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

#define W_LABEL(W) CHECK_CAST(W, w_label_get_class_id(), w_label_t)

#define W_LABEL_DATA \
    W_ALIGN_DATA \
    char *label; \
    int bouncy;

typedef struct w_label
{
    W_LABEL_DATA
}
w_label_t;

w_class_id w_label_get_class_id();

void w_label_render(w_widget_t *widget, int x, int y, int focus);

void w_label_set_bouncy(w_label_t *label, int bouncy);

void w_label_destroy(w_widget_t *widget);

void w_label_init(w_label_t *label, char *text);

w_widget_t *w_label_create(char *string);

#endif /* GAMEGUI_LABEL_H */
