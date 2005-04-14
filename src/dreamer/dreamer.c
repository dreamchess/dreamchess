/*  DreamChess
**  Copyright (C) 2003-2005  The DreamChess project
**
**  This program is free software; you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation; either version 2 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "state.h"
#include "board.h"
#include "move.h"
#include "search.h"
#include "hashing.h"
#include "e_comm.h"
#include "commands.h"

#ifdef __WIN32__
#include <windows.h>
#define drm_sleep(M) Sleep(M)
#elif defined _arch_dreamcast
#include <kos/thread.h>
#define drm_sleep(M) thd_sleep(M)
#elif defined __BEOS__
#define drm_sleep(M) snooze((M) * 1000)
#else
#define drm_sleep(M) usleep((M) * 1000)
#endif

static state_t state;
int moves_made;

int my_turn(state_t *state)
{
    return (((state->mode == MODE_WHITE) &&
             (state->board.current_player == SIDE_WHITE))
            || ((state->mode == MODE_BLACK) &&
                (state->board.current_player == SIDE_BLACK)));
}

int is_check(board_t *board)
{
    move_t moves[28*16];
    board->current_player = OPPONENT(board->current_player);
    if (compute_legal_moves(board, moves) < 0)
    {
        /* We're in check. */
        board->current_player = OPPONENT(board->current_player);
        return 1;
    }
    board->current_player = OPPONENT(board->current_player);
    return 0;
}

int check_game_state(board_t *board)
{
    int mate = STATE_MATE;
    move_t moves[28*16];
    int total_moves;
    int move_nr;
    total_moves = compute_legal_moves(board, moves);
    for (move_nr = 0; move_nr < total_moves; move_nr++)
    {
        bitboard_t en_passent = board->en_passent;
        int castle_flags = board->castle_flags;
        int fifty_moves = board->fifty_moves;

        execute_move(board, &moves[move_nr]);
        board->current_player = OPPONENT(board->current_player);
        if (!is_check(board))
        {
            mate = STATE_NORMAL;
            board->current_player = OPPONENT(board->current_player);
            unmake_move(board, &moves[move_nr], en_passent, castle_flags, fifty_moves);
            break;
        }
        board->current_player = OPPONENT(board->current_player);
        unmake_move(board, &moves[move_nr], en_passent, castle_flags, fifty_moves);
    }
    /* We're either stalemated or checkmated. */
    if (!is_check(board) && (mate == STATE_MATE))
        mate = STATE_STALEMATE;
    if (is_check(board) && (mate == STATE_NORMAL))
        mate = STATE_CHECK;
    return mate;
}

void check_game_end(state_t *state)
{
    board_t *board = &state->board;
    int res = check_game_state(board);

    switch (res)
    {
    case STATE_MATE:
        state->done = 1;
        if (board->current_player == SIDE_WHITE)
            e_comm_send("0-1 {Black mates}\n");
        else
            e_comm_send("1-0 {White mates}\n");
        return;
    case STATE_STALEMATE:
        state->done = 1;
        e_comm_send("1/2-1/2 {Stalemate}\n");
        return;
    case STATE_NORMAL:
        switch (is_draw(board))
        {
        case 1:
            state->done = 1;
            e_comm_send("1/2-1/2 {Drawn by 3-fold repetition}\n");
            return;
        case 2:
            state->done = 1;
            e_comm_send("1/2-1/2 {Drawn by 50 move rule}\n");
            return;
        }
    }

}

int check_abort()
{
    char *s = e_comm_poll();
    if (!s)
        return 0;
    return command_check_abort(&state, s);
}

void do_move(state_t *state, move_t *move)
{
    state->moves++;
    state->undo_data = realloc(state->undo_data,
                               sizeof(undo_data_t) * state->moves);
    state->undo_data[state->moves - 1].en_passent =
        state->board.en_passent;
    state->undo_data[state->moves - 1].castle_flags =
        state->board.castle_flags;
    state->undo_data[state->moves - 1].fifty_moves =
        state->board.fifty_moves;
    state->undo_data[state->moves - 1].move =
        *move;
    execute_move(&state->board, move);
    repetition_add(&state->board, move);
}

void undo_move(state_t *state)
{
    if (state->moves == 0)
        return;

    state->moves--;

    unmake_move(&state->board,
                &state->undo_data[state->moves].move,
                state->undo_data[state->moves].en_passent,
                state->undo_data[state->moves].castle_flags,
                state->undo_data[state->moves].fifty_moves);

    repetition_remove();
}

int engine()
{
    e_comm_init();
    move_init();
    board_init();
    init_hash();
    state.mode = MODE_IDLE;
    state.flags = 0;
    state.undo_data = 0;
    state.moves = 0;
    while (state.mode != MODE_QUIT)
    {
        char *s;
        while(!(s = e_comm_poll()))
            drm_sleep(10);
        command_handle(&state, s);
        if (!state.done && my_turn(&state))
        {
            move_t move;
            state.flags = 0;
            move = find_best_move(&state.board, state.depth);
            if (state.flags & FLAG_NEW_GAME)
                command_handle(&state, "new");
            else if (!(state.flags & FLAG_IGNORE_MOVE))
            {
                char *str = coord_move_str(&move);
                do_move(&state, &move);
                e_comm_send("move %s\n", str);
                free(str);
                check_game_end(&state);
            }
        }
        free(s);
    }
    return 0;
}
