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
#include <string.h>

#include "History.h"
#include "ChessBoard.h"

/** @brief Writes a token to an output file. Wraps lines at 79 characters.
 *
 *  @param f File to write to.
 *  @param width Width of current line (will be updated by function).
 *  @param token Token to write.
 */
static void write_token(FILE *f, int *width, const char *token)
{
    /* Handle line wrapping. Add 1 for leading space. */
    if ((*width > 0) && (*width + 1 + strlen(token) >= 80))
    {
        fputs("\n", f);
        *width = 0;
    }

    /* Output leading space. */
    if (*width > 0)
    {
        fputs(" ", f);
        (*width)++;
    }

    fputs(token, f);
    *width += strlen(token);
}

void History::init(Board *board) {
    //history_t *hist = (history_t *) malloc(sizeof(history_t));
    step_t *step = (step_t *) malloc(sizeof(step_t));

    step->board = new Board();
    step->prev = NULL;
    step->next = NULL;
    *step->board = *board;
    step->move = NULL;
    last = step;
    view = step;
    first = step;
    result = NULL;
}

History::~History() {
    step_t *step = last;

    while (step) {
        step_t *s;
        if (step->move)
            free(step->move);
        if (step->board)
            free(step->board);
        s = step;
        step = step->prev;
        free(s);
    }

    if (result) {
        free(result->reason);
        free(result);
    }
}

void History::play(move_t *move, Board *board) {
    step_t *step = (step_t *) malloc(sizeof(step_t));

    step->board = new Board();
    last->move = (move_t *) malloc(sizeof(move_t));
    *step->board = *board;
    step->move = NULL;
    step->prev = last;
    step->next = NULL;
    *last->move = *move;
    last->next = step;
    last = step;
    view = step;
}

int History::undo() {
    if (!last->prev)
        return 1;

    free(last->board);
    last = last->prev;
    free(last->move);
    last->move = NULL;
    free(last->next);
    last->next = NULL;
    view = last;
    return 0;
}

int History::viewNext() {
    if (!view->next)
        return 1;

    view = view->next;

    return 0;
}

int History::viewPrev() {
    if (!view->prev)
        return 1;

    view = view->prev;

    return 0;
}

int History::savePGN(char *filename) {
    FILE *f;
    int width;
    int side;
    int move;
    step_t *step;
    const char *res;

    f = fopen(filename, "w");

    if (!f)
        return 1;

    fputs("[Event \"DreamChess Game\"]\n", f);
    fputs("[Site \"?\"]\n", f);
    fputs("[Date \"????.??.??\"]\n", f);
    fputs("[Round \"-\"]\n", f);
    fputs("[White \"?\"]\n", f);
    fputs("[Black \"?\"]\n", f);

    if (!result)
        res = "*";
    else if (result->code == RESULT_WHITE_WINS)
        res = "1-0";
    else if (result->code == RESULT_BLACK_WINS)
        res = "0-1";
    else
        res = "1/2-1/2";

    fprintf(f, "[Result \"%s\"]\n\n", res);

    width = 0;
    move = 1;
    side = WHITE;
    step = first;

    while (step->next)
    {
        char buf[16];
        char *san;

        if (side == WHITE)
        {
            snprintf(buf, 16, "%i.", move++);
            write_token(f, &width, buf);
        }

        san = step->board->moveToSan(step->move);
        write_token(f, &width, san);
        free(san);

        side = OPPONENT(side);
        step = step->next;
    }

    write_token(f, &width, res);
    fputs("\n\n", f);
    fclose(f);

    return 0;
}
