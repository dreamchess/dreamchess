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

#include "config.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#elif defined(_MSC_VER)
#include "msvc/getopt.h"
#define HAVE_GETOPT_LONG
#endif /* HAVE_GETOPT_H */
#include <errno.h>
#include <SDL.h>

#include "audio.h"
#include "board.h"
#include "comm.h"
#include "debug.h"
#include "dir.h"
#include "dreamchess.h"
#include "git_rev.h"
#include "history.h"
#include "pgn_scanner.h"
#include "system_config.h"
#include "ui.h"
#include "i18n.h"

#ifdef __APPLE__
#include "CoreFoundation/CoreFoundation.h"
#endif

#ifdef HAVE_GETOPT_LONG
#define OPTION_TEXT(L, S) "  " L "\t  " S "\t%s\n"
#else
#define OPTION_TEXT(L, S) "  " S "\t%s\n"
#endif

typedef struct move_list {
	char **move;
	int entries, view, max_entries;
} move_list_t;

typedef struct cl_options {
	int width;
	int height;
	int fs;
	char *engine;
} cl_options_t;

static ui_driver_t *ui;
static config_t *config;
static move_list_t san_list, fan_list, fullalg_list;
static history_t *history;
static int in_game;
static int engine_error;

static void move_list_play(move_list_t *list, char *move) {
	if (list->entries == list->max_entries) {
		list->max_entries *= 2;
		list->move = realloc(list->move, list->max_entries * sizeof(char *));
	}
	list->move[list->entries++] = strdup(move);
	list->view = list->entries - 1;
}

static void move_list_undo(move_list_t *list) {
	if (list->entries > 0) {
		list->entries--;
		free(list->move[list->entries]);
		list->view = list->entries - 1;
	}
}

static void move_list_init(move_list_t *list) {
	list->max_entries = 20;
	list->move = malloc(list->max_entries * sizeof(char *));
	list->entries = 0;
	list->view = -1;
}

static void move_list_exit(move_list_t *list) {
	while (list->entries > 0)
		move_list_undo(list);
	free(list->move);
}

static void move_list_view_next(move_list_t *list) {
	if (list->view < list->entries - 1)
		list->view++;
}

static void move_list_view_prev(move_list_t *list) {
	if (list->view >= 0)
		list->view--;
}

board_t *game_get_board(void) {
	return history->last->board;
}

void game_view_next(void) {
	history_view_next(history);
	move_list_view_next(&fullalg_list);
	move_list_view_next(&san_list);
	move_list_view_next(&fan_list);
	ui->update(history->view->board, NULL);
}

void game_view_prev(void) {
	history_view_prev(history);
	move_list_view_prev(&fullalg_list);
	move_list_view_prev(&san_list);
	move_list_view_prev(&fan_list);
	ui->update(history->view->board, NULL);
}

void game_undo(void) {
	history_undo(history);
	move_list_undo(&fullalg_list);
	move_list_undo(&san_list);
	move_list_undo(&fan_list);
	if (history->result) {
		free(history->result->reason);
		free(history->result);
		history->result = NULL;
	}
	ui->update(history->view->board, NULL);
}

void game_retract_move(void) {
	/* Make sure a user is on move and we can undo two moves. */
	if (config->player[history->last->board->turn] != PLAYER_UI)
		return;
	if (!history->last->prev || !history->last->prev->prev)
		return;

	game_undo();
	game_undo();
	comm_send("remove\n");
}

void game_move_now(void) {
	/* Make sure engine is on move. */
	if (config->player[history->last->board->turn] != PLAYER_ENGINE)
		return;

	comm_send("?\n");
}

int game_want_move(void) {
	return config->player[history->last->board->turn] == PLAYER_UI && history->last == history->view;
}

int game_save(int slot) {
	int retval;
	char temp[80];

	if (!ch_userdir()) {
		snprintf(temp, sizeof(temp), "save%i.pgn", slot);
		retval = history_save_pgn(history, temp);
	} else {
		DBG_ERROR("Failed to enter user directory");
		retval = 1;
	}

	return retval;
}

void game_set_engine_error(int err) {
	engine_error = err;
}

int game_get_engine_error(void) {
	return engine_error;
}

static result_t *result_new(int code, const char *reason) {
	result_t *result = malloc(sizeof(result_t));

	result->code = code;
	result->reason = strdup(reason);

	return result;
}

static int do_move(move_t *move, int ui_update) {
	char *move_s, *move_f, *move_san;
	board_t new_board;

	if (!move_is_valid(history->last->board, move)) {
		DBG_WARN("Move is illegal");
		return 0;
	}

	if (history->result) {
		DBG_WARN("Game has already finished");
		return 0;
	}

	move_set_attr(history->last->board, move);
	new_board = *history->last->board;
	move_s = move_to_fullalg(&new_board, move);
	move_list_play(&fullalg_list, move_s);

	move_san = move_to_san(&new_board, move);
	move_f = san_to_fan(&new_board, move_san);

	DBG_LOG("Processing move %s (%s)", move_s, move_san);

	move_list_play(&san_list, move_san);
	move_list_play(&fan_list, move_f);

	free(move_san);
	free(move_f);
	free(move_s);

	make_move(&new_board, move);

	if (move->state == MOVE_CHECK)
		new_board.state = BOARD_CHECK;
	else if (move->state == MOVE_CHECKMATE)
		new_board.state = BOARD_CHECKMATE;
	else if (move->state == MOVE_STALEMATE)
		new_board.state = BOARD_STALEMATE;
	else
		new_board.state = BOARD_NORMAL;

	history_play(history, move, &new_board);

	if (ui_update)
		ui->update(history->view->board, move);

	if (new_board.state == BOARD_CHECKMATE) {
		if (new_board.turn == WHITE)
			history->result = result_new(RESULT_BLACK_WINS, "Black mates");
		else
			history->result = result_new(RESULT_WHITE_WINS, "White mates");
	} else if (new_board.state == BOARD_STALEMATE) {
		history->result = result_new(RESULT_DRAW, "Stalemate");
	} else if (history_has_threefold_rep(history)) {
		history->result = result_new(RESULT_DRAW, "Threefold repetition");
	} else if (new_board.halfmove_clock == 100) {
		history->result = result_new(RESULT_DRAW, "50-move rule");
	}

	if (history->result && ui_update)
		ui->show_result(history->result);

	return 1;
}

void game_make_move(move_t *move, int ui_update) {
	if (do_move(move, ui_update)) {
		comm_send("%s\n", fullalg_list.move[fullalg_list.entries - 1]);
	} else {
		char *move_str = move_to_fullalg(history->last->board, move);
		DBG_WARN("Ignoring illegal move %s", move_str);
		free(move_str);
	}
}

void game_quit(void) {
	in_game = 0;
}

int game_load(int slot) {
	int retval;
	char temp[80];
	board_t *board;

	if (ch_userdir()) {
		DBG_ERROR("Failed to enter user directory");
		return 1;
	}

	comm_send("force\n");

	snprintf(temp, sizeof(temp), "save%i.pgn", slot);
	retval = pgn_parse_file(temp);

	if (retval) {
		DBG_ERROR("Failed to parse PGN file '%s'", temp);
		return 1;
	}

	board = history->last->board;

	ui->update(board, NULL);

	if (config->player[board->turn] == PLAYER_ENGINE)
		comm_send("go\n");
	else if (config->player[OPPONENT(board->turn)] == PLAYER_ENGINE) {
		if (board->turn == WHITE)
			comm_send("white\n");
		else
			comm_send("black\n");
	}

	return retval;
}

void game_make_move_str(char *move_str, int ui_update) {
	board_t new_board = *history->last->board;
	move_t *engine_move;

	DBG_LOG("Parsing move string '%s'", move_str);

	engine_move = san_to_move(&new_board, move_str);

	if (!engine_move)
		engine_move = fullalg_to_move(&new_board, move_str);
	if (engine_move) {
		game_make_move(engine_move, ui_update);
		free(engine_move);
	} else
		DBG_ERROR("Failed to parse move string '%s'", move_str);
}

void game_get_move_list(char ***list, int *total, int *view) {
	*list = fan_list.move;
	*total = fan_list.entries;
	*view = fan_list.view;
}

int set_resolution(int init) {
	int width, height, fs, ms;
	option_t *option = config_get_option("resolution");
	config_resolution_t *res = option->selected->data;

	option = config_get_option("full_screen");
	fs = option->selected->index;

	option = config_get_option("multisampling");

	ms = option->selected->index * 2;

	if (res) {
		width = res->w;
		height = res->h;
	} else {
		/* Custom */
		option = config_get_option("custom_resolution_width");
		width = option->value;
		option = config_get_option("custom_resolution_height");
		height = option->value;
	}

	if (init)
		return ui->create_window(width, height, fs, ms);
	else
		return ui->resize(width, height, fs, ms);
}

static void init_resolution(void) {
	if (set_resolution(1)) {
		DBG_LOG("Switching to failsafe video mode defaults");
		config_set_failsafe_video();
		config_save();

		if (set_resolution(1)) {
			exit(1);
		}
	}
}

void toggle_fullscreen(void) {
	option_t *option = config_get_option("full_screen");
	option_select_value_by_index(option, 1 - option->selected->index);
	set_resolution(0);
}

static void parse_options(int argc, char **argv, ui_driver_t **ui_driver, cl_options_t *cl_options) {
	int c;

#ifdef HAVE_GETOPT_LONG

	int optindex;

	struct option options[] = {{"help", no_argument, NULL, 'h'},
							   {"fullscreen", no_argument, NULL, 'f'},
							   {"width", required_argument, NULL, 'W'},
							   {"height", required_argument, NULL, 'H'},
							   {"1st-engine", required_argument, NULL, '1'},
							   {0, 0, 0, 0}};

	/* On macOS (under certain circumstances) a process serial number will be passed in. In this
	 * case we skip parsing the command line options.
	 */ 
	if (argc > 1 && strncmp(argv[1], "-psn_", 5) == 0) {
		DBG_WARN("Received '%s'; ignoring all command line arguments", argv[1]);
		return;
	}

	while ((c = getopt_long(argc, argv, "1:fhW:H:", options, &optindex)) > -1) {
#else

	while ((c = getopt(argc, argv, "1:fhW:H:")) > -1) {
#endif /* HAVE_GETOPT_LONG */
		switch (c) {
		case 'h':
			printf(_("Usage: dreamchess [options]\n\n"
			         "An xboard-compatible chess interface.\n\n"
			         "Options:\n"));
			printf(OPTION_TEXT("--help\t", "-h\t"), _("show help"));
			printf(OPTION_TEXT("--fullscreen\t", "-f\t"), _("run fullscreen"));
			printf(OPTION_TEXT("--width\t", "-W<num>"), _("set screen width"));
   			printf(OPTION_TEXT("--height\t", "-H<num>"), _("set screen height"));
   			printf(OPTION_TEXT("--1st-engine <eng>", "-1<eng>"), _("use <eng> as first chess engine"));
			printf(OPTION_TEXT("\t\t", "\t"), _("  defaults to 'dreamer'"));
			exit(0);
		case '1':
			cl_options->engine = optarg;
			break;
		case 'f':
			cl_options->fs = 1;
			break;
		case 'W':
			cl_options->width = atoi(optarg);
			break;
		case 'H':
			cl_options->height = atoi(optarg);
		}
	}
}

static void set_cl_options(cl_options_t *cl_options) {
	option_t *option;

	if (cl_options->engine) {
		option = config_get_option("first_engine");
		option_string_set_text(option, cl_options->engine);
	}

	if (cl_options->fs) {
		option = config_get_option("full_screen");
		option_select_value_by_name(option, "On");
	}

	if (cl_options->width) {
		option = config_get_option("custom_resolution_width");
		option->value = cl_options->width;
		option = config_get_option("resolution");
		option_select_value_by_name(option, "Custom");
	}

	if (cl_options->height) {
		option = config_get_option("custom_resolution_height");
		option->value = cl_options->height;
		option = config_get_option("resolution");
		option_select_value_by_name(option, "Custom");
	}
}

int main(int argc, char **argv) {
	cl_options_t cl_options = {0};

	dbg_init();
	init_i18n();
	DBG_LOG("Version %s", g_version);

	ui = &ui_sdlgl;

	printf("DreamChess %s\n", g_version);

	parse_options(argc, argv, &ui, &cl_options);

	// Switch to UTF-8 for the GUI
	bind_textdomain_codeset("dreamchess", "UTF-8");

	config_init();
	set_cl_options(&cl_options);

	if (!ui) {
		DBG_ERROR("Failed to find a user interface driver");
		exit(1);
	}

	ui->init();

	init_resolution();

	while (1) {
		board_t board;
		int pgn_slot;
		option_t *option;

		if (!(config = ui->config(&pgn_slot)))
			break;

		ch_userdir();
		option = config_get_option("first_engine");

#ifdef __APPLE__
		char temp1[200];
		char temp2[200];

		if (!strcmp(option->string, "dreamer")) {
			CFBundleRef mainBundle = CFBundleGetMainBundle();

			CFURLRef bundledir = CFBundleCopyBundleURL(mainBundle);
			CFStringRef stringref = CFURLCopyFileSystemPath(bundledir, kCFURLPOSIXPathStyle);
			CFStringGetCString(stringref, temp1, 200, kCFStringEncodingMacRoman);

			snprintf(temp2, sizeof(temp2), "%s/contents/MacOS/dreamer", temp1);

			game_set_engine_error(comm_init(temp2));
		} else
			game_set_engine_error(comm_init(option->string));
#else
		game_set_engine_error(comm_init(option->string));
#endif

		comm_send("xboard\n");

		comm_send("new\n");
		comm_send("random\n");

		comm_send("sd %i\n", config->cpu_level);

		// If we appear to be talking to gnuchess, send the "depth" command,
		// since "sd" is not supported.
		if (strstr(option->string, "gnuchess"))
			comm_send("depth %i\n", config->cpu_level);

		// On easy mode, send the "noquiesce" command to dreamer. This does not
		// work on other engines.
		if (!strcmp(option->string, "dreamer") && config->difficulty == 0)
			comm_send("noquiesce\n");

		if (config->player[WHITE] == PLAYER_UI && config->player[BLACK] == PLAYER_UI)
			comm_send("force\n");

		if (config->player[WHITE] == PLAYER_ENGINE)
			comm_send("go\n");

		in_game = 1;
		board_setup(&board);
		history = history_init(&board);
		move_list_init(&san_list);
		move_list_init(&fan_list);
		move_list_init(&fullalg_list);

		if (pgn_slot >= 0)
			if (game_load(pgn_slot)) {
				DBG_ERROR("Failed to load savegame in slot %i", pgn_slot);
				exit(1);
			}

		ui->update(history->view->board, NULL);

		if (history->result)
			ui->show_result(history->result);

		while (in_game) {
			char *s;

			if ((s = comm_poll())) {
				DBG_LOG("Message from engine: '%s'", s);
				if (!history->result) {
					if ((!strncmp(s, "move ", 4) || strstr(s, "... ")) &&
						config->player[history->last->board->turn] == PLAYER_ENGINE) {
						char *move_str = strrchr(s, ' ') + 1;
						board_t new_board = *history->last->board;
						move_t *engine_move;

						DBG_LOG("Parsing move string '%s'", move_str);

						engine_move = san_to_move(&new_board, move_str);
						if (!engine_move)
							engine_move = fullalg_to_move(&new_board, move_str);
						if (engine_move) {
							audio_play_sound(AUDIO_MOVE);
							do_move(engine_move, 1);
							free(engine_move);
						} else
							DBG_ERROR("Failed to parse move string '%s'", move_str);
					} else if (strstr(s, "llegal move"))
						game_undo();
					/* Ignore result message if we've already determined a result ourselves. */
					else {
						char *start = strchr(s, '{');
						char *end = strchr(s, '}');

						if (start && end && end > start) {
							char *comment = malloc(end - start);
							history->result = malloc(sizeof(result_t));
							strncpy(comment, start + 1, end - start - 1);
							comment[end - start - 1] = '\0';
							history->result->reason = comment;
							if (strstr(s, "1-0")) {
								history->result->code = RESULT_WHITE_WINS;
								ui->show_result(history->result);
							} else if (strstr(s, "1/2-1/2")) {
								history->result->code = RESULT_DRAW;
								ui->show_result(history->result);
							} else if (strstr(s, "0-1")) {
								history->result->code = RESULT_BLACK_WINS;
								ui->show_result(history->result);
							} else {
								free(history->result->reason);
								free(history->result);
								history->result = NULL;
							}
						}
					}
				}

				free(s);
			}
			ui->poll();
		}
		comm_send("quit\n");
		comm_exit();
		history_exit(history);
		move_list_exit(&san_list);
		move_list_exit(&fan_list);
		move_list_exit(&fullalg_list);
	}
	ui->exit();
	dbg_exit();
	return 0;
}
