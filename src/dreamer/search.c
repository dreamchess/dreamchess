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
#include "history.h"
#include "repetition.h"
#include "transposition.h"
#include "hashing.h"
#include "dreamer.h"
#include "e_comm.h"

/* #define DEBUG */

extern int moves_made;
int abort_search;
move_t prev_best_move;
int prev_best_score;
int have_move;

static int total_nodes;
static int start_time;

/* Principal variation */
move_t pv[MAX_DEPTH][MAX_DEPTH];
int pv_len[MAX_DEPTH];

static inline void pv_term(int ply)
{
    pv_len[ply] = 0;
}

static inline void pv_copy(int ply, move_t move)
{
    pv[ply][0] = move;
    memcpy(&pv[ply][1], &pv[ply + 1][0], pv_len[ply + 1] * sizeof(move_t));
    pv_len[ply] = pv_len[ply + 1] + 1;
}

static void pv_print(state_t *state, int depth, int score)
{
    int i = 0;
    e_comm_send("%3i %7i %i %i", depth, score, total_nodes, get_time() - start_time, total_nodes);
    if (state->board.current_player == SIDE_BLACK) {
        char *s = coord_move_str(pv[0][0]);
        e_comm_send(" %2d. ... %s", state->moves / 2 + 1, s);
        i = 1;
        free(s);
    }
    while (1)
    {
        if (i == pv_len[0])
            break;
        char *s = coord_move_str(pv[0][i++]);
        e_comm_send(" %2d. %s", (state->moves + i) / 2 + 1, s);
        free(s);
        if (i == pv_len[0])
            break;
        s = coord_move_str(pv[0][i++]);
        e_comm_send(" %s", s);
        free(s);
    }
    e_comm_send("\n");
}

static void pv_store_ht(board_t *board, int index)
{
    long long en_passant = board->en_passant;
    int castle_flags = board->castle_flags;
    int fifty_moves = board->fifty_moves;

    if (index == pv_len[0])
        return;

    store_board(board, 0, EVAL_PV, 0, 0, 0, pv[0][index]);
    execute_move(board, pv[0][index]);
    pv_store_ht(board, index + 1);
    unmake_move(board, pv[0][index], en_passant, castle_flags, fifty_moves);
}

int
alpha_beta(board_t *board, int depth, int ply, int check, int alpha, int beta, int side);

int
is_check(board_t *board, int ply);

void poll_abort()
{
    if (!have_move)
        return;

    if (check_abort())
        abort_search = 1;
}

int
quiescence(board_t *board, int ply, int check, int alpha, int beta, int side)
{
    int eval;
    bitboard_t en_passant;
    int castle_flags;
    int fifty_moves;
    move_t move;

    if ((total_nodes++) % 10000 == 0)
        poll_abort();

    if (abort_search)
        return 0;

    if (is_repetition(board, ply - 1))
        return 0;
/*
    if ((check & (board->current_player? 2 : 1)) && is_check(board, ply))
        return alpha_beta(board, 1, ply, board->current_player? 2 : 1, alpha, beta, side);
*/
    /* Needed to catch illegal moves at sd 1 */
    if (compute_legal_moves(board, ply) < 0)
        return ALPHABETA_ILLEGAL;

    eval = board_eval_complete(board, side, alpha, beta);

    if (ply == MAX_DEPTH - 1)
        return eval;

    if (!get_option(OPTION_QUIESCE) || eval >= beta)
        return eval;

    if (eval > alpha)
        alpha = eval;

    en_passant = board->en_passant;
    castle_flags = board->castle_flags;
    fifty_moves = board->fifty_moves;

    while ((move = move_next(board, ply)) != NO_MOVE)
    {
        if (move & (CAPTURE_MOVE | CAPTURE_MOVE_EN_PASSENT | MOVE_PROMOTION_MASK))
        {
            execute_move(board, move);
            eval = -quiescence(board, ply + 1, check & (board->current_player? 1 : 2), -beta, -alpha, side);
            unmake_move(board, move, en_passant, castle_flags, fifty_moves);
            if (eval == -ALPHABETA_ILLEGAL)
                continue;
            if (eval >= beta)
            {
                add_count(move, board->current_player);
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
        board->current_player = OPPONENT(board->current_player);
        if (compute_legal_moves(board, ply) < 0)
        {
            /* depth is added to make checkmates that are
            ** further away more preferable over the ones
            ** that are closer.
            */
            board->current_player = OPPONENT(board->current_player);
            return alpha;
        }
        else
        {
            /* We're stalemated. */
            board->current_player = OPPONENT(board->current_player);
            return 0;
        }
    }

    return alpha;
}

int
alpha_beta(board_t *board, int depth, int ply, int check, int alpha, int beta, int side)
{
    int eval;
    int best_move_score;
    int eval_type = EVAL_UPPERBOUND;
    long long en_passant;
    int castle_flags;
    int fifty_moves;
    move_t best_move;
    move_t move;

    if ((total_nodes++) % 10000 == 0)
        poll_abort();

    if (abort_search)
        return 0;

    if (is_repetition(board, ply - 1)) {
        pv_term(ply);
        return 0;
    }

    if (board->fifty_moves == 100)
    {
        if (compute_legal_moves(board, ply) < 0)
            return ALPHABETA_ILLEGAL;

        pv_term(ply);

        if (is_check(board, ply))
            return ALPHABETA_MIN + ply;

        return 0;
    }

    switch (lookup_board(board, depth, ply, &eval))
    {
    case EVAL_ACCURATE:
        pv_term(ply);
        return eval;
    case EVAL_LOWERBOUND:
        if (eval >= beta)
            return beta;
        break;
    case EVAL_UPPERBOUND:
        if (eval <= alpha)
            return alpha;
    }

    if (depth == 0 || ply == MAX_DEPTH - 1) {
        pv_term(ply);
        return quiescence(board, ply, check, alpha, beta, side);
    }

    if (compute_legal_moves(board, ply) < 0)
        return ALPHABETA_ILLEGAL;

    best_move = NO_MOVE;
    best_move_score = ALPHABETA_ILLEGAL;

    en_passant = board->en_passant;
    castle_flags = board->castle_flags;
    fifty_moves = board->fifty_moves;

    while ((move = move_next(board, ply)) != NO_MOVE)
    {
        int score;
        execute_move(board, move);
        score = -alpha_beta(board, depth - 1, ply + 1, check, -beta, -alpha, side);
        unmake_move(board, move, en_passant, castle_flags, fifty_moves);
        if (abort_search)
            return 0;
        if (score == -ALPHABETA_ILLEGAL)
            continue;
        if (score >= beta) {
                store_board(board, beta, EVAL_LOWERBOUND, depth, ply,
                            0 /* FIXME moves_made */, move);
                add_count(move, board->current_player);
                return beta;
        }
        if (score > best_move_score) {
            if (score > alpha) {
                eval_type = EVAL_ACCURATE;
                alpha = score;
                pv_copy(ply, move);
            }
            best_move_score = score;
            best_move = move;
        }
    }

    if (best_move == NO_MOVE)
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

    store_board(board, alpha, eval_type, depth, ply, 0 /* FIXME moves_made */, best_move);

    return alpha;
}

move_t
find_best_move(state_t *state)
{
    int depth = state->depth;
    board_t *board = &state->board;
    move_t best_move;
    int cur_depth;
    int best_score;
    long long en_passant = board->en_passant;
    int castle_flags = board->castle_flags;
    int fifty_moves = board->fifty_moves;

    total_nodes = 0;
    start_time = get_time();
    prev_best_score = ALPHABETA_ILLEGAL;
    abort_search = 0;
    have_move = 0;

    for (cur_depth = 0; cur_depth < depth; cur_depth++)
    {
        int alpha = ALPHABETA_MIN;
        int eval;
	move_t move;
        best_score = ALPHABETA_MIN;

        compute_legal_moves(board, 0);

        /* e_comm_send("------------------\n"); */
        while ((move = move_next(board, 0)) != NO_MOVE)
        {
            int score;
            /* char *s = coord_move_str(move);
            e_comm_send("Examining move %s..\n", s);
            free(s); */
            execute_move(board, move);
            score = -alpha_beta(board, cur_depth, 1, 3, ALPHABETA_MIN, -alpha, OPPONENT(board->current_player));
            unmake_move(board, move, en_passant, castle_flags, fifty_moves);
            /* e_comm_send("Move scored %i\n", score); */
            if (abort_search)
                break;
            if (score == -ALPHABETA_ILLEGAL)
                continue;
            if (score > alpha)
                alpha = score;
            if (score > best_score)
            {
                best_move = move;
                best_score = score;
                pv_copy(0, move);
                if (get_option(OPTION_POST))
                    pv_print(state, cur_depth + 1, best_score);
            }
        }

        if (abort_search)
        {
            best_move = prev_best_move;
            best_score = prev_best_score;
            break;
        }

        /* If we found a mate we stop the search */
        if (best_score > ALPHABETA_MAX - 100) {
            break;
        }

        if (best_score < ALPHABETA_MIN + 100) {
            break;
        }

        if (best_score == ALPHABETA_ILLEGAL)
        {
            /* There are no legal moves. We're either checkmated or
            ** stalemated.
            */

            /* If the opponent can capture the king that means we're
            ** checkmated.
            */
            board->current_player = OPPONENT(board->current_player);
            if (compute_legal_moves(board, 0) < 0)
            {
                /* We're checkmated. */
                move = RESIGN_MOVE;
                board->current_player = OPPONENT(board->current_player);
                return move;
            }
            else
            {
                /* We're stalemated. */
                move = STALEMATE_MOVE;
                board->current_player = OPPONENT(board->current_player);
                return move;
            }
        }

        have_move = 1;

        prev_best_move = best_move;
        prev_best_score = best_score;
#ifdef DEBUG
        {
            char *str = coord_move_str(&prev_best_move);
            e_comm_send("Best move at depth %i: %s\n", cur_depth + 1, str);
            free(str);
        }
#endif

        pv_store_ht(board, 0);
    }

    return best_move;
}
