/*  DreamChess
**
**  DreamChess is the legal property of its developers, whose names are too
**  numerous to list here. Please refer to the COPYRIGHT file distributed
**  with this source distribution.
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/** @file
 *  @brief Action widget.
 */

#ifndef GAMEGUI_ACTION_H
#define GAMEGUI_ACTION_H

#include <gamegui/system.h>
#include <gamegui/bin.h>

/** Typecast to action widget. */
#define GG_ACTION(W) GG_CHECK_CAST(W, gg_action_get_class_id(), gg_action_t)

#define GG_ACTION_DATA \
    GG_BIN_DATA \
    gg_signal_t pressed; 

typedef struct gg_action
{
    GG_ACTION_DATA
}
gg_action_t;

gg_class_id gg_action_get_class_id(void);

void gg_action_render(gg_widget_t *widget, int x, int y, int focus);

int gg_action_input(gg_widget_t *widget, gg_event_t event);

void gg_action_init(gg_action_t *action, gg_widget_t *widget);

gg_widget_t *gg_action_create(gg_widget_t *widget);

gg_widget_t *gg_action_create_with_label(char *text, float xalign, float yalign);

#endif
