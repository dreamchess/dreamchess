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

#ifndef DREAMCHESS_DREAMCHESS_H
#define DREAMCHESS_DREAMCHESS_H

#include "board.h"

#define PLAYER_UI 0
#define PLAYER_ENGINE 1

class Dreamchess {
public:
	Dreamchess();
	~Dreamchess();

	int init(void *data);

	void gameViewNext(void);
	void gameViewPrev(void);
	void gameUndo(void);
	int gameWantMove(void);
	void gameMakeMove(move_t *move, int ui_update);
	void gameRetractMove(void);
	void gameMoveNow(void);
	void gameQuit(void);
	void gameGetMoveList(char ***list, int *total, int *view);
	int gameSave( int slot );
	int gameLoad( int slot );
	void gameMakeMoveStr(char *move_str, int ui_update);
	int gameGetEngineError(void);
	void gameSetEngineError(int err);

	void initResolution();
	int setResolution(int init);
	void toggleFullscreen();
};

extern Dreamchess *g_Dreamchess;
void game_make_move_str(char *move_str, int ui_update);

typedef struct arguments
{
    int argc;
    char **argv;
}
arguments_t;

typedef struct config
{
    int player[2];
    int cpu_level;
    int difficulty;
}
config_t;

#endif
