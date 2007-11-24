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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>

#include "dreamer.h"
#include "board.h"
#include "move.h"
#include "search.h"
#include "hashing.h"
#include "e_comm.h"
#include "commands.h"
#include "repetition.h"
#include "transposition.h"
#include "svn_version.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef __WIN32__

#include <windows.h>
#define drm_sleep(M) Sleep(M)

#elif defined _arch_dreamcast

#include <kos/thread.h>
#define drm_sleep(M) thd_sleep(M)

#elif defined __BEOS__

#define drm_sleep(M) snooze((M) * 1000)

#elif defined HAVE_USLEEP

#define drm_sleep(M) usleep((M) * 1000)

#else

void drm_sleep(unsigned long usec)
{
    struct timeval tv;

    tv.tv_sec = 0;
    tv.tv_usec = usec;
    select(0, 0, 0, 0, &tv);
}

#endif

static int start_time;
static state_t state;

int my_turn(state_t *state)
{
    return (((state->mode == MODE_WHITE) &&
             (state->board.current_player == SIDE_WHITE))
            || ((state->mode == MODE_BLACK) &&
                (state->board.current_player == SIDE_BLACK)));
}

int is_check(board_t *board)
{
    /* FIXME */
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
        bitboard_t en_passant = board->en_passant;
        int castle_flags = board->castle_flags;
        int fifty_moves = board->fifty_moves;

        execute_move(board, &moves[move_nr]);
        board->current_player = OPPONENT(board->current_player);
        if (!is_check(board))
        {
            mate = STATE_NORMAL;
            board->current_player = OPPONENT(board->current_player);
            unmake_move(board, &moves[move_nr], en_passant, castle_flags, fifty_moves);
            break;
        }
        board->current_player = OPPONENT(board->current_player);
        unmake_move(board, &moves[move_nr], en_passant, castle_flags, fifty_moves);
    }
    /* We're either stalemated or checkmated. */
    if (!is_check(board) && (mate == STATE_MATE))
        mate = STATE_STALEMATE;
    if (is_check(board) && (mate == STATE_NORMAL))
        mate = STATE_CHECK;
    return mate;
}

int get_option(int option)
{
    return state.options & (1 << option);
}

void set_option(int option, int value)
{
    state.options &= ~(1 << option);
    state.options |= (value << option);
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
    char *s;

    if (get_time() >= state.move_now_time)
        return 1;

    s = e_comm_poll();
    if (!s)
        return 0;
    return command_check_abort(&state, s);
}

void do_move(state_t *state, move_t *move)
{
    state->moves++;
    state->undo_data = realloc(state->undo_data,
                               sizeof(undo_data_t) * state->moves);
    state->undo_data[state->moves - 1].en_passant =
        state->board.en_passant;
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
                state->undo_data[state->moves].en_passant,
                state->undo_data[state->moves].castle_flags,
                state->undo_data[state->moves].fifty_moves);

    repetition_remove();
}

static void set_start_time()
{
    struct timeval tv;

    gettimeofday(&tv, NULL);

    start_time = tv.tv_sec;
}

static void set_move_now_time()
{
    if (state.engine_time - state.time.inc > 0)
        state.move_now_time = get_time() + (state.engine_time - state.time.inc) / 30 + state.time.inc;
    else
        state.move_now_time = 0;
}

int get_time()
{
    struct timeval tv;

    gettimeofday(&tv, NULL);

    tv.tv_sec -= start_time;

    return tv.tv_sec * 100 + tv.tv_usec / 10000;
}

int engine()
{
    printf("Dreamer v" PACKAGE_VERSION " (r" SVN_VERSION ")\n");
    e_comm_init();
    move_init();
    board_init();
    init_hash();
    transposition_init(128);
    set_start_time();

    setup_board(&state.board);
    state.mode = MODE_IDLE;
    state.flags = 0;
    state.undo_data = 0;
    state.moves = 0;
    state.time.mps = 40;
    state.time.base = 5;
    state.time.inc = 0;
    state.done = 0;
    set_option(OPTION_QUIESCE, 1);

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
            set_move_now_time();
            move = find_best_move(&state);
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

    transposition_exit();
    return 0;
}
