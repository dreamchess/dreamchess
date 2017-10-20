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

#include "DreamChess.h"
#include "System.h"
#include "ResourcePool.h"
#include "TitleScene.h"

int DreamChess::init(LaunchArguments *args) {
    _titleScene = new TitleScene(this);
    _titleScene->init();

    _currentScene = _titleScene;

    return true;
}

/*
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#elif defined(_MSC_VER)
#include "msvc/getopt.h"
#endif // HAVE_GETOPT_H 
#include <errno.h>

#include "ChessBoard.h"
#include "History.h"
//#include "ui.h"
//#include "comm.h"
//#include "dir.h"
#include "DreamChess.h"
//#include "debug.h"
#include "git_rev.h"
#include "audio.h"
//#include "system_config.h"
#include "pgn_scanner.h"

#include "GameConfig.h"

#ifdef __APPLE__
#include "CoreFoundation/CoreFoundation.h"
#endif

#ifdef HAVE_GETOPT_LONG
#define OPTION_TEXT(L, S, T) "  " L "\t" S "\t" T "\n"
#else
#define OPTION_TEXT(L, S, T) "  " S "\t" T "\n"
#endif

#include "moveList.h"

//static ui_driver_t *ui;
static MoveList *san_list, *fan_list, *fullalg_list;
static int in_game;
static int engine_error;

void DreamChess::gameViewNext() {
    _history->viewNext();
    fullalg_list->viewNext();
    san_list->viewNext();
    fan_list->viewNext();
    //ui->update(_history->view->board, NULL);
}

void DreamChess::gameViewPrev(void) {
    _history->viewPrev();
    fullalg_list->viewPrev();
    san_list->viewPrev();
    fan_list->viewPrev();
    //ui->update(_history->view->board, NULL);
}

void DreamChess::gameUndo(void) {
    _history->undo();
    fullalg_list->undo();
    san_list->undo();
    fan_list->undo();
    if (_history->result) {
        free(_history->result->reason);
        free(_history->result);
        _history->result = NULL;
    }
    //ui->update(_history->view->board, NULL);
}

void DreamChess::gameRetractMove(void) {
    // Make sure a user is on move and we can undo two moves. 
    if (_config->player[_history->last->board->turn] != PLAYER_UI)
        return;
    if (!_history->last->prev || !_history->last->prev->prev)
        return;

    gameUndo();
    gameUndo();
    comm_send("remove\n");
}

void DreamChess::gameMoveNow(void) {
    // Make sure engine is on move. 
    if (_config->player[_history->last->board->turn] != PLAYER_ENGINE)
        return;

    comm_send("?\n");
}

int DreamChess::gameWantMove(void) {
    return _config->player[_history->last->board->turn] == PLAYER_UI
           && _history->last == _history->view;
}

int DreamChess::gameSave( int slot ) {
    int retval;
    char temp[80];

    if (!ch_userdir())
    {
        sprintf( temp, "save%i.pgn", slot );
        retval = _history->savePGN(temp);
    }
    else
    {
        DBG_ERROR("failed to enter user directory");
        retval = 1;
    }

    return retval;
}

void DreamChess::gameSetEngineError(int err) {
    engine_error = err;
}

int DreamChess::gameGetEngineError(void) {
    return engine_error;
}

int DreamChess::doMove(move_t *move, int ui_update) {
    char *move_s, *move_f, *move_san;
    Board new_board;

    if (!_history->last->board->moveIsValid(move)) {
        DBG_WARN("move is illegal");
        return 0;
    }

    _history->last->board->moveSetAttr(move);
    new_board = *_history->last->board;
    move_s = new_board.moveToFullalg(move);
    fullalg_list->play(move_s);

    move_san = new_board.moveToSan(move);
    move_f = new_board.sanToFan(move_san);

    DBG_LOG("processing move %s (%s)", move_s, move_san);

    san_list->play(move_san);
    fan_list->play(move_f);

    free(move_san);
    free(move_f);
    free(move_s);

    new_board.makeMove(move);

    if (move->state == MOVE_CHECK)
        new_board.state = BOARD_CHECK;
    else if (move->state == MOVE_CHECKMATE)
        new_board.state = BOARD_CHECKMATE;
    else
        new_board.state = BOARD_NORMAL;

    _history->play(move, &new_board);

    if (ui_update)
        ui->update(_history->view->board, move);

    if (new_board.state == MOVE_CHECKMATE) {
        _history->result = (result_t *)malloc(sizeof(result_t));

        if (new_board.turn == WHITE) {
            _history->result->code = RESULT_BLACK_WINS;
            _history->result->reason = strdup("Black mates");
        }
        else {
            _history->result->code = RESULT_WHITE_WINS;
            _history->result->reason = strdup("White mates");
        }

        if (ui_update)
            ui->show_result(_history->result);
    }
    else if (new_board.state == MOVE_STALEMATE) {
        _history->result = (result_t *)malloc(sizeof(result_t));

        _history->result->code = RESULT_DRAW;
        _history->result->reason = strdup("Stalemate");

        if (ui_update)
            ui->show_result(_history->result);
    }

    return 1;
}

void DreamChess::gameMakeMove(move_t *move, int ui_update) {
    if (doMove(move, ui_update)) {
        comm_send("%s\n", fullalg_list->move[fullalg_list->entries-1]);
    }
    else {
        char *move_str = _history->last->board->moveToFullalg(move);
        DBG_WARN("ignoring illegal move %s", move_str);
        free(move_str);
    }
}

void DreamChess::gameQuit(void) {
    in_game = 0;
}

int DreamChess::gameLoad( int slot ) {
    int retval;
    char temp[80];
    Board *board;

    if (ch_userdir())
    {
        DBG_ERROR("failed to enter user directory");
        return 1;
    }

    comm_send("force\n");

    sprintf( temp, "save%i.pgn", slot );
    retval = pgn_parse_file(temp);

    if (retval)
    {
        DBG_ERROR("failed to parse PGN file '%s'", temp);
        return 1;
    }

    board = _history->last->board;

    ui->update(board, NULL);

    if (_config->player[board->turn] == PLAYER_ENGINE)
        comm_send("go\n");
    else if (_config->player[OPPONENT(board->turn)] == PLAYER_ENGINE)
    {
        if (board->turn == WHITE)
            comm_send("white\n");
        else
            comm_send("black\n");
    }

    return retval;
}

void game_make_move_str(char *move_str, int ui_update) {
    return g_DreamChess->gameMakeMoveStr(move_str, ui_update);
}

void DreamChess::gameMakeMoveStr(char *move_str, int ui_update) {
    Board new_board = *_history->last->board;
    move_t *engine_move;

    DBG_LOG("parsing move string '%s'", move_str);

    engine_move = new_board.sanToMove(move_str);

    if (!engine_move)
        engine_move = new_board.fullalgToMove(move_str);
    if (engine_move)
    {
        gameMakeMove(engine_move, ui_update);
        free(engine_move);
    }
    else
        DBG_ERROR("failed to parse move string '%s'", move_str);
}

void DreamChess::gameGetMoveList(char ***list, int *total, int *view) {
    *list = fan_list->move;
    *total = fan_list->entries;
    *view = fan_list->view;
}

int DreamChess::setResolution(int init) {
    int width, height, fs, ms;
    option_t *option = config_get_option("resolution");
    config_resolution_t *res = (config_resolution_t *)option->selected->data;

    option = config_get_option("full_screen");
    fs = option->selected->index;

    option = config_get_option("multisampling");

    ms = option->selected->index * 2;
    
    if (res) {
        width = res->w;
        height = res->h;
    }
    else {
        // Custom 
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

void DreamChess::initResolution() {
    if (setResolution(1)) {
        DBG_LOG("switching to failsafe video mode defaults");
        config_set_failsafe_video();
        config_save();

        if (setResolution(1)) {
            exit(1);
        }
    }
}

void DreamChess::toggleFullscreen() {
    option_t *option = config_get_option("full_screen");
    option_select_value_by_index(option, 1 - option->selected->index);
    setResolution(0);
}

DreamChess::DreamChess() {

}

DreamChess::~DreamChess() {
    delete _history;
}

int DreamChess::init(LaunchArguments *arg)
{
    _config = new GameConfig();
    GameConfig->parseOptions(arg);

    //cl_options_t cl_options = { 0 };
    ui = ui_driver[0];

    printf( "DreamChess %s\n", g_version );

    //parse_options(arg->argc, arg->argv, &ui, &cl_options);
    //config_init();
    //set_cl_options(&cl_options);

    if (!ui)
    {
        DBG_ERROR("failed to find a user interface driver");
        exit(1);
    }

    ui->init();

    initResolution();

    while (1)
    {
        Board board;
        int pgn_slot;
        option_t *option;

        if (!(_config = ui->config(&pgn_slot)))
            break;

        ch_userdir();
        option = config_get_option("first_engine");

#ifdef __APPLE__
		char temp1[200];
		char temp2[200];
				
		if (!strcmp(option->string,"dreamer") || !strcmp(option->string,"Dreamer") )
		{
			CFBundleRef mainBundle = CFBundleGetMainBundle();

			CFURLRef bundledir=CFBundleCopyBundleURL(mainBundle);
			CFStringRef stringref=CFURLCopyFileSystemPath( bundledir, kCFURLPOSIXPathStyle );	
			CFStringGetCString ( stringref, temp1, 200, kCFStringEncodingMacRoman);
	
			sprintf( temp2, "%s/contents/MacOS/dreamer", temp1 );
		
			game_set_engine_error(comm_init(temp2));
		}
		else
		gameSetEngineError(comm_init(option->string));		
#else
		gameSetEngineError(comm_init(option->string));
#endif

        comm_send("xboard\n");

        comm_send("new\n");
        comm_send("random\n");

        comm_send("sd %i\n", _config->cpu_level);
        comm_send("depth %i\n", _config->cpu_level);

        if (_config->difficulty == 0)
	    comm_send("noquiesce\n");

        if (_config->player[WHITE] == PLAYER_UI
	    && _config->player[BLACK] == PLAYER_UI)
	    comm_send("force\n");

        if (_config->player[WHITE] == PLAYER_ENGINE)
	    comm_send("go\n");

        in_game = 1;
        board.boardSetup();
        _history = new History(&board);
        san_list = new MoveList();
        fan_list = new MoveList();
        fullalg_list = new MoveList();

        if (pgn_slot >= 0)
            if (gameLoad(pgn_slot))
            {
                 DBG_ERROR("failed to load savegame in slot %i", pgn_slot);
                 exit(1);
            }

        ui->update(_history->view->board, NULL);
        while (in_game)
        {
            char *s;

            if ((s = comm_poll()))
            {
                DBG_LOG("message from engine: '%s'", s);
                if  (!_history->result)
                {
                    if ((!strncmp(s, "move ", 4) || strstr(s, "... ")) && _config->player[_history->last->board->turn] == PLAYER_ENGINE)
                    {
                        char *move_str = strrchr(s, ' ') + 1;
                        Board new_board = *_history->last->board;
                        move_t *engine_move;

                        DBG_LOG("parsing move string '%s'", move_str);

                        engine_move = new_board.sanToMove(move_str);
                        if (!engine_move)
                            engine_move = new_board.fullalgToMove(move_str);
                        if (engine_move)
                        {
                            audio_play_sound(AUDIO_MOVE);
                            do_move(engine_move, 1);
                            free(engine_move);
                        }
                        else
                            DBG_ERROR("failed to parse move string '%s'", move_str);
                    }
                    else if (strstr(s, "llegal move"))
                        gameUndo();
                    // Ignore result message if we've already determined a result ourselves. 
                    else
                    {
                        char *start = strchr(s, '{');
                        char *end = strchr(s, '}');

                        if (start && end && end > start)
                        {
                            char *comment = (char *)malloc(end - start);
                            _history->result = (result_t *)malloc(sizeof(result_t));
                            strncpy(comment, start + 1, end - start - 1);
                            comment[end - start - 1] = '\0';
                            _history->result->reason = comment;
                            if (strstr(s, "1-0"))
                            {
                                _history->result->code = RESULT_WHITE_WINS;
                                ui->show_result(_history->result);
                            }
                            else if (strstr(s, "1/2-1/2"))
                            {
                                _history->result->code = RESULT_DRAW;
                                ui->show_result(_history->result);
                            }
                            else if (strstr(s, "0-1"))
                            {
                                _history->result->code = RESULT_BLACK_WINS;
                                ui->show_result(_history->result);
                            }
                            else
                            {
                                free(_history->result->reason);
                                free(_history->result);
                                _history->result = NULL;
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
        delete san_list;
        delete fan_list;
        delete fullalg_list;
    }
    ui->exit();
    return 0;
}
*/