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

#include "ui_sdlgl.h"

static int dialog_close_cb(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data)
{
	gg_dialog_close();
	return 1;
}

gg_dialog_t *dialog_victory_create(result_t *result)
{
	gg_widget_t *dialog;
	gg_widget_t *hbox = gg_hbox_create(20);
	gg_widget_t *vbox = gg_vbox_create(0);
	gg_widget_t *image_l, *image_r;
	gg_widget_t *action;
	gg_widget_t *text;

	switch (result->code) {
	case RESULT_WHITE_WINS:
		image_l = gg_image_create(get_white_piece(GUI_PIECE_KING));
		image_r = gg_image_create(get_white_piece(GUI_PIECE_QUEEN));
		text = gg_label_create("White won the match!");
		break;

	case RESULT_BLACK_WINS:
		image_l = gg_image_create(get_black_piece(GUI_PIECE_KING));
		image_r = gg_image_create(get_black_piece(GUI_PIECE_QUEEN));
		text = gg_label_create("Black won the match!");
		break;

	default:
		image_l = gg_image_create(get_white_piece(GUI_PIECE_KING));
		image_r = gg_image_create(get_black_piece(GUI_PIECE_KING));
		text = gg_label_create("The game ended in a draw!");
	}

	gg_container_append(GG_CONTAINER(vbox), text);
	text = gg_label_create(result->reason);
	gg_container_append(GG_CONTAINER(vbox), text);
	text = gg_label_create("");
	gg_container_append(GG_CONTAINER(vbox), text);
	action = gg_action_create_with_label("Ok", 0.5f, 0.5f);
	gg_widget_subscribe_signal_name(action, action->id, "action_pressed", dialog_close_cb, NULL);
	gg_container_append(GG_CONTAINER(vbox), action);
	gg_container_append(GG_CONTAINER(hbox), image_l);
	gg_container_append(GG_CONTAINER(hbox), vbox);
	gg_container_append(GG_CONTAINER(hbox), image_r);
	dialog = gg_dialog_create(hbox, NULL, NULL, 0);
	gg_dialog_set_modal(GG_DIALOG(dialog), 1);
	gg_dialog_set_style(GG_DIALOG(dialog), get_ingame_style());
	return GG_DIALOG(dialog);
}
