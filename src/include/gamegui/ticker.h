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

#ifndef GAMEGUI_TICKER_H
#define GAMEGUI_TICKER_H

#include <gamegui/box.h>

#define GG_TICKER(W) GG_CHECK_CAST(W, gg_ticker_get_class_id(), gg_ticker_t)

#define GG_TICKER_DATA \
    GG_BOX_DATA \
    int offset; \
    int start_time; \
    int focus_x;

typedef struct gg_ticker {
    GG_TICKER_DATA
} gg_ticker_t;

gg_class_id gg_ticker_get_class_id();

void gg_ticker_render(gg_widget_t *widget, int x, int y, int focus);

void gg_ticker_init(gg_ticker_t *ticker, int width, int spacing);

void gg_ticker_set_size(gg_widget_t *widget, int width, int height);

gg_rect_t gg_ticker_get_focus_pos(gg_widget_t *widget);

int gg_ticker_set_focus_pos(gg_widget_t *widget, int x, int y);

int gg_ticker_input(gg_widget_t *widget, gg_event_t event);

gg_widget_t *gg_ticker_create(int width, int spacing);

#endif /* GAMEGUI_TICKER_H */
