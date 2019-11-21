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

#include <stdlib.h>
#include <string.h>

#include "audio.h"
#include "dialogs.h"
#include "dreamchess.h"
#include "options.h"
#include "system_config.h"
#include "i18n.h"

static gg_widget_t *entry;

static int dialog_close_cb(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data) {
	option_t *option = config_get_option("first_engine");
	free(option->string);
	option->string = strdup(gg_entry_get_text(GG_ENTRY(entry)));
	config_save();
	gg_dialog_close();
	return 1;
}

static int dialog_title_theme(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data) {
	option_t *option = config_get_option("theme");
	option_select_value_by_index(option, gg_option_get_selected(GG_OPTION(widget)));
	return 1;
}

static int dialog_title_lettering(gg_widget_t *widget, gg_widget_t *emitter, void* data, void *extra_data) {
	option_t *option = config_get_option("lettering");
	option_select_value_by_index(option, gg_option_get_selected(GG_OPTION(widget)));
	return 1;
}

static int dialog_title_music_vol(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data) {
	option_t *option = config_get_option("music_volume");
	option_select_value_by_index(option, gg_option_get_selected(GG_OPTION(widget)));

	audio_set_music_volume(option->selected->index);

	return 1;
}

static int dialog_title_sound_vol(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data) {
	option_t *option = config_get_option("sound_volume");
	option_select_value_by_index(option, gg_option_get_selected(GG_OPTION(widget)));

	audio_set_sound_volume(option->selected->index);
	audio_play_sound(AUDIO_MOVE);

	return 1;
}

static int dialog_title_resolution_load(gg_widget_t *widget, gg_widget_t *emitter, void *data, void *extra_data) {
	gg_dialog_open(dialog_resolution_create(gg_widget_find_dialog(widget)));
	return 1;
}

static void create_option_values(gg_option_t *widget, option_t *option) {
	option_value_t *value;

	OPTION_VALUE_FOREACH(value, option) {
		gg_option_append_label(widget, value->name, 0.5f, 0.0f);
	}

	gg_option_set_selected(widget, option->selected->index);
}

gg_dialog_t *dialog_systemopts_create(gg_dialog_t *parent) {
	gg_widget_t *dialog;
	gg_widget_t *vbox, *vbox2, *hbox;
	gg_widget_t *widget;
	option_t *option;

	vbox = gg_vbox_create(0);
	vbox2 = gg_vbox_create(2);

	widget = gg_label_create(_("Theme:"));
	gg_align_set_alignment(GG_ALIGN(widget), 0.0f, 0.0f);
	gg_container_append(GG_CONTAINER(vbox2), widget);

	widget = gg_label_create(_("Board Lettering:"));
	gg_align_set_alignment(GG_ALIGN(widget), 0.0f, 0.0f);
	gg_container_append(GG_CONTAINER(vbox2), widget);

	widget = gg_label_create(_("Engine:"));
	gg_align_set_alignment(GG_ALIGN(widget), 0.0f, 0.0f);
	gg_container_append(GG_CONTAINER(vbox2), widget);

	widget = gg_label_create(_("Music Volume:"));
	gg_align_set_alignment(GG_ALIGN(widget), 0.0f, 0.0f);
	gg_container_append(GG_CONTAINER(vbox2), widget);

	widget = gg_label_create(_("Sound Volume:"));
	gg_align_set_alignment(GG_ALIGN(widget), 0.0f, 0.0f);
	gg_container_append(GG_CONTAINER(vbox2), widget);

	hbox = gg_hbox_create(20);
	gg_container_append(GG_CONTAINER(hbox), vbox2);
	vbox2 = gg_vbox_create(2);

	option = config_get_option("theme");
	widget = gg_option_create();
	create_option_values(GG_OPTION(widget), option);
	gg_widget_subscribe_signal_name(widget, widget->id, "option_changed", dialog_title_theme, NULL);
	gg_container_append(GG_CONTAINER(vbox2), widget);

	option = config_get_option("lettering");
	widget = gg_option_create();
	create_option_values(GG_OPTION(widget), option);
	gg_widget_subscribe_signal_name(widget, widget->id, "option_changed", dialog_title_lettering, NULL);
	gg_container_append(GG_CONTAINER(vbox2), widget);
	
	option = config_get_option("first_engine");
	entry = gg_entry_create(100);
	gg_entry_set_text(GG_ENTRY(entry), option->string);
	gg_container_append(GG_CONTAINER(vbox2), entry);

	option = config_get_option("music_volume");
	widget = gg_option_create();
	create_option_values(GG_OPTION(widget), option);
	gg_widget_subscribe_signal_name(widget, widget->id, "option_changed", dialog_title_music_vol, NULL);
	gg_container_append(GG_CONTAINER(vbox2), widget);

	option = config_get_option("sound_volume");
	widget = gg_option_create();
	create_option_values(GG_OPTION(widget), option);
	gg_widget_subscribe_signal_name(widget, widget->id, "option_changed", dialog_title_sound_vol, NULL);
	gg_container_append(GG_CONTAINER(vbox2), widget);

	gg_container_append(GG_CONTAINER(hbox), vbox2);
	gg_container_append(GG_CONTAINER(vbox), hbox);

	widget = gg_action_create_with_label(_("Change resolution…"), 0.0f, 0.0f);
	gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed", dialog_title_resolution_load, NULL);
	gg_container_append(GG_CONTAINER(vbox), widget);

	widget = gg_action_create_with_label(_("OK"), 0.5f, 0.0f);
	gg_widget_subscribe_signal_name(widget, widget->id, "action_pressed", dialog_close_cb, NULL);
	gg_container_append(GG_CONTAINER(vbox), widget);

	dialog = gg_dialog_create(vbox, NULL, parent, GG_DIALOG_AUTOHIDE_PARENT);
	gg_dialog_set_modal(GG_DIALOG(dialog), 1);
	gg_dialog_set_position(GG_DIALOG(dialog), gg_system_get_screen_width() / 2, 23, 0.5f, 0.0f);
	gg_dialog_set_style(GG_DIALOG(dialog), get_menu_style());

	return GG_DIALOG(dialog);
}
