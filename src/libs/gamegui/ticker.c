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

#include <stdlib.h>

#include <gamegui/ticker.h>
#include <gamegui/clipping.h>

gg_class_id gg_ticker_get_class_id() {
    GG_CHILD(gg_box_get_class_id())
}

static gg_widget_t *get_child_for_x(gg_ticker_t *ticker, int x) {
	int size = gg_container_get_size(GG_CONTAINER(ticker));

	int child_x = ticker->width_a - ticker->offset;
	int i;
	for (i = 0; i < size; i++) {
		gg_widget_t *child = gg_container_get_child(GG_CONTAINER(ticker), i);
		if (child_x <= x && x < child_x + child->width_a)
			return gg_container_get_child(GG_CONTAINER(ticker), i);
		child_x += child->width_a;
	}
	return NULL;
}

void gg_ticker_render(gg_widget_t *widget, int x, int y, int focus) {
	gg_box_t *box = GG_BOX(widget);
    gg_rect_t rect;
    rect.x = x;
    rect.y = y;
    rect.width = widget->width_a;
    rect.height = widget->height_a;

    gg_clipping_adjust(&rect);

	int size = gg_container_get_size(GG_CONTAINER(widget));
	int i;

	gg_ticker_t *ticker = GG_TICKER(widget);
	int child_x = ticker->width_a - ticker->offset;
	for (i = 0; i < size; i++) {
		gg_widget_t *child = gg_container_get_child(GG_CONTAINER(widget), i);
		if (child_x < widget->width_a && child_x + child->width_a > 0) {
			int focus_child;

			if (focus == GG_FOCUS_ALL)
				focus_child = GG_FOCUS_ALL;
			else if (focus == GG_FOCUS_ONE)
				focus_child = (box->sel == i ? GG_FOCUS_ONE : GG_FOCUS_NONE);
			else
				focus_child = 0;

			child->render(child, x + child_x, y, focus_child);
		}
		child_x += child->width_a + ticker->spacing;
	}

    gg_clipping_undo();
	if (child_x <= 0)
		ticker->offset = 0;
	else
		ticker->offset += 1;
}

void gg_ticker_get_requested_size(gg_widget_t *widget, int *width, int *height) {
	gg_container_t *container = GG_CONTAINER(widget);
	int size = gg_container_get_size(container);
	int i;

	widget->height = 0;

	for (i = 0; i < size; i++) {
		int child_width, child_height;
		gg_widget_t *child = gg_container_get_child(container, i);

		child->get_requested_size(child, &child_width, &child_height);

		if (child_height > widget->height)
			widget->height = child_height;
	}

	gg_widget_get_requested_size(widget, width, height);
}

void gg_ticker_set_size(gg_widget_t *widget, int width, int height) {
	int i;

	for (i = 0; i < gg_container_get_size(GG_CONTAINER(widget)); i++) {
		gg_widget_t *child = gg_container_get_child(GG_CONTAINER(widget), i);
		int item_width;

		child->get_requested_size(child, &item_width, NULL);
		child->set_size(child, item_width, height);
	}

	gg_set_size(widget, width, height);
}

int gg_ticker_set_focus_pos(gg_widget_t *widget, int x, int y) {
	gg_box_t *box = GG_BOX(widget);
	gg_container_t *container = GG_CONTAINER(widget);
	int cur_x = widget->width_a - GG_TICKER(widget)->offset;
	int prev = box->sel;

	box->sel = -1;

	while (gg_select_next(GG_SELECT(widget), 0, 0))
	{
		gg_widget_t *child = gg_container_get_child(container, box->sel);

		if (x >= cur_x && cur_x + child->width_a > x) {
			if (!child->input || !child->enabled ||
					(!child->set_focus_pos(child, x - cur_x, y)))
				break;
			else
				return 1;
		}
		cur_x += child->width_a + box->spacing;
	}

	box->sel = prev;
	return 0;
}

int gg_ticker_input(gg_widget_t *widget, gg_event_t event) {
	if (event.type == GG_EVENT_MOUSE) {
		gg_widget_t *child = get_child_for_x(GG_TICKER(widget), event.mouse.x);
		if (child) {
			event.mouse.x = 0;
			return child->input(child, event);
		}
	}
	return 0;
}

void gg_ticker_init(gg_ticker_t *ticker, int width, int spacing) {
    gg_box_init((gg_box_t *) ticker, spacing);

    ticker->render = gg_ticker_render;
    ticker->get_requested_size = gg_ticker_get_requested_size;
    ticker->set_size = gg_ticker_set_size;
    ticker->set_focus_pos = gg_ticker_set_focus_pos;
    ticker->input = gg_ticker_input;
    ticker->id = gg_ticker_get_class_id();
    ticker->width = width;
    ticker->offset = 0;
}

gg_widget_t *gg_ticker_create(int width, int spacing) {
    gg_ticker_t *ticker = malloc(sizeof(gg_ticker_t));

    gg_ticker_init(ticker, width, spacing);

    return GG_WIDGET(ticker);
}
