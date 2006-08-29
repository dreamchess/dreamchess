/*  DreamChess
**  Copyright (C) 2003-2004  The DreamChess project
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
#include <string.h>

#include "board.h"
#include "san.h"

static int move_is_semi_valid(board_t *board, move_t *move);
static int in_check(board_t *board, int turn);

void board_setup(board_t *board)
{
    int i;

    for (i = 16; i < 48; i++)
        board->square[i] = NONE;

    board->square[0] = WHITE_ROOK;
    board->square[1] = WHITE_KNIGHT;
    board->square[2] = WHITE_BISHOP;
    board->square[3] = WHITE_QUEEN;
    board->square[4] = WHITE_KING;
    board->square[5] = WHITE_BISHOP;
    board->square[6] = WHITE_KNIGHT;
    board->square[7] = WHITE_ROOK;
    board->square[56] = BLACK_ROOK;
    board->square[57] = BLACK_KNIGHT;
    board->square[58] = BLACK_BISHOP;
    board->square[59] = BLACK_QUEEN;
    board->square[60] = BLACK_KING;
    board->square[61] = BLACK_BISHOP;
    board->square[62] = BLACK_KNIGHT;
    board->square[63] = BLACK_ROOK;

    for (i = 8; i < 16; i++)
        board->square[i] = WHITE_PAWN;

    for (i = 48; i < 56; i++)
        board->square[i] = BLACK_PAWN;

    for (i = 0; i < 10; i++)
        board->captured[i] = 0;

    board->turn = WHITE;
    board->state = BOARD_NORMAL;
}

static int move_is_capture(board_t *board, move_t *move)
{
    if (board->square[move->destination] != NONE)
        return 1;

    if ((PIECE(board->square[move->source]) == PAWN) &&
            ((move->destination - move->source) % 8 != 0))
        return 1;

    return 0;
}

char *move_to_fullalg(board_t *board, move_t *move)
{
    char *s = (char *) malloc(6);
    char prom[4] = "nbrq";

    s[0] = (move->source % 8) + 'a';
    s[1] = (move->source / 8) + '1';
    s[2] = (move->destination % 8) + 'a';
    s[3] = (move->destination / 8) + '1';
    s[4] = '\0';
    s[5] = '\0';

    if ((PIECE(board->square[move->source]) == PAWN) && ((move->destination < 8) || (move->destination >= 56)))
        s[4] = prom[(move->promotion_piece - 2) / 2];
    return s;
}

static int san_piece(int piece)
{
    switch (PIECE(piece))
    {
    case KING:
        return SAN_KING;
    case QUEEN:
        return SAN_QUEEN;
    case ROOK:
        return SAN_ROOK;
    case BISHOP:
        return SAN_BISHOP;
    case KNIGHT:
        return SAN_KNIGHT;
    case PAWN:
        return SAN_PAWN;
    }

    exit(1);
}

int make_move(board_t *board, move_t *move)
{
    /* Assume that move is valid. */

    if ((PIECE(board->square[move->source]) == PAWN) && ((PIECE(board->square[move->destination]) == NONE)) && ((move->source % 8) != (move->destination % 8)))
    {
        /* En-passent move. */
        int ep = move->destination - 8 * (move->destination > move->source ? 1 : -1);
        board->captured[board->square[ep]]++;
        board->square[ep] = NONE;
    }

    /* Update captured pieces. */
    if (board->square[move->destination] != NONE)
        board->captured[board->square[move->destination]]++;

    if ((PIECE(board->square[move->source]) == KING) && (move->destination - move->source == 2))
    {
        /* Kingside castle. */
        board->square[move->destination - 1] = board->square[move->destination + 1];
        board->square[move->destination + 1] = NONE;
    }

    if ((PIECE(board->square[move->source]) == KING) && (move->source - move->destination == 2))
    {
        /* Queenside castle. */
        board->square[move->destination + 1] = board->square[move->destination - 2];
        board->square[move->destination - 2] = NONE;
    }

    if ((PIECE(board->square[move->source]) == PAWN) && (move->destination < 8 || move->destination >= 56))
        /* Promotion. */
        board->square[move->destination] = move->promotion_piece;
    else
        board->square[move->destination] = board->square[move->source];

    board->square[move->source] = NONE;
    board->turn = OPPONENT(board->turn);

    return 0;
}

#define HOR abs(move->source % 8 - move->destination % 8)
#define VERT abs(move->source / 8 - move->destination / 8)

static int ray_ok(board_t *board, move_t *move)
{
    int step, i;
    if (VERT != 0)
        step = (move->destination - move->source) / VERT;
    else
        step = (move->destination > move->source ? 1 : -1);

    i = move->source + step;

    while (i != move->destination)
    {
        if (board->square[i] != NONE)
            return 0;
        i += step;
    }

    if (board->square[i] == NONE)
        return 1;

    if (COLOUR(board->square[i]) != COLOUR(board->square[move->source]))
        return 1;

    return 0;
}

static int square_attacked(board_t *b, int square, int side)
{
    board_t board = *b;
    move_t move;
    int i;

    board.turn = side;

    /* We add a piece because we only want to take capture moves into consideration. */
    board.square[square] = KING + OPPONENT(side);

    move.destination = square;

    for (i = 0; i < 64; i++)
    {
        if (COLOUR(board.square[i]) == side)
        {
            move.source = i;
            if ((PIECE(board.square[i]) == PAWN) && ((square < 8) || (square >= 56)))
                move.promotion_piece = QUEEN + side;
            else
                move.promotion_piece = NONE;
            if (move_is_semi_valid(&board, &move))
            {
                /* printf("Found: %i-%i\n", move.source, move.destination); */
                return 1;
            }
        }
    }
    return 0;
}

/* Checks whether a move is valid, except for whether this puts own king in check. */
static int move_is_semi_valid(board_t *board, move_t *move)
{
    /* Bounds check. */
    if ((move->source > 63) || (move->source < 0))
        return 0;

    if ((move->destination > 63) || (move->destination < 0))
        return 0;

    /* Test for moving to same square. */
    if (move->source == move->destination)
        return 0;

    /* Test for empty source square. */
    if (board->square[move->source] == NONE)
        return 0;

    /* Test for moving opponent's piece. */
    if (COLOUR(board->square[move->source]) != board->turn)
        return 0;

    /* Check that a promotion piece is specified for promotion moves. */
    if ((PIECE(board->square[move->source]) == PAWN) && ((move->destination < 8) || (move->destination >= 56)))
    {
        switch (PIECE(move->promotion_piece))
        {
        case KNIGHT:
        case ROOK:
        case BISHOP:
        case QUEEN:
            break;
        default:
            return 0;
        }

        if (COLOUR(move->promotion_piece) != board->turn)
            return 0;
    }
    else if (move->promotion_piece != NONE)
        /* Promotion piece specified for non-promotion move. */
        return 0;

    switch (PIECE(board->square[move->source]))
    {
    case KNIGHT:
        if ((HOR != 1) && (HOR != 2))
            return 0;
        if ((HOR == 1) && (VERT != 2))
            return 0;
        if ((HOR == 2) && (VERT != 1))
            return 0;
        if (board->square[move->destination] == NONE)
            break;
        if (COLOUR(board->square[move->destination]) == COLOUR(board->square[move->source]))
            return 0;
        break;
    case BISHOP:
        if (HOR != VERT)
            return 0;
        if (!ray_ok(board, move))
            return 0;
        break;
    case ROOK:
        if ((HOR != 0) && (VERT != 0))
            return 0;
        if (!ray_ok(board, move))
            return 0;
        break;
    case QUEEN:
        if ((HOR != 0) && (VERT != 0) && (HOR != VERT))
            return 0;
        if (!ray_ok(board, move))
            return 0;
        break;
    case PAWN:
        /* Catch moves in wrong direction. */
        if ((move->destination > move->source) && (COLOUR(board->square[move->source]) == BLACK))
            return 0;
        if ((move->destination < move->source) && (COLOUR(board->square[move->source]) == WHITE))
            return 0;

        if (HOR > 1)
            return 0;

        if (HOR == 0)
        {
            /* Regular or double push. */

            if (VERT > 2)
                return 0;
            if (VERT == 2)
                if (!(((move->source >= 8) && (move->source <= 15)) || ((move->source >= 48) && (move->source <= 55))))
                    return 0;
            /* Use ray checking code with added requirement that destination square is empty. */
            if (!ray_ok(board, move) || (board->square[move->destination] != NONE))
                return 0;
        }
        else
        {
            if (VERT != 1)
                return 0;
            if (!ray_ok(board, move))
                return 0;
            /* En-passent move. */
            if (board->square[move->destination] == NONE)
            {
                if ((COLOUR(board->square[move->source]) == WHITE) && !((move->source >= 32) && (move->source < 40)))
                    return 0;
                if ((COLOUR(board->square[move->source]) == BLACK) && !((move->source >= 24) && (move->source < 32)))
                    return 0;

                if (board->square[move->destination + (COLOUR(board->square[move->source]) == WHITE ? -8 : 8)] !=
                        PAWN + OPPONENT(COLOUR(board->square[move->source])))
                    return 0;
            }

        }
        break;
    case KING:
        if (HOR > 2)
            return 0;
        else if (HOR == 2)
        {
            int white = COLOUR(board->square[move->source]) == WHITE;

            int i, step = (move->destination > move->source ? 1 : -1);
            int rook = (step == 1 ? (white ? 7 : 63) : (white ? 0 : 56));

            /* Castling. */
            if (VERT != 0)
                return 0;

            if (move->source != (white ? 4 : 60))
                return 0;

            if (board->square[rook] != ROOK + COLOUR(board->square[move->source]))
                return 0;

            i = move->source + step;

            while (i != rook)
            {
                if (board->square[i] != NONE)
                    return 0;
                i += step;
            }

            /* Check whether any of the squares the king moves over is under
            ** attack. Note that destination square is checked later.
            */
            if (square_attacked(board, move->source, (white ? BLACK : WHITE)))
                return 0;
            if (square_attacked(board, move->source + step, (white ? BLACK : WHITE)))
                return 0;
        }
        else
        {
            if (VERT > 1)
                return 0;
            if (!ray_ok(board, move))
                return 0;
        }
    }
    return 1;
}

static int in_check(board_t *board, int turn)
{
    int i;
    for (i = 0; i < 64; i++)
        if (board->square[i] ==  KING + turn)
            break;

    if (i == 64)
    {
        printf("Fatal error: No king on chessboard!\n");
        exit(1);
    }

    return square_attacked(board, i, OPPONENT(turn));
}

int move_is_valid(board_t *b, move_t *move)
{
    board_t board = *b;

    if (!move_is_semi_valid(&board, move))
        return 0;

    make_move(&board, move);

    return !in_check(&board, b->turn);
}

static int is_mated(board_t *board, int side)
{
    int src, dest;
    move_t move;

    for (src = 0; src < 64; src++)
        if (COLOUR(board->square[src]) == side)
            for (dest = 0; dest < 64; dest++)
            {
                move.source = src;
                move.destination = dest;
                if ((PIECE(board->square[src]) == PAWN) && ((dest < 8) || (dest >= 56)))
                    move.promotion_piece = QUEEN + side;
                else
                    move.promotion_piece = NONE;
                if (move_is_valid(board, &move))
                    return 0;
            }
    return 1;
}

void move_set_attr(board_t *b, move_t *move)
{
    int check, mated;
    board_t board = *b;

    if (move_is_capture(b, move))
        move->type = CAPTURE;
    else
        move->type = NORMAL;

    if (PIECE(b->square[move->source]) == KING)
    {
        int hor = move->destination % 8 - move->source % 8;

        if (hor > 1)
            move->type = KINGSIDE_CASTLE;
        else if (hor < -1)
            move->type = QUEENSIDE_CASTLE;
    }

    make_move(&board, move);

    check = in_check(&board, board.turn);
    mated = is_mated(&board, board.turn);

    if (check && mated)
        move->state = MOVE_CHECKMATE;
    else if (check)
        move->state = MOVE_CHECK;
    else if (mated)
        move->state = MOVE_STALEMATE;
    else
        move->state = MOVE_NORMAL;

}

move_t *fullalg_to_move(board_t *board, char *move_s)
{
    move_t *move;

    if ((strlen(move_s) < 4) || (strlen(move_s) > 5))
        return NULL;
    if ((move_s[0] < 'a') || (move_s[0] > 'h'))
        return NULL;
    if ((move_s[1] < '1') || (move_s[1] > '8'))
        return NULL;
    if ((move_s[2] < 'a') || (move_s[2] > 'h'))
        return NULL;
    if ((move_s[3] < '1') || (move_s[3] > '8'))
        return NULL;
    if ((strlen(move_s) == 5) && (move_s[4] != 'n')
            && (move_s[4] != 'b') && (move_s[4] != 'r')
            && (move_s[4] != 'q'))
        return NULL;

    move = (move_t *) malloc(sizeof(move_t));
    move->source = move_s[0] - 'a' + (move_s[1] - '1') * 8;
    move->destination = move_s[2] - 'a' + (move_s[3] - '1') * 8;

    if (strlen(move_s) == 5)
    {
        switch (move_s[4])
        {
        case 'n':
            move->promotion_piece = KNIGHT;
            break;
        case 'b':
            move->promotion_piece = BISHOP;
            break;
        case 'r':
            move->promotion_piece = ROOK;
            break;
        case 'q':
            move->promotion_piece = QUEEN;
        }
        move->promotion_piece += board->turn;
    }
    else
        move->promotion_piece = NONE;

    if (!move_is_valid(board, move))
    {
        free(move);
        return NULL;
    }

    move_set_attr(board, move);

    return move;
}

static move_t* find_unique_move(board_t *board, san_move_t *san_move)
{
    int square;
    int piece;
    int found = 0;
    move_t move, *retval;

    if (san_move->type == SAN_QUEENSIDE_CASTLE)
    {
        san_move->source_file = 4;
        san_move->source_rank = (board->turn == WHITE ? 0 : 7);
        san_move->destination = (board->turn == WHITE ? 2 : 58);
        piece = KING;
    }
    else if (san_move->type == SAN_KINGSIDE_CASTLE)
    {
        san_move->source_file = 4;
        san_move->source_rank = (board->turn == WHITE ? 0 : 7);
        san_move->destination = (board->turn == WHITE ? 6 : 62);
        piece = KING;
    }
    else
        switch (san_move->piece)
        {
        case SAN_PAWN:
            piece = PAWN;
            break;
        case SAN_KNIGHT:
            piece = KNIGHT;
            break;
        case SAN_BISHOP:
            piece = BISHOP;
            break;
        case SAN_ROOK:
            piece = ROOK;
            break;
        case SAN_QUEEN:
            piece = QUEEN;
            break;
        case SAN_KING:
            piece = KING;
            break;
        default:
            exit(1);
        }

    piece += board->turn;

    for (square = 0; square < 64; square++)
    {
        move_t m;

        if (board->square[square] != piece)
            continue;

        /* We found a piece. */

        if (san_move->source_file != SAN_NOT_SPECIFIED)
            if (san_move->source_file != square % 8)
                continue;

        if (san_move->source_rank != SAN_NOT_SPECIFIED)
            if (san_move->source_rank != square / 8)
                continue;

        m.source = square;
        m.destination = san_move->destination;
        if (san_move->promotion_piece != SAN_NOT_SPECIFIED)
            m.promotion_piece = san_move->promotion_piece;
        else
            m.promotion_piece = NONE;

        if (move_is_valid(board, &m))
        {
            move = m;
            found++;
            if (found > 1)
            {
                fprintf(stderr, "More than one SAN match found\n");
                return NULL;
            }
            /* printf("Found candidate at %i\n", square); */
        }
    }

    if (!found)
    {
        fprintf(stderr, "No SAN match found\n");
        return NULL;
    }

    retval = (move_t *) malloc(sizeof(move_t));
    *retval = move;
    return retval;
}

char *move_to_san(board_t *board, move_t *move)
{
    san_move_t san_move;

    switch (move->state)
    {
    case MOVE_CHECK:
        san_move.state = SAN_STATE_CHECK;
        break;
    case MOVE_CHECKMATE:
        san_move.state = SAN_STATE_CHECKMATE;
        break;
    default:
        san_move.state = SAN_STATE_NORMAL;
    }

    switch (move->type)
    {
    case QUEENSIDE_CASTLE:
        san_move.type = SAN_QUEENSIDE_CASTLE;
        return san_string(&san_move);
    case KINGSIDE_CASTLE:
        san_move.type = SAN_KINGSIDE_CASTLE;
        return san_string(&san_move);
    case CAPTURE:
        san_move.type = SAN_CAPTURE;
        break;
    default:
        san_move.type = SAN_NORMAL;
    }

    san_move.piece = san_piece(board->square[move->source]);
    san_move.source_file = SAN_NOT_SPECIFIED;
    san_move.source_rank = SAN_NOT_SPECIFIED;
    san_move.destination = move->destination;
    if (move->promotion_piece != NONE)
        san_move.promotion_piece = san_piece(move->promotion_piece);
    else
        san_move.promotion_piece = NONE;

    if (san_move.piece == SAN_PAWN)
    {
        if (move->source % 8 != move->destination % 8)
            san_move.source_file = move->source % 8;
    }
    else
    {
        move_t *u_move;
        u_move = find_unique_move(board, &san_move);
        if (!u_move)
        {
            san_move.source_file = move->source % 8;
            u_move = find_unique_move(board, &san_move);
            if (!u_move)
            {
                san_move.source_file = SAN_NOT_SPECIFIED;
                san_move.source_rank = move->source / 8;
                u_move = find_unique_move(board, &san_move);
                if (!u_move)
                {
                    san_move.source_file = move->source % 8;
                    u_move = find_unique_move(board, &san_move);
                    if (!u_move)
                    {
                        fprintf(stderr, "Couldn't convert move to SAN.\n");
                        return NULL;
                    }
                }
                else
                    free(u_move);
            }
            else
                free(u_move);
        }
        else
            free(u_move);
    }

    return san_string(&san_move);
}

move_t* san_to_move(board_t *board, char *move_s)
{
    san_move_t *san_move = san_parse(move_s);

    if (!san_move)
        return NULL;

    return find_unique_move(board, san_move);
}

char* san_to_fan(board_t *board, char *move_s)
{
    char *move = strdup(move_s);

    switch (move[0])
    {
    case 'K':
        move[0] = CHAR_KING;
        break;
    case 'Q':
        move[0] = CHAR_QUEEN;
        break;
    case 'R':
        move[0] = CHAR_ROOK;
        break;
    case 'N':
        move[0] = CHAR_KNIGHT;
        break;
    case 'B':
        move[0] = CHAR_BISHOP;
    }

    return move;
}
