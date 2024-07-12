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

#include "board.h"
#include "commands.h"
#include "config.h"
#include "dreamer.h"
#include "e_comm.h"
#include "hashing.h"
#include "move.h"
#include "repetition.h"
#include "search.h"
#include "transposition.h"

#ifdef _WIN32

#include <windows.h>
#define drm_sleep(M) Sleep(M)

#elif defined HAVE_USLEEP

#include <unistd.h>

#define drm_sleep(M) usleep((M)*1000)

#else

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

static void drm_sleep(unsigned long usec) {
	struct timeval tv;

	tv.tv_sec = 0;
	tv.tv_usec = usec;
	select(0, 0, 0, 0, &tv);
}

#endif

static int start_time;
static state_t state;

int my_turn(state_t *state) {
	return (((state->mode == MODE_WHITE) && (state->board.current_player == SIDE_WHITE)) ||
			((state->mode == MODE_BLACK) && (state->board.current_player == SIDE_BLACK)));
}

int is_check(board_t *board, int ply) {
	/* FIXME */
	board->current_player = OPPONENT(board->current_player);
	if (compute_legal_moves(board, ply) < 0) {
		/* We're in check. */
		board->current_player = OPPONENT(board->current_player);
		return 1;
	}
	board->current_player = OPPONENT(board->current_player);
	return 0;
}

int check_game_state(board_t *board, int ply) {
	move_t move;
	int mate = STATE_MATE;
	compute_legal_moves(board, ply);

	while ((move = move_next(board, ply)) != NO_MOVE) {
		bitboard_t en_passant = board->en_passant;
		int castle_flags = board->castle_flags;
		int fifty_moves = board->fifty_moves;

		execute_move(board, move);
		board->current_player = OPPONENT(board->current_player);
		if (!is_check(board, ply + 1)) {
			mate = STATE_NORMAL;
			board->current_player = OPPONENT(board->current_player);
			unmake_move(board, move, en_passant, castle_flags, fifty_moves);
			break;
		}
		board->current_player = OPPONENT(board->current_player);
		unmake_move(board, move, en_passant, castle_flags, fifty_moves);
	}
	/* We're either stalemated or checkmated. */
	if (!is_check(board, ply) && (mate == STATE_MATE))
		mate = STATE_STALEMATE;
	if (is_check(board, ply) && (mate == STATE_NORMAL))
		mate = STATE_CHECK;
	return mate;
}

int get_option(int option) {
	return state.options & (1 << option);
}

void set_option(int option, int value) {
	state.options &= ~(1 << option);
	state.options |= (value << option);
}

void check_game_end(state_t *state) {
	board_t *board = &state->board;
	int res = check_game_state(board, 0);

	switch (res) {
	case STATE_MATE:
		state->done = 1;
		if (board->current_player == SIDE_WHITE)
			e_comm_send("0-1 {Checkmate}\n");
		else
			e_comm_send("1-0 {Checkmate}\n");
		return;
	case STATE_STALEMATE:
		state->done = 1;
		e_comm_send("1/2-1/2 {Stalemate}\n");
		return;
	case STATE_NORMAL:
		switch (is_draw(board)) {
		case 1:
			state->done = 1;
			e_comm_send("1/2-1/2 {Threefold repetition}\n");
			return;
		case 2:
			state->done = 1;
			e_comm_send("1/2-1/2 {50-move rule}\n");
			return;
		}
	}
}

int check_abort(int ply) {
	char *s;

	if (!(state.flags & FLAG_PONDER) && (timer_get(&state.move_time) <= 0))
		return 1;

	s = e_comm_poll();
	if (!s)
		return 0;
	return command_check_abort(&state, ply, s);
}

void do_move(state_t *state, move_t move) {
	state->moves++;
	state->undo_data = realloc(state->undo_data, sizeof(undo_data_t) * state->moves);
	state->undo_data[state->moves - 1].en_passant = state->board.en_passant;
	state->undo_data[state->moves - 1].castle_flags = state->board.castle_flags;
	state->undo_data[state->moves - 1].fifty_moves = state->board.fifty_moves;
	state->undo_data[state->moves - 1].move = move;
	execute_move(&state->board, move);
	repetition_add(&state->board, move);
}

void undo_move(state_t *state) {
	if (state->moves == 0)
		return;

	state->moves--;

	unmake_move(&state->board, state->undo_data[state->moves].move, state->undo_data[state->moves].en_passant,
				state->undo_data[state->moves].castle_flags, state->undo_data[state->moves].fifty_moves);

	repetition_remove();
}

static void set_start_time(void) {
	struct timeval tv;

	gettimeofday(&tv, NULL);

	start_time = tv.tv_sec;
}

void set_move_time(void) {
	int safe_time = timer_get(&state.engine_time) - 1000;

	timer_init(&state.move_time, 1);

	if (safe_time > 0) {
		if (state.time.mps == 0)
			timer_set(&state.move_time, safe_time / 30 + state.time.inc);
		else {
			int moves_left = state.time.mps - (state.moves / 2) % state.time.mps;
			timer_set(&state.move_time, safe_time / moves_left + state.time.inc);
		}
	} else
		timer_set(&state.move_time, 0);
}

int get_time(void) {
	struct timeval tv;

	gettimeofday(&tv, NULL);

	tv.tv_sec -= start_time;

	return tv.tv_sec * 100 + tv.tv_usec / 10000;
}

static void update_clock(state_t *state) {
	int val;

	if (state->time.mps == 0)
		return;

	val = timer_get(&state->engine_time);

	if ((((state->moves + 1) / 2) % state->time.mps) == 0)
		val += state->time.base;

	val += state->time.inc;

	timer_set(&state->engine_time, val);
}

void send_move(state_t *state, move_t move) {
	char *str = coord_move_str(move);
	do_move(state, move);
	e_comm_send("move %s\n", str);
	timer_stop(&state->move_time);
	update_clock(state);
	free(str);
	check_game_end(state);
}

int engine(void *data) {
	e_comm_init();
	set_start_time();

	state.time.mps = 40;
	state.time.base = 5;
	state.time.inc = 0;
	set_option(OPTION_QUIESCE, 1);
	set_option(OPTION_PONDER, 0);
	set_option(OPTION_POST, 0);

	command_handle(&state, "new");

	while (state.mode != MODE_QUIT) {
		char *s;
		move_t move;

		s = e_comm_poll();

		if (!s)
			drm_sleep(10);
		else {
			command_handle(&state, s);
			free(s);
		}

		if (state.mode != MODE_IDLE && state.mode != MODE_FORCE && !state.done) {
			if (my_turn(&state)) {
				state.flags = 0;
				set_move_time();

				timer_start(&state.engine_time);
				move = find_best_move(&state);

				if (state.flags & FLAG_NEW_GAME)
					command_handle(&state, "new");
				else if (MOVE_IS_REGULAR(move)) {
					send_move(&state, move);
					timer_stop(&state.engine_time);
					if (get_option(OPTION_PONDER))
						state.flags |= FLAG_PONDER;
				}
			} else if (state.flags & FLAG_PONDER) {
				move = ponder(&state);

				if (state.flags & FLAG_NEW_GAME)
					command_handle(&state, "new");
				else if (!my_turn(&state)) {
					if (move != NO_MOVE) {
						/* We are done pondering, but opponent hasn't moved yet. */
						state.ponder_my_move = move;
						state.flags = 0;
					} else {
						/* Opponent made an illegal move, continue pondering. */
						if (get_option(OPTION_PONDER))
							state.flags |= FLAG_PONDER;
					}
				} else if (MOVE_IS_REGULAR(move)) {
					/* Opponent made the expected move. */
					send_move(&state, move);
					if (get_option(OPTION_PONDER))
						state.flags |= FLAG_PONDER;
				}
			}
		}
	}

	transposition_exit();
	return 0;
}
