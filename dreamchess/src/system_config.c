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

#include "audio.h"
#include "options.h"
#include "system_config.h"
#include "theme.h"
#include "i18n.h"

static option_group_t *config;

static void add_resolution(option_t *option, int w, int h) {
	config_resolution_t *res;
	char str[10];

	res = malloc(sizeof(config_resolution_t));
	res->w = w;
	res->h = h;
	if (snprintf(str, 10, "%dx%d", w, h) > 0)
		option_add_value(option, str, res);
	else
		free(res);
}

void config_init(void) {
	option_t *option;
	int i;

	config = option_group_create("options");

	theme_find_music_packs();
	option = option_group_add_option(config, "theme");
	theme_find_themes(option);

	option = option_group_add_option(config, "lettering");
	option_add_value(option, _N("Off"), NULL);
	option_add_value(option, _N("On"), NULL);
	option_select_value_by_name(option, _N("Off"));

	option = option_group_add_option(config, "music_volume");
	option_add_value(option, _N("Off"), NULL);
	for (i = 1; i <= AUDIO_MAX_VOL; i++) {
		char buf[3];

		snprintf(buf, 3, "%d", i);
		option_add_value(option, buf, NULL);
	}
	option_select_value_by_name(option, "8");

	option = option_group_add_option(config, "sound_volume");
	option_add_value(option, _N("Off"), NULL);
	for (i = 1; i <= AUDIO_MAX_VOL; i++) {
		char buf[3];

		snprintf(buf, 3, "%d", i);
		option_add_value(option, buf, NULL);
	}
	option_select_value_by_name(option, "8");

	option = option_group_add_option(config, "time_moves");
	option_add_value(option, "Whole game", NULL);
	option_add_value(option, "10", NULL);
	option_add_value(option, "20", NULL);
	option_add_value(option, "30", NULL);
	option_add_value(option, "40", NULL);
	option_add_value(option, "Custom", NULL);

	option = option_group_add_int(config, "custom_time_moves");
	option->value = 40;

	option = option_group_add_option(config, "time_time");
	option_add_value(option, "20", NULL);
	option_add_value(option, "30", NULL);
	option_add_value(option, "60", NULL);
	option_add_value(option, "120", NULL);
	option_add_value(option, "Custom", NULL);

	option = option_group_add_int(config, "custom_time_time");
	option->value = 60;

	option = option_group_add_option(config, "time_increment");
	option_add_value(option, "0", NULL);
	option_add_value(option, "10", NULL);
	option_add_value(option, "30", NULL);
	option_add_value(option, "60", NULL);
	option_add_value(option, "Custom", NULL);

	option = option_group_add_int(config, "custom_time_increment");
	option->value = 0;

	option = option_group_add_option(config, "resolution");
	add_resolution(option, 640, 480);
	add_resolution(option, 800, 600);
	add_resolution(option, 1024, 768);
	add_resolution(option, 1280, 1024);
	add_resolution(option, 1280, 720);
	add_resolution(option, 1920, 1080);
	add_resolution(option, 3840, 2160);
	option_add_value(option, _N("Custom"), NULL);

	option = option_group_add_option(config, "full_screen");
	option_add_value(option, _N("Off"), NULL);
	option_add_value(option, _N("On"), NULL);

	option = option_group_add_int(config, "custom_resolution_width");
	option->value = 640;
	option = option_group_add_int(config, "custom_resolution_height");
	option->value = 480;

	option = option_group_add_option(config, "multisampling");
	option_add_value(option, _N("Off"), NULL);
	option_add_value(option, "2x", NULL);
	option_add_value(option, "4x", NULL);
	option_add_value(option, "6x", NULL);
	option_add_value(option, "8x", NULL);

	option = option_group_add_string(config, "first_engine");
	option->string = strdup("dreamer");

	option = option_group_add_option(config, "game_mode");
	option_add_value(option, _N("Human vs. CPU"), NULL);
	option_add_value(option, _N("CPU vs. Human"), NULL);
	option_add_value(option, _N("Human vs. Human"), NULL);

	option = option_group_add_option(config, "difficulty");
	option_add_value(option, _N("Easy"), NULL);
	option_add_value(option, _N("Normal"), NULL);
	option_select_value_by_name(option, "Normal");

	option = option_group_add_option(config, "level");
	for (i = 1; i <= 8; i++) {
		char buf[2];

		snprintf(buf, 2, "%d", i);
		option_add_value(option, buf, NULL);
	}

	config_load();
}

void config_set_failsafe_video(void) {
	option_t *option = option_group_find_option(config, "resolution");
	option_select_value_by_name(option, "640x480");

	option = option_group_find_option(config, "full_screen");
	option_select_value_by_name(option, _N("Off"));

	option = option_group_find_option(config, "custom_resolution_width");
	option->value = 640;
	option = option_group_find_option(config, "custom_resolution_height");
	option->value = 480;

	option = option_group_find_option(config, "multisampling");
	option_select_value_by_name(option, _N("Off"));
}

int config_save(void) {
	return option_group_save_xml(config);
}

int config_load(void) {
	return option_group_load_xml(config);
}

option_t *config_get_option(char *name) {
	return option_group_find_option(config, name);
}
