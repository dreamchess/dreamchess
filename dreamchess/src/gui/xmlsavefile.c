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
#include "xml.h"
#include "i18n.h"

static int usedslots[SAVEGAME_SLOTS];
static char time_save[SAVEGAME_SLOTS][80];
static config_t config_save[SAVEGAME_SLOTS];
static board_t saved_board[SAVEGAME_SLOTS];

board_t *get_saved_board(int index) {
	return &saved_board[index];
}

config_t *get_config_save(int index) {
	return &config_save[index];
}

char *get_time_save(int index) {
	return time_save[index];
}

void set_slot_used_status(int slot, int status) {
 	usedslots[slot] = status;
}

int get_slot_used_status(int slot) {
 	return usedslots[slot];
}

int write_save_xml(int slot) {
	FILE *fp;
	char temp[80];
	char *fen;
	int retval = 0;

	snprintf(temp, sizeof(temp), "save%i.xml", slot);

	if (ch_userdir()) {
		DBG_WARN("Could not enter user directory");
		return -1;
	}

	fp = fopen(temp, "w");

	if (!fp)
		return -1;

	DBG_LOG("Writing save xml: %s", temp);

	fputs("<?xml version=\"1.0\"?>\n<save>\n", fp);
	fprintf(fp, "<time>%i</time>\n<white>", (int)time(NULL));

	if (get_config()->player[WHITE] == PLAYER_UI)
		fputs("ui", fp);
	else
		fputs("engine", fp);

	fputs("</white>\n<black>", fp);

	if (get_config()->player[BLACK] == PLAYER_UI)
		fputs("ui", fp);
	else
		fputs("engine", fp);

	fputs("</black>\n", fp);

	fprintf(fp, "<level>%i</level>\n", get_config()->cpu_level);
	fprintf(fp, "<difficulty>%i</difficulty>\n", get_config()->difficulty);

	fen = fen_encode(game_get_board());
	if (!fen) {
		DBG_ERROR("Error encoding FEN");
		retval = -1;
	} else {
		fprintf(fp, "<fen>%s</fen>\n", fen);
		free(fen);
	}

	fputs("</save>\n", fp);

	if (ferror(fp)) {
		DBG_ERROR("Error writing XML file for savegame in slot %d", slot);
		retval = -1;
	}

	fclose(fp);

	return retval;
}

static void save_cb(void *user_data, const char *element, char *const *attrs, const char *text) {
	int slot = *((int *)user_data);

	if (!strcmp(element, "time")) {
		time_t time;
		struct tm *tm;

		time = atoi(text);
		tm = localtime(&time);
		snprintf(time_save[slot], sizeof(time_save[slot]), "%04d-%02d-%02d %02d:%02d",
			tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min);
	} else if (!strcmp(element, "white")) {
		if (!strcmp(text, "ui"))
			config_save[slot].player[WHITE] = PLAYER_UI;
		else
			config_save[slot].player[WHITE] = PLAYER_ENGINE;
	} else if (!strcmp(element, "black")) {
		if (!strcmp(text, "ui"))
			config_save[slot].player[BLACK] = PLAYER_UI;
		else
			config_save[slot].player[BLACK] = PLAYER_ENGINE;
	} else if (!strcmp(element, "level")) {
		config_save[slot].cpu_level = atoi(text);
	} else if (!strcmp(element, "difficulty")) {
		config_save[slot].difficulty = atoi(text);
	} else if (!strcmp(element, "fen")) {
		board_t *board = fen_decode(text);
		if (board) {
			saved_board[slot] = *board;
			free(board);
		}
	}
}

void load_saves_xml(void) {
	int slot;

	for (slot = 0; slot < SAVEGAME_SLOTS; ++slot) {
		snprintf(time_save[slot], sizeof(time_save[slot]), _("Unknown"));
		config_save[slot].player[WHITE] = PLAYER_UI;
		config_save[slot].player[BLACK] = PLAYER_ENGINE;
		config_save[slot].cpu_level = 1;
		config_save[slot].difficulty = 1;
		board_setup(&saved_board[slot]);
		usedslots[slot] = 0;
	}

	if (ch_userdir()) {
		DBG_WARN("Could not enter user directory");
		return;
	}


	for (slot = 0; slot < SAVEGAME_SLOTS; ++slot) {
		char temp[256];
		snprintf(temp, sizeof(temp), "save%i.xml", slot);

		DBG_LOG("Reading save xml: %s", temp);

		if (!xml_parse(temp, "save", save_cb, NULL, NULL, &slot))
			usedslots[slot] = 1;
		else
			DBG_LOG("Failed to load: %s", temp);
	}
}
