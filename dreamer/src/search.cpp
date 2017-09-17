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
#include <string.h>

#include "board.h"
#include "move.h"
#include "search.h"
#include "eval.h"
#include "repetition.h"
#include "ttable.h"
#include "hashing.h"
#include "dreamer.h"
#include "e_comm.h"
#include "commands.h"
#include "timer.h"

/* #define DEBUG */

extern int moves_made;
int abort_search;

static int total_nodes;
static int start_time;

/* Principal variation */
Move pv[MAX_DEPTH][MAX_DEPTH];
int pv_len[MAX_DEPTH];

#if 0
void
print_board(const Board &board)
/* Temporary! */
{
	long long i,j;
	int k;
	for (i = 7; i >= 0; i--) {
		e_comm_send("%lli", i+1);
		for (j = 0; j < 8; j++) {
			char c;
			int fcol, bcol;
			c = ' ';
			fcol = 1;
			for (k = 0; k < NR_BITBOARDS; k++) {
				if (board.bitboard[k] & square_bit[i * 8 + j]) {
					switch (k & PIECE_MASK) {
						case PAWN: c = 'I'; break;
						case ROOK: c = 'X'; break;
						case KNIGHT: c = '7'; break;
						case BISHOP: c = 'l'; break;
						case QUEEN: c = 'Y'; break;
						case KING: c = 'K';
					}
					if (PIECE_IS_WHITE(k)) fcol = 7;
				}
			}
			bcol = (((i % 2) + j) % 2? 7 : 0);
			e_comm_send("\e[1;%i;%im%c\e[0m", fcol + 30, bcol + 40, c);
		}
		e_comm_send("\n");
	}
	e_comm_send(" ABCDEFGH\n");
}
#endif

static inline void pv_term(int ply)
{
    pv_len[ply] = 0;
}

static inline void pv_copy(int ply, Move move)
{
    pv[ply][0] = move;
    for (unsigned i = 0; i < pv_len[ply + 1]; ++i)
        pv[ply][1 + i] = pv[ply + 1][0 + i];
    pv_len[ply] = pv_len[ply + 1] + 1;
}

static void pv_print_move(state_t *state, int index)
{
    long long en_passant = state->board.en_passant;
    int castle_flags = state->board.castle_flags;
    int fifty_moves = state->board.fifty_moves;
    char *s;

    if (index == pv_len[0])
        return;

    if ((state->moves + index) % 2 == 0)
        e_comm_send(" %2d.", (state->moves + index) / 2 + 1);

    /* Ply 0 is used by find_best_move(). */
    s = san_move_str(state->board, 1, pv[0][index]);
    e_comm_send(" %s", s);
    free(s);

    state->board.makeMove(pv[0][index]);
    pv_print_move(state, index + 1);
    state->board.unmakeMove(pv[0][index], en_passant, castle_flags, fifty_moves);
}

static void pv_print(state_t *state, int depth, int score)
{
    if (state->mode == MODE_BLACK)
        score = -score;

    e_comm_send("%3i %7i %i %i", depth, score, total_nodes, get_time() - start_time, total_nodes);
    if (state->board.current_player == SIDE_BLACK)
        e_comm_send(" %2d. ...", state->moves / 2 + 1);

    pv_print_move(state, 0);

    e_comm_send("\n");
}

void pv_clear(void)
{
    pv_term(0);
}

static void pv_store_ht(Board &board, int index)
{
    long long en_passant = board.en_passant;
    int castle_flags = board.castle_flags;
    int fifty_moves = board.fifty_moves;

    if (index == pv_len[0])
        return;

    g_transTable->setBestMove(board, pv[0][index]);
    board.makeMove(pv[0][index]);
    pv_store_ht(board, index + 1);
    board.unmakeMove(pv[0][index], en_passant, castle_flags, fifty_moves);
}

int
alpha_beta(Board &board, int depth, int ply, int alpha, int beta, int side);

int
is_check(Board &board, int ply);

static void poll_abort(int ply)
{
    if (pv_len[0] == 0)
        return;

    if (check_abort(ply))
        abort_search = 1;
}

static int
quiescence(Board &board, int ply, int alpha, int beta, int side)
{
    int eval;
    bitboard_t en_passant;
    int castle_flags;
    int fifty_moves;
    Move move;

    if ((total_nodes++) % 10000 == 0)
        poll_abort(ply);

    if (abort_search)
        return 0;

    if (is_repetition(board, ply - 1))
        return 0;

    /* Needed to catch illegal moves at sd 1 */
    if (g_moveGenerator->computeLegalMoves(board, ply) < 0)
        return ALPHABETA_ILLEGAL;

    eval = board_eval_complete(board, side, alpha, beta);

    if (ply == MAX_DEPTH - 1)
        return eval;

    if (!get_option(OPTION_QUIESCE) || eval >= beta)
        return eval;

    if (eval > alpha)
        alpha = eval;

    en_passant = board.en_passant;
    castle_flags = board.castle_flags;
    fifty_moves = board.fifty_moves;

    while (!(move = g_moveGenerator->getNextMove(board, ply)).isNone())
    {
        if (move.doesCapture() || move.doesPromotion())
        {
            board.makeMove(move);
            eval = -quiescence(board, ply + 1, -beta, -alpha, side);
            board.unmakeMove(move, en_passant, castle_flags, fifty_moves);
            if (eval == -ALPHABETA_ILLEGAL)
                continue;
            if (eval >= beta)
            {
                g_moveGenerator->incHistoryCounter(move, board.current_player);
                return beta;
            }
            if (eval > alpha)
                alpha = eval;
        }
    }

    if (alpha <= ALPHABETA_MIN)
    {
        /* There are no legal moves. We're either checkmated or
        ** stalemated.
        */

        /* If the opponent can capture the king that means we're
        ** checkmated.
        */
        board.current_player = OPPONENT(board.current_player);
        if (g_moveGenerator->computeLegalMoves(board, ply) < 0)
        {
            /* depth is added to make checkmates that are
            ** further away more preferable over the ones
            ** that are closer.
            */
            board.current_player = OPPONENT(board.current_player);
            return alpha;
        }
        else
        {
            /* We're stalemated. */
            board.current_player = OPPONENT(board.current_player);
            return 0;
        }
    }

    return alpha;
}

int
alpha_beta(Board &board, int depth, int ply, int alpha, int beta, int side)
{
    int eval;
    int best_move_score;
    TTable::EvalType evalType = TTable::EvalType::Upperbound;
    long long en_passant;
    int castle_flags;
    int fifty_moves;
    Move best_move;
    Move move;

    if ((total_nodes++) % 10000 == 0)
        poll_abort(ply);

    if (abort_search)
        return 0;

    if (is_repetition(board, ply - 1)) {
        pv_term(ply);
        return 0;
    }

    if (board.fifty_moves == 100)
    {
        if (g_moveGenerator->computeLegalMoves(board, ply) < 0)
            return ALPHABETA_ILLEGAL;

        pv_term(ply);

        /* FIXME, check for mate */
        return 0;
    }

    switch (g_transTable->lookupBoard(board, depth, ply, eval))
    {
    case TTable::EvalType::Accurate:
        pv_term(ply);
        return eval;
    case TTable::EvalType::Lowerbound:
        if (eval >= beta)
            return beta;
        break;
    case TTable::EvalType::Upperbound:
        if (eval <= alpha)
            return alpha;
    }

    if (depth == 0 || ply == MAX_DEPTH - 1) {
        pv_term(ply);
        return quiescence(board, ply, alpha, beta, side);
    }

    if (g_moveGenerator->computeLegalMoves(board, ply) < 0)
        return ALPHABETA_ILLEGAL;

    best_move = Move();
    best_move_score = ALPHABETA_ILLEGAL;

    en_passant = board.en_passant;
    castle_flags = board.castle_flags;
    fifty_moves = board.fifty_moves;

    while (!(move = g_moveGenerator->getNextMove(board, ply)).isNone())
    {
        int score;
        board.makeMove(move);
        score = -alpha_beta(board, depth - 1, ply + 1, -beta, -alpha, side);
        board.unmakeMove(move, en_passant, castle_flags, fifty_moves);
        if (abort_search)
            return 0;
        if (score == -ALPHABETA_ILLEGAL)
            continue;
        if (score >= beta) {
                g_transTable->storeBoard(board, beta, TTable::EvalType::Lowerbound, depth, ply,
                            0 /* FIXME moves_made */, move);
                g_moveGenerator->incHistoryCounter(move, board.current_player);
                return beta;
        }
        if (score > best_move_score) {
            if (score > alpha) {
                evalType = TTable::EvalType::Accurate;
                alpha = score;
                pv_copy(ply, move);
            }
            best_move_score = score;
            best_move = move;
        }
    }

    if (best_move.isNone())
    {
        /* There are no legal moves. We're either checkmated or
        ** stalemated.
        */
        if (is_check(board, ply))
        {
            /* depth is added to make checkmates that are
            ** further away more preferable over the ones
            ** that are closer.
            */
            pv_term(ply);
            return ALPHABETA_MIN + ply;
        }
        else
        {
            /* We're stalemated. */
            pv_term(ply);
            return 0;
        }
    }

    g_transTable->storeBoard(board, alpha, evalType, depth, ply, 0 /* FIXME moves_made */, best_move);

    return alpha;
}

Move
find_best_move(state_t *state)
{
    int depth = state->depth;
    Board &board = state->board;
    Move best_move;
    int cur_depth;
    long long en_passant = board.en_passant;
    int castle_flags = board.castle_flags;
    int fifty_moves = board.fifty_moves;

    total_nodes = 0;
    start_time = get_time();
    abort_search = 0;
    pv_len[0] = 0;

    state->moveTime.start(Timer::Direction::Down);
    g_moveGenerator->ageHistory();

    for (cur_depth = 0; cur_depth < depth; cur_depth++)
    {
        int alpha = ALPHABETA_MIN;
	    Move move;

        g_moveGenerator->computeLegalMoves(board, 0);

        /* e_comm_send("------------------\n"); */
        while (!(move = g_moveGenerator->getNextMove(board, 0)).isNone())
        {
            int score;
            /* char *s = coord_move_str(move);
            e_comm_send("Examining move %s..\n", s);
            free(s); */
            state->board.makeMove(move);
            score = -alpha_beta(state->board, cur_depth, 1, ALPHABETA_MIN, -alpha, OPPONENT(board.current_player));
            state->board.unmakeMove(move, en_passant, castle_flags, fifty_moves);
            /* e_comm_send("Move scored %i\n", score); */
            if (abort_search)
            {
                if (state->flags & FLAG_IGNORE_MOVE)
                    return Move();
                break;
            }
            if (score == -ALPHABETA_ILLEGAL)
                continue;
            if (score > alpha)
            {
                alpha = score;
                best_move = move;
                pv_copy(0, move);
                if (get_option(OPTION_POST))
                    pv_print(state, cur_depth + 1, alpha);
            }
        }

        /* If we found a mate in 'ply' we stop the search */
        if (alpha == ALPHABETA_MAX - cur_depth) {
            break;
        }

        if (alpha < ALPHABETA_MIN + 100) {
            break;
        }

#ifdef DEBUG
        {
            char *str = coord_move_str(&prev_best_move);
            e_comm_send("Best move at depth %i: %s\n", cur_depth + 1, str);
            free(str);
        }
#endif

        pv_store_ht(board, 0);

        if (abort_search)
            break;
    }

    if (best_move.isNone())
    {
	state->hint = Move();

        /* There are no legal moves. We're either checkmated or
        ** stalemated.
        */

        /* If the opponent can capture the king that means we're
        ** checkmated.
        */
        board.current_player = OPPONENT(board.current_player);
        if (g_moveGenerator->computeLegalMoves(board, 0) < 0)
        {
            /* We're checkmated. */
            board.current_player = OPPONENT(board.current_player);
            return Move(0, 0, 0, Move::Type::Resign, 0);
        }
        else
        {
            /* We're stalemated. */
            board.current_player = OPPONENT(board.current_player);
            return Move(0, 0, 0, Move::Type::Stalemate, 0);
        }
    }

    if (pv_len[0] > 1)
        state->hint = pv[0][1];
    else
    {
        /* Try to get hint move from hash table. */
        state->board.makeMove(best_move);
        state->hint = g_transTable->lookupBestMove(board);
        state->board.unmakeMove(best_move, en_passant, castle_flags, fifty_moves);
    }

    return best_move;
}

Move
ponder(state_t *state)
{
	Move move;

	if (state->hint.isNone())
		return Move();

        state->root_board = state->board;
	state->ponder_actual_move = Move();
	state->ponder_opp_move = state->hint;
	do_move(state, state->ponder_opp_move);
        state->flags = FLAG_DELAY_MOVE;

        set_move_time();

        command_handle(state, "hint");
	move = find_best_move(state);

	if (state->mode == MODE_QUIT || (state->flags & FLAG_NEW_GAME))
		return Move();

	if (move.isNone())
        {
		/* Player did not play the move we expected */
		/* or pondering was switched off. */
		undo_move(state);
                if (!state->ponder_actual_move.isNone())
                {
                    do_move(state, state->ponder_actual_move);
                    check_game_end(state);
                }

		return Move();
        }

        if (state->flags & FLAG_DELAY_MOVE)
        {
                /* Opponent hasn't moved yet. */
 		undo_move(state);
        }

	return move;
}
