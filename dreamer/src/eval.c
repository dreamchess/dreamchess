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
#include "move.h"
#include "move_data.h"
#include "eval.h"

static int
min(int a, int b)
{
    if (a < b)
        return a;
    else
        return b;
}

static int
max(int a, int b)
{
    if (a > b)
        return a;
    else
        return b;
}

static int
eval_king_tropism(board_t *board, int side)
{
    int score = 0;
    int square;
    /* Avoid compiler warnings. */
    int king_rank = 0;
    int king_file = 0;
    int piece_rank;
    int piece_file;
    if (side == SIDE_WHITE)
    {
        for (square = 0; square < 64; square++)
            if (board->bitboard[BLACK_KING] & square_bit[square])
            {
                king_rank = square >> 3;
                king_file = square & 7;
                break;
            }
        /* printf("King on square: %i\n", square); */
        for (square = 0; square < 64; square++)
        {
            piece_rank = square >> 3;
            piece_file = square & 7;

            if (board->bitboard[WHITE_ROOK] & square_bit[square])
                score -= min(abs(king_rank - piece_rank),
                             abs(king_file - piece_file)) << 1;
            else
                if (board->bitboard[WHITE_KNIGHT] & square_bit[square])
                    score += 5 - abs(king_rank - piece_rank)
                             - abs(king_file - piece_file);
                else
                    if (board->bitboard[WHITE_QUEEN] & square_bit[square])
                        score -= min(abs(king_rank - piece_rank),
                                     abs(king_file - piece_file));
        }
    }
    else
    {
        for (square = 0; square < 64; square++)
            if (board->bitboard[WHITE_KING] & square_bit[square])
            {
                king_rank = square >> 3;
                king_file = square & 7;
                break;
            }
        /* printf("King on square: %i\n", square); */
        for (square = 0; square < 64; square++)
        {
            piece_rank = square >> 3;
            piece_file = square & 7;

            if (board->bitboard[BLACK_ROOK] & square_bit[square])
                score -= min(abs(king_rank - piece_rank),
                             abs(king_file - piece_file)) << 1;
            else
                if (board->bitboard[BLACK_KNIGHT] & square_bit[square])
                    score += 5 - abs(king_rank - piece_rank)
                             - abs(king_file - piece_file);
                else
                    if (board->bitboard[BLACK_QUEEN] & square_bit[square])
                        score -= min(abs(king_rank - piece_rank),
                                     abs(king_file - piece_file));
        }
    }

    return score;
}

static int
eval_rook_bonus(board_t *board, eval_data_t *eval_data, int side)
{
    int score = 0;
    int square;
    bitboard_t bitboard = board->bitboard[ROOK + side];
    if (side == SIDE_WHITE)
    {
        for (square = 0; square < 64; square++)
            if (bitboard & square_bit[square])
            {
                int piece_rank = square >> 3;
                int piece_file = square & 7;

                if (piece_rank == 6)
                    score += 22;

                if (eval_data->max_pawn_file_bins[piece_file] == 0)
                {
                    if (eval_data->min_pawn_file_bins[piece_file] == 0)
                        score += 10;
                    else
                        score += 4;
                }

                if (square < eval_data->max_passed_pawns[piece_file])
                    score += 25;
                bitboard ^= square_bit[square];

                if (!bitboard)
                    return score;
            }
    }
    else
    {
        for (square = 63; square >= 0; square--)
            if (bitboard & square_bit[square])
            {
                int piece_rank = square >> 3;
                int piece_file = square & 7;

                if (piece_rank == 1)
                    score += 22;

                if (eval_data->max_pawn_file_bins[piece_file] == 0)
                {
                    if (eval_data->min_pawn_file_bins[piece_file] == 0)
                        score += 10;
                    else
                        score += 4;
                }

                if (square > eval_data->max_passed_pawns[piece_file])
                    score += 25;
                bitboard ^= square_bit[square];

                if (!bitboard)
                    return score;
            }
    }

    /* Avoid compiler warnings. */
    return 0;
}

static int
eval_development(board_t *board, int side)
{
    int score = 0;
    int square;
    bitboard_t bitboard;

    if (side == SIDE_WHITE)
    {
        if (board->bitboard[WHITE_PAWN] & square_bit[SQUARE_D2])
            score -= 15;
        if (board->bitboard[WHITE_PAWN] & square_bit[SQUARE_E2])
            score -= 15;

        bitboard = board->bitboard[WHITE_KNIGHT] |
                   board->bitboard[WHITE_BISHOP];

        for (square = SQUARE_A1; square <= SQUARE_H1; square++)
        {
            if (bitboard & square_bit[square])
                score -= 10;
        }

        bitboard = board->bitboard[WHITE_QUEEN];

        if (bitboard && !(bitboard & square_bit[SQUARE_D1]))
        {
            int count = 0;
            if (board->bitboard[WHITE_ROOK] & square_bit[SQUARE_A1])
                count++;
            if (board->bitboard[WHITE_ROOK] & square_bit[SQUARE_H1])
                count++;
            if (board->bitboard[WHITE_KNIGHT] & square_bit[SQUARE_B1])
                count++;
            if (board->bitboard[WHITE_KNIGHT] & square_bit[SQUARE_G1])
                count++;
            if (board->bitboard[WHITE_BISHOP] & square_bit[SQUARE_C1])
                count++;
            if (board->bitboard[WHITE_BISHOP] & square_bit[SQUARE_F1])
                count++;
            /* if (board->bitboard[WHITE_KING] & square_bit[SQUARE_E1])
                count++; */

            score -= count << 3;
        }

        if (board->bitboard[BLACK_QUEEN])
        {
            if (board->castle_flags & WHITE_HAS_CASTLED)
                score += 10;
            else
                if ((board->castle_flags & WHITE_CAN_CASTLE_KINGSIDE) &&
                        (board->castle_flags & WHITE_CAN_CASTLE_QUEENSIDE))
                    score -= 24;
                else
                    if (board->castle_flags & WHITE_CAN_CASTLE_KINGSIDE)
                        score -= 40;
                    else
                        if (board->castle_flags & WHITE_CAN_CASTLE_QUEENSIDE)
                            score -= 80;
                        else
                            score -= 120;
        }
    }
    else
    {
        if (board->bitboard[BLACK_PAWN] & square_bit[SQUARE_D7])
            score -= 15;
        if (board->bitboard[BLACK_PAWN] & square_bit[SQUARE_E7])
            score -= 15;

        bitboard = board->bitboard[BLACK_KNIGHT] |
                   board->bitboard[BLACK_BISHOP];

        for (square = SQUARE_A8; square <= SQUARE_H8; square++)
        {
            if (bitboard & square_bit[square])
                score -= 10;
        }

        bitboard = board->bitboard[BLACK_QUEEN];

        if (bitboard && !(bitboard & square_bit[SQUARE_D8]))
        {
            int count = 0;
            if (board->bitboard[BLACK_ROOK] & square_bit[SQUARE_A8])
                count++;
            if (board->bitboard[BLACK_ROOK] & square_bit[SQUARE_H8])
                count++;
            if (board->bitboard[BLACK_KNIGHT] & square_bit[SQUARE_B8])
                count++;
            if (board->bitboard[BLACK_KNIGHT] & square_bit[SQUARE_G8])
                count++;
            if (board->bitboard[BLACK_BISHOP] & square_bit[SQUARE_C8])
                count++;
            if (board->bitboard[BLACK_BISHOP] & square_bit[SQUARE_F8])
                count++;
            /* if (board->bitboard[BLACK_KING] & square_bit[SQUARE_E8])
                count++; */

            score -= count << 3;
        }

        if (board->bitboard[WHITE_QUEEN])
        {
            if (board->castle_flags & BLACK_HAS_CASTLED)
                score += 10;
            else
                if ((board->castle_flags & BLACK_CAN_CASTLE_KINGSIDE) &&
                        (board->castle_flags & BLACK_CAN_CASTLE_QUEENSIDE))
                    score -= 24;
                else
                    if (board->castle_flags & BLACK_CAN_CASTLE_KINGSIDE)
                        score -= 40;
                    else
                        if (board->castle_flags & BLACK_CAN_CASTLE_QUEENSIDE)
                            score -= 80;
                        else
                            score -= 120;
        }
    }

    return score;
}

static int
eval_bad_bishops(board_t *board, eval_data_t *eval_data, int side)
{
    int square;
    int score = 0;
    bitboard_t bitboard = board->bitboard[BISHOP + side];

    if (!bitboard)
        return 0;

    for (square = 0; square < 64; square++)
    {
        if (bitboard & square_bit[square])
        {
            int piece_rank = square >> 3;
            int piece_file = square & 7;

            if ((piece_rank & 1) == (piece_file & 1))
                score -= eval_data->max_pawn_color_bins[0] << 3;
            else
                score -= eval_data->max_pawn_color_bins[1] << 3;

            bitboard ^= square_bit[square];

            if (!bitboard)
                return score;
        }
    }

    return 0;
}

static int
eval_pawn_structure(board_t *board, eval_data_t *eval_data, int side)
{
    int score = 0;
    int bin;

    for (bin = 0; bin < 8; bin++)
        if (eval_data->max_pawn_file_bins[bin] > 1)
            score -= 8;

    if ((eval_data->max_pawn_file_bins[0] > 0) &&
            eval_data->max_pawn_file_bins[1] == 0)
        score -= 15;
    if ((eval_data->max_pawn_file_bins[7] > 0) &&
            eval_data->max_pawn_file_bins[6] == 0)
        score -= 15;

    for (bin = 1; bin < 7; bin++)
        if ((eval_data->max_pawn_file_bins[bin] > 0) &&
                (eval_data->max_pawn_file_bins[bin - 1] == 0) &&
                (eval_data->max_pawn_file_bins[bin + 1] == 0))
            score -= 15;

    if (eval_data->max_total_pawns == 8)
        score -= 10;

    score -= 8 * eval_data->pawn_rams;

    if (side == SIDE_WHITE)
    {
        for (bin = 0; bin < 8; bin++)
            if (eval_data->max_passed_pawns[bin] > 0)
                score += (eval_data->max_passed_pawns[bin] >> 3) *
                         (eval_data->max_passed_pawns[bin] >> 3);
    }
    else
    {
        for (bin = 0; bin < 8; bin++)
            if (eval_data->max_passed_pawns[bin] < 63)
                score += (7 - (eval_data->max_passed_pawns[bin] >> 3)) *
                         (7 - (eval_data->max_passed_pawns[bin] >> 3));
    }

    return score;
}

static void
analyze_pawn_structure(board_t *board, eval_data_t *eval_data, int side)
{
    int i;
    for (i = 0; i < 8; i++)
    {
        eval_data->max_pawn_file_bins[i] = 0;
        eval_data->min_pawn_file_bins[i] = 0;
    }

    eval_data->max_pawn_color_bins[0] = 0;
    eval_data->max_pawn_color_bins[1] = 0;
    eval_data->pawn_rams = 0;
    eval_data->max_total_pawns = 0;

    if (side == SIDE_WHITE)
    {
        int square;

        for (i = 0; i < 8; i++)
        {
            eval_data->max_most_advanced[i] = 0;
            eval_data->min_most_backward[i] = 0;
            eval_data->max_passed_pawns[i] = 0;
        }

        for (square = 8; square <= 55; square++)
        {
            if (board->bitboard[WHITE_PAWN] & square_bit[square])
            {
                int piece_rank = square >> 3;
                int piece_file = square & 7;

                eval_data->max_pawn_file_bins[piece_file]++;
                eval_data->max_total_pawns++;
                eval_data->max_most_advanced[piece_file] = square;

                if ((piece_rank & 1) == (piece_file & 1))
                    eval_data->max_pawn_color_bins[0]++;
                else
                    eval_data->max_pawn_color_bins[1]++;

                if (board->bitboard[BLACK_PAWN] & square_bit[square + 8])
                    eval_data->pawn_rams++;
            }
            else
                if (board->bitboard[BLACK_PAWN] & square_bit[square])
                {
                    int piece_file = square & 7;
                    eval_data->min_pawn_file_bins[piece_file]++;
                    eval_data->min_most_backward[piece_file] = square;
                }
        }

        if (eval_data->max_most_advanced[0] >
                max(eval_data->min_most_backward[0],
                    eval_data->min_most_backward[1]))
            eval_data->max_passed_pawns[0] = eval_data->max_most_advanced[0];

        if (eval_data->max_most_advanced[7] >
                max(eval_data->min_most_backward[6],
                    eval_data->min_most_backward[7]))
            eval_data->max_passed_pawns[7] = eval_data->max_most_advanced[7];

        for (i = 1; i < 7; i++)
            if ((eval_data->max_most_advanced[i] >
                    eval_data->min_most_backward[i-1]) &&
                    (eval_data->max_most_advanced[i] >
                     eval_data->min_most_backward[i]) &&
                    (eval_data->max_most_advanced[i] >
                     eval_data->min_most_backward[i+1]))
                eval_data->max_passed_pawns[i] =
                    eval_data->max_most_advanced[i];
    }
    else
    {
        int square;

        for (i = 0; i < 8; i++)
        {
            eval_data->max_most_advanced[i] = 63;
            eval_data->min_most_backward[i] = 63;
            eval_data->max_passed_pawns[i] = 63;
        }

        for (square = 55; square >= 8; square--)
        {
            if (board->bitboard[BLACK_PAWN] & square_bit[square])
            {
                int piece_rank = square >> 3;
                int piece_file = square & 7;

                eval_data->max_pawn_file_bins[piece_file]++;
                eval_data->max_total_pawns++;
                eval_data->max_most_advanced[piece_file] = square;

                if ((piece_rank & 1) == (piece_file & 1))
                    eval_data->max_pawn_color_bins[0]++;
                else
                    eval_data->max_pawn_color_bins[1]++;

                if (board->bitboard[WHITE_PAWN] & square_bit[square - 8])
                    eval_data->pawn_rams++;
            }
            else
                if (board->bitboard[WHITE_PAWN] & square_bit[square])
                {
                    int piece_file = square & 7;
                    eval_data->min_pawn_file_bins[piece_file]++;
                    eval_data->min_most_backward[piece_file] = square;
                }
        }

        if (eval_data->max_most_advanced[0] <
                min(eval_data->min_most_backward[0],
                    eval_data->min_most_backward[1]))
            eval_data->max_passed_pawns[0] = eval_data->max_most_advanced[0];

        if (eval_data->max_most_advanced[7] <
                min(eval_data->min_most_backward[6],
                    eval_data->min_most_backward[7]))
            eval_data->max_passed_pawns[7] = eval_data->max_most_advanced[7];

        for (i = 1; i < 7; i++)
            if ((eval_data->max_most_advanced[i] <
                    eval_data->min_most_backward[i-1]) &&
                    (eval_data->max_most_advanced[i] <
                     eval_data->min_most_backward[i]) &&
                    (eval_data->max_most_advanced[i] <
                     eval_data->min_most_backward[i+1]))
                eval_data->max_passed_pawns[i] =
                    eval_data->max_most_advanced[i];
    }
}

static int
board_eval_material(board_t *board, int side)
{
    int mat_total;

    if (board->material_value[SIDE_BLACK] ==
            board->material_value[SIDE_WHITE])
        return 0;

    mat_total = board->material_value[SIDE_WHITE] +
                board->material_value[SIDE_BLACK];

    if (board->material_value[SIDE_BLACK] >
            board->material_value[SIDE_WHITE])
    {
        int mat_diff = board->material_value[SIDE_BLACK] -
                       board->material_value[SIDE_WHITE];
        int val = (2400 < mat_diff? 2400 : mat_diff) +
                  (mat_diff * (12000 - mat_total) *
                   board->num_pawns[SIDE_BLACK])
                  / (6400 * (board->num_pawns[SIDE_BLACK ] + 1 ));
        if (side == SIDE_BLACK)
            return val;
        else
            return -val;
    }
    else
    {
        int mat_diff = board->material_value[SIDE_WHITE] -
                       board->material_value[SIDE_BLACK];
        int val = (2400 < mat_diff? 2400 : mat_diff) +
                  (mat_diff * (12000 - mat_total) *
                   board->num_pawns[SIDE_WHITE])
                  / (6400 * (board->num_pawns[SIDE_WHITE] + 1 ));
        if (side == SIDE_WHITE)
            return val;
        else
            return -val;
    }
}

int
board_eval_quick(board_t *board, int side)
{
    int eval = board_eval_material(board, side);
    if (board->current_player == side)
        return eval;
    else
        return -eval;
}

int
board_eval_complete(board_t *board, int side, int alpha, int beta)
{
    eval_data_t eval_data;
    int eval2;
    int eval1 = board_eval_material(board, side);

    if (board->current_player != side)
        eval1 = -eval1;
#if 0
    if (eval1 - 200 >= beta)
        return beta;

    if (eval1 + 200 <= alpha)
        return alpha;
#endif
    analyze_pawn_structure(board, &eval_data, side);
    eval2 = eval_pawn_structure(board, &eval_data, side) +
            eval_bad_bishops(board, &eval_data, side) +
            eval_development(board, side) +
            eval_rook_bonus(board, &eval_data, side) +
            eval_king_tropism(board, side) + 192; /* Add 192 to have the starting position score 0 */

    if (board->current_player == side)
        return eval1 + eval2;
    else
        return eval1 - eval2;
}

