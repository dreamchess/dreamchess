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

#include "board.h"
#include "move.h"
#include "search.h"
#include "eval.h"
#include "history.h"
#include "repetition.h"
#include "transposition.h"
#include "hashing.h"
#include "dreamer.h"

/* #define DEBUG */

extern int moves_made;
int abort_search;
int node_trigger;
move_t prev_best_move;
int prev_best_score;
int have_move;

int
alpha_beta(board_t *board, int depth, int ply, int check, int alpha, int beta);

int
is_check(board_t *board);

void poll_abort()
{
    if (!have_move)
        return;

    if (check_abort())
        abort_search = 1;
}

int
quiescence(board_t *board, int ply, int check, int alpha, int beta)
{
    move_t moves[28*16];
    int total_moves;
    int move_nr;
    int best_move_score;
    bitboard_t en_passant;
    int castle_flags;
    int fifty_moves;

    if (!node_trigger--)
    {
        poll_abort();
        node_trigger = 10000;
    }

    if (abort_search)
        return 0;

    if (is_repetition(board, ply))
        return 0;

    if ((check & (board->current_player? 2 : 1)) && is_check(board))
        return alpha_beta(board, 1, ply, board->current_player? 2 : 1, alpha, beta);

    /* Is this needed? */
    if ((total_moves = compute_legal_moves(board, moves)) < 0)
        return ALPHABETA_ILLEGAL;

    sort_moves(moves, total_moves, board->current_player);

    best_move_score = board_eval_complete(board);

    if (!get_option(OPTION_QUIESCE) || best_move_score >= beta)
        return best_move_score;

    if (best_move_score > alpha)
        alpha = best_move_score;

    en_passant = board->en_passant;
    castle_flags = board->castle_flags;
    fifty_moves = board->fifty_moves;

    for (move_nr = 0; move_nr < total_moves; move_nr++)
    {
        int score;
        if ((moves[move_nr].type == CAPTURE_MOVE) ||
                (moves[move_nr].type == CAPTURE_MOVE_EN_PASSENT) ||
                (moves[move_nr].type & MOVE_PROMOTION_MASK))
        {

            execute_move(board, &moves[move_nr]);
            score = -quiescence(board, ply + 1, check & (board->current_player? 1 : 2), -beta, -alpha);
            unmake_move(board, &moves[move_nr], en_passant, castle_flags, fifty_moves);
            if (abort_search)
                return 0;
            if (score == -ALPHABETA_ILLEGAL)
                continue;
            if (score > alpha)
                alpha = score;
            if (score > best_move_score)
            {
                best_move_score = score;
                if (best_move_score >= beta)
                {
                    add_count(&moves[move_nr], board->current_player);
                    return best_move_score;
                }
            }
        }
    }
    if (best_move_score <= ALPHABETA_MIN)
    {
        /* There are no legal moves. We're either checkmated or
        ** stalemated.
        */

        /* If the opponent can capture the king that means we're
        ** checkmated.
        */
        board->current_player = OPPONENT(board->current_player);
        if (compute_legal_moves(board, moves) < 0)
        {
            /* depth is added to make checkmates that are
            ** further away more preferable over the ones
            ** that are closer.
            */
            board->current_player = OPPONENT(board->current_player);
            return best_move_score;
        }
        else
        {
            /* We're stalemated. */
            board->current_player = OPPONENT(board->current_player);
            return 0;
        }
    }

    return best_move_score;
}

int
alpha_beta(board_t *board, int depth, int ply, int check, int alpha, int beta)
{
    move_t moves[28*16];
    int total_moves;
    int move_nr;
    int best_move_score;
    int trans_eval;
    int hard_alpha = ALPHABETA_MIN;
    int hard_beta = ALPHABETA_MAX;
    int current_alpha = alpha;
    long long en_passant;
    int castle_flags;
    int fifty_moves;

    if (!node_trigger--)
    {
        poll_abort();
        node_trigger = 10000;
    }

    if (abort_search)
        return 0;

    if (is_repetition(board, ply))
        return 0;

    switch (lookup_board(board, depth, ply, &trans_eval))
    {
    case EVAL_ACCURATE:
        return trans_eval;
    case EVAL_LOWERBOUND:
        if (trans_eval >= beta)
            return trans_eval;
        else
        {
            hard_alpha = trans_eval;
            break;
        }
    case EVAL_UPPERBOUND:
        if (trans_eval <= alpha)
            return trans_eval;
        else
            hard_beta = trans_eval;
    }

    if (depth == 0)
        return quiescence(board, ply, check, alpha, beta);

    if ((total_moves = compute_legal_moves(board, moves)) < 0)
        return ALPHABETA_ILLEGAL;

    sort_moves(moves, total_moves, board->current_player);

    best_move_score = ALPHABETA_ILLEGAL;

    en_passant = board->en_passant;
    castle_flags = board->castle_flags;
    fifty_moves = board->fifty_moves;

    for (move_nr = 0; move_nr < total_moves; move_nr++)
    {
        int score;
        execute_move(board, &moves[move_nr]);
        score = -alpha_beta(board, depth - 1, ply + 1, check, -beta, -current_alpha);
        unmake_move(board, &moves[move_nr], en_passant, castle_flags, fifty_moves);
        if (abort_search)
            return 0;
        if (score == -ALPHABETA_ILLEGAL)
            continue;
        if (score > current_alpha)
            current_alpha = score;
        if (score > best_move_score)
        {
            best_move_score = score;
            if (best_move_score >= beta)
            {
                store_board(board, best_move_score, EVAL_LOWERBOUND, depth, ply,
                            moves_made);
                add_count(&moves[move_nr], board->current_player);
                break;
            }
        }
    }

    if (best_move_score == ALPHABETA_ILLEGAL)
    {
        /* There are no legal moves. We're either checkmated or
        ** stalemated.
        */

        /* If the opponent can capture the king that means we're
        ** checkmated.
        */
        board->current_player = OPPONENT(board->current_player);
        if (compute_legal_moves(board, moves) < 0)
        {
            /* depth is added to make checkmates that are
            ** further away more preferable over the ones
            ** that are closer.
            */
            board->current_player = OPPONENT(board->current_player);
            return ALPHABETA_MIN + ply;
        }
        else
        {
            /* We're stalemated. */
            board->current_player = OPPONENT(board->current_player);
            return 0;
        }
    }

    if (current_alpha == alpha)
    {
        /* Fail low. */
        store_board(board, best_move_score, EVAL_UPPERBOUND, depth, ply,
                    moves_made);
    }
    else if (best_move_score < beta)
    {
        store_board(board, best_move_score, EVAL_ACCURATE, depth, ply, moves_made);
    }

    if (best_move_score < hard_alpha)
        best_move_score = hard_alpha;
    if (best_move_score > hard_beta)
        best_move_score = hard_beta;

    if (board->fifty_moves == 100)
        return 0;
    else
        return best_move_score;
}

move_t
find_best_move(board_t *board, int depth)
{
    /* depth = 4; */
    move_t moves[28*16];
    move_t *best_move;
    int total_moves;
    int move_nr;
    int cur_depth;
    int best_score;
    long long en_passant = board->en_passant;
    int castle_flags = board->castle_flags;
    int fifty_moves = board->fifty_moves;
    node_trigger = 10000;
    prev_best_score = ALPHABETA_MIN;
    abort_search = 0;
    have_move = 0;

    for (cur_depth = 0; cur_depth < depth; cur_depth++)
    {
        int alpha = ALPHABETA_MIN;
        best_score = ALPHABETA_MIN;
        total_moves = compute_legal_moves(board, moves);
        sort_moves(moves, total_moves, board->current_player);
        /* e_comm_send("\nDepth %i\n", cur_depth); */ 
        for (move_nr = 0; move_nr < total_moves; move_nr++)
        {
            int score;
            execute_move(board, &moves[move_nr]);
            /* e_comm_send("Examining move %c%c-%c%c..\n", moves[move_nr].source % 8 + 'a',
                moves[move_nr].source / 8 + '1', moves[move_nr].destination % 8 + 'a',
                moves[move_nr].destination / 8 + '1'); */
            score = -alpha_beta(board, cur_depth, 0, 3, ALPHABETA_MIN, -alpha);
            unmake_move(board, &moves[move_nr], en_passant, castle_flags, fifty_moves);
            /* e_comm_send("Move scored %i\n", score); */
            if (abort_search)
                break;
            if (score == -ALPHABETA_ILLEGAL)
                continue;
            if (score > alpha)
                alpha = score;
            if (score > best_score)
            {
                best_move = &moves[move_nr];
                best_score = score;
            }
        }

        /* If we found a mate a mate in 'cur_depth' we can stop the search */
        if (best_score == ALPHABETA_MAX - cur_depth)
            break;

        if (abort_search)
        {
            best_move = &prev_best_move;
            best_score = prev_best_score;
            break;
        }

        have_move = 1;

        prev_best_move = *best_move;
        prev_best_score = best_score;
#ifdef DEBUG
        {
            char *str = coord_move_str(&prev_best_move);
            e_comm_send("Best move at depth %i: %s\n", cur_depth + 1, str);
            free(str);
        }
#endif
    }

    if (best_score <= ALPHABETA_MIN)
    {
        /* There are no legal moves. We're either checkmated or
        ** stalemated.
        */
        move_t move;

        /* If the opponent can capture the king that means we're
        ** checkmated.
        */
        board->current_player = OPPONENT(board->current_player);
        if (compute_legal_moves(board, moves) < 0)
        {
            /* We're checkmated. */
            move.type = RESIGN_MOVE;
            board->current_player = OPPONENT(board->current_player);
            return move;
        }
        else
        {
            /* We're stalemated. */
            move.type = STALEMATE_MOVE;
            board->current_player = OPPONENT(board->current_player);
            return move;
        }
    }

    return *best_move;
}
