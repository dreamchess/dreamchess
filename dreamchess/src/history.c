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

#include "history.h"
#include "board.h"

/** @brief Writes a token to an output file. Wraps lines at 79 characters.
 *
 *  @param f File to write to.
 *  @param width Width of current line (will be updated by function).
 *  @param token Token to write.
 */
static void write_token(FILE *f, int *width, char *token)
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

history_t *history_init(board_t *board)
{
    history_t *hist = (history_t *) malloc(sizeof(history_t));
    step_t *step = (step_t *) malloc(sizeof(step_t));

    step->board = (board_t *) malloc(sizeof(board_t));
    step->prev = NULL;
    step->next = NULL;
    *step->board = *board;
    step->move = NULL;
    hist->last = step;
    hist->view = step;
    hist->first = step;
    hist->result = NULL;

    return hist;
}

void history_exit(history_t *hist)
{
    step_t *step = hist->last;

    while (step)
    {
        step_t *s;
        if (step->move)
            free(step->move);
        if (step->board)
            free(step->board);
        s = step;
        step = step->prev;
        free(s);
    }

    if (hist->result)
    {
        free(hist->result->reason);
        free(hist->result);
    }

    free(hist);
}

void history_play(history_t *hist, move_t *move, board_t *board)
{
    step_t *step = (step_t *) malloc(sizeof(step_t));

    step->board = (board_t *) malloc(sizeof(board_t));
    hist->last->move = (move_t *) malloc(sizeof(move_t));
    *step->board = *board;
    step->move = NULL;
    step->prev = hist->last;
    step->next = NULL;
    *hist->last->move = *move;
    hist->last->next = step;
    hist->last = step;
    hist->view = step;
}

int history_undo(history_t *hist)
{
    if (!hist->last->prev)
        return 1;

    free(hist->last->board);
    hist->last = hist->last->prev;
    free(hist->last->move);
    hist->last->move = NULL;
    free(hist->last->next);
    hist->last->next = NULL;
    hist->view = hist->last;
    return 0;
}

int history_view_next(history_t *hist)
{
    if (!hist->view->next)
        return 1;

    hist->view = hist->view->next;

    return 0;
}

int history_view_prev(history_t *hist)
{
    if (!hist->view->prev)
        return 1;

    hist->view = hist->view->prev;

    return 0;
}

int history_save_pgn(history_t *hist, char *filename)
{
    FILE *f;
    int width;
    int side;
    int move;
    step_t *step;
    char *res;

    f = fopen(filename, "w");

    if (!f)
        return 1;

    fputs("[Event \"DreamChess Game\"]\n", f);
    fputs("[Site \"?\"]\n", f);
    fputs("[Date \"????.??.??\"]\n", f);
    fputs("[Round \"-\"]\n", f);
    fputs("[White \"?\"]\n", f);
    fputs("[Black \"?\"]\n", f);

    if (!hist->result)
        res = "*";
    else if (hist->result->code == RESULT_WHITE_WINS)
        res = "1-0";
    else if (hist->result->code == RESULT_BLACK_WINS)
        res = "0-1";
    else
        res = "1/2-1/2";

    fprintf(f, "[Result \"%s\"]\n\n", res);

    width = 0;
    move = 1;
    side = WHITE;
    step = hist->first;

    while (step->next)
    {
        char buf[16];
        char *san;

        if (side == WHITE)
        {
            snprintf(buf, 16, "%i.", move++);
            write_token(f, &width, buf);
        }

        san = move_to_san(step->board, step->move);
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
