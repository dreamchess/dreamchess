/*  DreamChess
**
**  DreamChess is the legal property of its developers, whose names are too
**  numerous to list here. Please refer to the AUTHORS.txt file distributed
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gamegui/clipping.h>
#include <gamegui/dialog.h>

void gg_dialog_destroy2(gg_widget_t *widget);

/** Titlebar size factor relative to text height */
#define GG_DIALOG_TITLE_FACT 1.25

/** Titlebar seperator height in pixels */
#define GG_DIALOG_TITLE_SEP_HEIGHT 1

static gg_colour_t col_white = {1.0f, 1.0f, 1.0f, 1.0f};

static gg_colour_t col_black = {0.0f, 0.0f, 0.0f, 1.0f};

static gg_colour_t col_blue = {0.0f, 0.5f, 1.0f, 1.0f};

static gg_colour_t col_blue2 = {0.0f, 0.0f, 0.5f, 1.0f};

static gg_colour_t col_grey = {0.5f, 0.5f, 0.5f, 1.0f};

static gg_colour_t col_grey2 = {0.25f, 0.25f, 0.25f, 1.0f};

gg_class_id gg_dialog_get_class_id(void) {
	GG_CHILD(gg_bin_get_class_id())
}

static TAILQ_HEAD(dialogs_head, gg_dialog) dialogs = TAILQ_HEAD_INITIALIZER(dialogs);
static TAILQ_HEAD(closed_dialogs_head, gg_dialog) closed_dialogs = TAILQ_HEAD_INITIALIZER(closed_dialogs);

void gg_dialog_cleanup(void) {
	while (!TAILQ_EMPTY(&closed_dialogs)) {
		gg_dialog_t *dialog = TAILQ_FIRST(&closed_dialogs);
		TAILQ_REMOVE(&closed_dialogs, dialog, entries);
		dialog->destroy(GG_WIDGET(dialog));
	}
}

/** @brief Adds a dialog to the top of the dialog stack.
 *  @param dialog The dialog to add.
 */
void gg_dialog_open(gg_dialog_t *dialog) {
	TAILQ_INSERT_HEAD(&dialogs, dialog, entries);

	if ((dialog->flags & GG_DIALOG_AUTOHIDE_PARENT) && dialog->parent_dialog)
		dialog->parent_dialog->flags |= GG_DIALOG_HIDDEN;
}

static void gg_dialog_cls(gg_dialog_t *dialog) {
	if ((dialog->flags & GG_DIALOG_AUTOHIDE_PARENT) && dialog->parent_dialog)
		dialog->parent_dialog->flags &= ~GG_DIALOG_HIDDEN;

	TAILQ_REMOVE(&dialogs, dialog, entries);
	TAILQ_INSERT_HEAD(&closed_dialogs, dialog, entries);
}

/** @brief Closes the dialog that's on top of the dialog stack. */
void gg_dialog_close(void) {
	if (!TAILQ_EMPTY(&dialogs))
		gg_dialog_cls(TAILQ_FIRST(&dialogs));
}

void gg_dialog_close_all(void) {
	while (!TAILQ_EMPTY(&dialogs))
		gg_dialog_cls(TAILQ_FIRST(&dialogs));
}

/** @brief Returns the dialog that's on top of the stack.
 *
 *  @return The dialog that's on top of the stack, or NULL if the stack is
 *          empty.
 */
gg_dialog_t *gg_dialog_get_active(void) {
	return TAILQ_FIRST(&dialogs);
}

void gg_dialog_set_active(gg_dialog_t *dialog) {
	TAILQ_REMOVE(&dialogs, dialog, entries);
	TAILQ_INSERT_HEAD(&dialogs, dialog, entries);
}

void gg_dialog_get_screen_pos(gg_dialog_t *dialog, int *x, int *y) {
	*x = dialog->pos.x - dialog->width * dialog->pos.x_align;
	*y = dialog->pos.y - dialog->height * dialog->pos.y_align;
}

void draw_border(void *image[9], char *title, int active, gg_rect_t area, int size) {
	gg_rect_t source, dest;
	int image_size;
	int titlebar_height = 0;

	gg_system_get_image_size(image[0], &image_size, NULL);

	source.x = 0;
	source.y = 0;
	source.width = image_size;
	source.height = image_size;

	if (title) {
		int text_height;
		gg_system_get_string_size(title, NULL, &text_height);
		titlebar_height = text_height * GG_DIALOG_TITLE_FACT;
		titlebar_height += GG_DIALOG_TITLE_SEP_HEIGHT;

		dest.x = area.x;
		dest.y = area.y + area.height - titlebar_height;
		dest.width = area.width;
		dest.height = GG_DIALOG_TITLE_SEP_HEIGHT;

		gg_system_draw_filled_rect(dest.x, dest.y, dest.width, GG_DIALOG_TITLE_SEP_HEIGHT, &col_black);

		dest.y += GG_DIALOG_TITLE_SEP_HEIGHT;
		dest.height = titlebar_height - GG_DIALOG_TITLE_SEP_HEIGHT;

		if (active)
			gg_system_draw_gradient_rect(dest.x, dest.y, dest.width, dest.height, &col_blue2, &col_blue, &col_blue2,
										 &col_blue);
		else
			gg_system_draw_gradient_rect(dest.x, dest.y, dest.width, dest.height, &col_grey2, &col_grey, &col_grey2,
										 &col_grey);

		gg_clipping_adjust(&dest);
		dest.y += text_height * (GG_DIALOG_TITLE_FACT - 1) / 2;
		gg_system_draw_string(title, dest.x + dest.width / 2, dest.y, &col_white, 0, 0.5);
		gg_clipping_undo();
	}

	area.x -= size;
	area.y -= size;

	area.width += 2 * size;
	area.height += 2 * size;

	dest.x = area.x;
	dest.y = area.y;
	dest.width = size;
	dest.height = size;

	/* Draw four corners.. */
	gg_system_draw_image(image[6], source, dest, GG_MODE_SCALE, GG_MODE_SCALE, &col_white);
	dest.y += area.height - size;
	gg_system_draw_image(image[0], source, dest, GG_MODE_SCALE, GG_MODE_SCALE, &col_white);
	dest.x += area.width - size;
	gg_system_draw_image(image[2], source, dest, GG_MODE_SCALE, GG_MODE_SCALE, &col_white);
	dest.y -= area.height - size;
	gg_system_draw_image(image[8], source, dest, GG_MODE_SCALE, GG_MODE_SCALE, &col_white);

	/* Draw bottom */
	dest.x = area.x + size;
	dest.y = area.y;
	dest.width = area.width - (2 * size);
	dest.height = size;
	gg_system_draw_image(image[7], source, dest, GG_MODE_TILE, GG_MODE_SCALE, &col_white);

	/* Draw top */
	dest.y += area.height - size;
	gg_system_draw_image(image[1], source, dest, GG_MODE_TILE, GG_MODE_SCALE, &col_white);

	/* Draw left */
	dest.x = area.x;
	dest.y = area.y + size;
	dest.width = size;
	dest.height = area.height - (2 * size);
	gg_system_draw_image(image[3], source, dest, GG_MODE_SCALE, GG_MODE_TILE, &col_white);

	/* Draw right */
	dest.x += area.width - size;
	gg_system_draw_image(image[5], source, dest, GG_MODE_SCALE, GG_MODE_TILE, &col_white);

	/* Draw middle */
	dest.x = area.x + size;
	dest.width = area.width - 2 * size;
	dest.height -= titlebar_height;
	gg_system_draw_image(image[4], source, dest, GG_MODE_TILE, GG_MODE_TILE, &col_white);
}

static float dialog_transition_speed = 200;
static float dialog_transition_start_pos;
static int dialog_trans_in;
static int dialog_trans_reset;
static int dialog_in_trans;

#if 0
static void dialog_reset_transition(int in)
{
    dialog_trans_in = in;
    dialog_trans_reset = 1;
    dialog_in_trans = 1;
}
#endif

static float dialog_get_transition(void) {
	float ticks = gg_system_get_ticks();

	if (dialog_trans_reset) {
		dialog_transition_start_pos = gg_system_get_ticks();
		dialog_trans_reset = 0;
	}

	float dialog_transition = ((ticks - dialog_transition_start_pos) / dialog_transition_speed);

	if (dialog_transition > 1.0f) {
		dialog_transition = 1.0f;
		dialog_in_trans = 0;
	}

	if (dialog_trans_in)
		return dialog_transition;
	else
		return 1.0f - dialog_transition;
}

/** @brief Renders a dialog.
 *
 *  Renders a dialog in a specific style and at a specific position.
 *
 *  @param menu The dialog to render.
 *  @param style The style to render in.
 *  @param pos The position to render at.
 */
void gg_dialog_render(gg_dialog_t *dialog) {
	gg_dialog_style_t *style = &dialog->style;
	gg_widget_t *child = gg_bin_get_child(GG_BIN(dialog));
	int size;
	int active;
	gg_rect_t area;

	int xmin, xmax, ymin, ymax;

	if (dialog->flags & GG_DIALOG_HIDDEN)
		return;

	gg_dialog_get_screen_pos(dialog, &xmin, &ymin);

	xmax = (xmin + dialog->width);
	ymax = (ymin + dialog->height);

	/* Draw the 'fade' */
	gg_system_draw_filled_rect(0, 0, gg_system_get_screen_width(), gg_system_get_screen_height(), &style->fade_col);

	gg_system_get_image_size(style->border.image[0], &size, NULL);

	xmin += size;
	xmax -= size;
	ymin += size;
	ymax -= size;

	area.x = xmin;
	area.y = ymin;
	area.width = xmax - xmin;
	area.height = ymax - ymin;

	active = gg_dialog_get_active() == dialog;

	if (dialog_in_trans)
		area.height *= dialog_get_transition();

	draw_border(style->border.image, dialog->title, active, area, size);

	xmin += style->hor_pad;
	xmax -= style->hor_pad;
	ymin += style->vert_pad;
	ymax -= style->vert_pad;

	if (!dialog_in_trans)
		child->render(child, xmin, ymin, active);

	dialog_get_transition();
}

int gg_dialog_input(gg_widget_t *widget, gg_event_t event) {
	gg_dialog_t *dialog = GG_DIALOG(widget);
	gg_widget_t *child = gg_bin_get_child(GG_BIN(widget));
	int x, y;

	if (dialog->flags & GG_DIALOG_HIDDEN)
		return 0;

	if (!dialog->modal && event.type == GG_EVENT_KEY && event.key == GG_KEY_ESCAPE)
		gg_dialog_close();

	if (!dialog->modal && event.type == GG_EVENT_MOUSE && event.mouse.type == GG_MOUSE_BUTTON_DOWN &&
		event.mouse.button == 2)
		gg_dialog_close();

	gg_dialog_get_screen_pos(dialog, &x, &y);

	if (event.type == GG_EVENT_MOUSE) {
		int size;

		event.mouse.x -= x;
		event.mouse.y -= y;

		gg_system_get_image_size(dialog->style.border.image[0], &size, NULL);

		if (event.mouse.type == GG_MOUSE_BUTTON_DOWN && event.mouse.button == 0) {
			if (dialog->title) {
				int text_height;
				int titlebar_height;
				gg_system_get_string_size(dialog->title, NULL, &text_height);
				titlebar_height = text_height * GG_DIALOG_TITLE_FACT;

				dialog->dialog_state |= GG_DIALOG_LEFT_BUTTON;

				/* Mouse click outside of dialog */
				if (event.mouse.y >= dialog->height || event.mouse.y < 0 || event.mouse.x >= dialog->width ||
					event.mouse.x < 0)
					return 0;

				gg_dialog_set_active(dialog);

				/* Title bar click */
				if (event.mouse.y >= dialog->height - size - titlebar_height && event.mouse.y < dialog->height - size &&
					event.mouse.x >= size && event.mouse.x < dialog->width - size) {
					dialog->dialog_state |= GG_DIALOG_MOVING;
					dialog->movement_org_x = event.mouse.x;
					dialog->movement_org_y = event.mouse.y;
					return 1;
				}
			}
		} else if (event.mouse.type == GG_MOUSE_BUTTON_UP && event.mouse.button == 0) {
			if (dialog->title) {
				dialog->dialog_state &= ~GG_DIALOG_LEFT_BUTTON;
				if (dialog->dialog_state & GG_DIALOG_MOVING) {
					dialog->dialog_state &= ~GG_DIALOG_MOVING;
					return 1;
				}
			}
		}

		if ((event.mouse.type == GG_MOUSE_MOVE) && (dialog->dialog_state & GG_DIALOG_MOVING)) {
			int xoff = dialog->width * dialog->pos.x_align;
			int yoff = dialog->height * dialog->pos.y_align;

			gg_dialog_set_position(dialog, x + event.mouse.x - dialog->movement_org_x + xoff,
								   y + event.mouse.y - dialog->movement_org_y + yoff, dialog->pos.x_align,
								   dialog->pos.y_align);
		}

		event.mouse.x -= size + dialog->style.hor_pad;
		event.mouse.y -= size + dialog->style.vert_pad;

		if (!(event.mouse.type == GG_MOUSE_MOVE && dialog->dialog_state & GG_DIALOG_LEFT_BUTTON)) {
			gg_widget_t *child = gg_bin_get_child(GG_BIN(dialog));

			if (event.mouse.x >= 0 && event.mouse.x < child->width_a && event.mouse.y >= 0 &&
				event.mouse.y < child->height_a)
				dialog->set_focus_pos(GG_WIDGET(dialog), event.mouse.x, event.mouse.y);
		}
	}

	if (child->input)
		return child->input(child, event);

	return 0;
}

void gg_dialog_input_current(gg_event_t event) {
	gg_dialog_t *dialog;

	TAILQ_FOREACH(dialog, &dialogs, entries)
    	if (!(dialog->flags & GG_DIALOG_HIDDEN) && gg_dialog_input(GG_WIDGET(dialog), event))
	    	break;
}

void gg_dialog_set_modal(gg_dialog_t *dialog, int modal) {
	dialog->modal = modal;
}

void gg_dialog_show(gg_dialog_t *dialog) {
	dialog->flags &= ~GG_DIALOG_HIDDEN;
}

void gg_dialog_hide(gg_dialog_t *dialog) {
	dialog->flags |= GG_DIALOG_HIDDEN;
}

void gg_dialog_set_position(gg_dialog_t *dialog, int x, int y, float x_align, float y_align) {
	dialog->pos.x = x;
	dialog->pos.y = y;
	dialog->pos.x_align = x_align;
	dialog->pos.y_align = y_align;
}

void gg_dialog_init(gg_dialog_t *dialog, gg_widget_t *child, char *title, gg_dialog_t *parent, int flags) {
	gg_bin_init((gg_bin_t *)dialog, child);

	dialog->input = gg_dialog_input;
	dialog->destroy = gg_dialog_destroy;
	dialog->id = gg_dialog_get_class_id();
	dialog->flags = flags;
	dialog->dialog_state = 0;
	dialog->parent_dialog = parent;
	dialog->modal = 0;

	if (title) {
		dialog->title = malloc(strlen(title) + 1);
		strcpy(dialog->title, title);
	} else
		dialog->title = NULL;

	child->get_requested_size(child, &dialog->width, &dialog->height);
	child->set_size(child, dialog->width, dialog->height);

	gg_dialog_set_position(dialog, gg_system_get_screen_width() / 2, gg_system_get_screen_height() / 2, 0.5f, 0.5f);

	//  dialog_reset_transition(1);
}

/** @brief Creates a dialog.
 *
 *  @param child The widget the dialog contains.
 *  @param title The title bar text, or NULL for no title bar.
 *  @return The created dialog.
 */
gg_widget_t *gg_dialog_create(gg_widget_t *child, char *title, gg_dialog_t *parent, int flags) {
	gg_dialog_t *dialog = malloc(sizeof(gg_dialog_t));

	gg_dialog_init(dialog, child, title, parent, flags);

	return GG_WIDGET(dialog);
}

void gg_dialog_destroy(gg_widget_t *widget) {
	gg_dialog_t *dialog = GG_DIALOG(widget);

	if (dialog->title)
		free(dialog->title);

	gg_container_destroy(widget);
}

void gg_dialog_set_style(gg_dialog_t *dialog, gg_dialog_style_t *style) {
	int size;
	gg_widget_t *child = gg_bin_get_child(GG_BIN(dialog));

	dialog->style = *style;
	dialog->width = child->width_a + 2 * dialog->style.hor_pad;
	dialog->height = child->height_a + 2 * dialog->style.vert_pad;

	gg_system_get_image_size(style->border.image[0], &size, NULL);
	dialog->width += 2 * size;
	dialog->height += 2 * size;

	if (dialog->title) {
		int text_height;
		gg_system_get_string_size(dialog->title, NULL, &text_height);
		dialog->height += text_height * GG_DIALOG_TITLE_FACT;
		dialog->height += GG_DIALOG_TITLE_SEP_HEIGHT;
	}
}

void gg_dialog_render_all(void) {
	gg_dialog_t *dialog;

	TAILQ_FOREACH_REVERSE(dialog, &dialogs, dialogs_head, entries)
    	if (!(dialog->flags & GG_DIALOG_HIDDEN))
	    	gg_dialog_render(dialog);
}
