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

#include "ChessBoard.h"
#include "san.h"
//#include "debug.h"

//static int move_is_semi_valid(board_t *board, move_t *move);
//static int in_check(board_t *board, int turn);

void Board::boardSetup() {
    int i;

    for (i = 16; i < 48; i++)
        square[i] = NONE;

    square[0] = WHITE_ROOK;
    square[1] = WHITE_KNIGHT;
    square[2] = WHITE_BISHOP;
    square[3] = WHITE_QUEEN;
    square[4] = WHITE_KING;
    square[5] = WHITE_BISHOP;
    square[6] = WHITE_KNIGHT;
    square[7] = WHITE_ROOK;
    square[56] = BLACK_ROOK;
    square[57] = BLACK_KNIGHT;
    square[58] = BLACK_BISHOP;
    square[59] = BLACK_QUEEN;
    square[60] = BLACK_KING;
    square[61] = BLACK_BISHOP;
    square[62] = BLACK_KNIGHT;
    square[63] = BLACK_ROOK;

    for (i = 8; i < 16; i++)
        square[i] = WHITE_PAWN;

    for (i = 48; i < 56; i++)
        square[i] = BLACK_PAWN;

    for (i = 0; i < 10; i++)
        captured[i] = 0;

    turn = WHITE;
    state = BOARD_NORMAL;
}

int Board::moveIsCapture(move_t *move)
{
    if (square[move->destination] != NONE)
        return 1;

    if ((PIECE(square[move->source]) == PAWN) &&
            ((move->destination - move->source) % 8 != 0))
        return 1;

    return 0;
}

static void square_to_str(char *buf, int square)
{
    buf[0] = (square % 8) + 'a';
    buf[1] = (square / 8) + '1';
}

char *Board::moveToFullalg(move_t *move) {
    char *s = (char *) malloc(6);
    char prom[4] = { 'n', 'b', 'r', 'q' };

    square_to_str(s, move->source);
    square_to_str(s + 2, move->destination);
    s[4] = '\0';
    s[5] = '\0';

    if ((PIECE(square[move->source]) == PAWN) && ((move->destination < 8) || (move->destination >= 56)))
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

    //DBG_ERROR("failed to convert user interface piece to SAN piece");
    exit(1);
}

int Board::makeMove(move_t *move)
{
    /* Assume that move is valid. */

    if ((PIECE(square[move->source]) == PAWN) && ((PIECE(square[move->destination]) == NONE)) && ((move->source % 8) != (move->destination % 8)))
    {
        /* En-passant move. */
        int ep = move->destination - 8 * (move->destination > move->source ? 1 : -1);
        captured[square[ep]]++;
        square[ep] = NONE;
    }

    /* Update captured pieces. */
    if (square[move->destination] != NONE)
        captured[square[move->destination]]++;

    if ((PIECE(square[move->source]) == KING) && (move->destination - move->source == 2))
    {
        /* Kingside castle. */
        square[move->destination - 1] = square[move->destination + 1];
        square[move->destination + 1] = NONE;
    }

    if ((PIECE(square[move->source]) == KING) && (move->source - move->destination == 2))
    {
        /* Queenside castle. */
        square[move->destination + 1] = square[move->destination - 2];
        square[move->destination - 2] = NONE;
    }

    if ((PIECE(square[move->source]) == PAWN) && (move->destination < 8 || move->destination >= 56))
        /* Promotion. */
        square[move->destination] = move->promotion_piece;
    else
        square[move->destination] = square[move->source];

    square[move->source] = NONE;
    turn = OPPONENT(turn);

    return 0;
}

#define HOR abs(move->source % 8 - move->destination % 8)
#define VERT abs(move->source / 8 - move->destination / 8)

int Board::rayOk(move_t *move)
{
    int step, i;
    if (VERT != 0)
        step = (move->destination - move->source) / VERT;
    else
        step = (move->destination > move->source ? 1 : -1);

    i = move->source + step;

    while (i != move->destination)
    {
        if (square[i] != NONE)
            return 0;
        i += step;
    }

    if (square[i] == NONE)
        return 1;

    if (COLOUR(square[i]) != COLOUR(square[move->source]))
        return 1;

    return 0;
}

int Board::squareAttacked(int squarei, int side) { /* FIXME MAYBE */
    move_t move;
    int i;

    turn = side;

    /* We add a piece because we only want to take capture moves into consideration. */
    square[squarei] = KING + OPPONENT(side);

    move.destination = squarei;

    for (i = 0; i < 64; i++)
    {
        if (COLOUR(square[i]) == side)
        {
            move.source = i;
            if ((PIECE(square[i]) == PAWN) && ((squarei < 8) || (squarei >= 56)))
                move.promotion_piece = QUEEN + side;
            else
                move.promotion_piece = NONE;
            if (moveIsSemiValid(&move))
                return 1;
        }
    }
    return 0;
}

/* Checks whether a move is valid, except for whether this puts own king in check. */
int Board::moveIsSemiValid(move_t *move)
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
    if (square[move->source] == NONE)
        return 0;

    /* Test for moving opponent's piece. */
    if (COLOUR(square[move->source]) != turn)
        return 0;

    /* Check that a promotion piece is specified for promotion moves. */
    if ((PIECE(square[move->source]) == PAWN) && ((move->destination < 8) || (move->destination >= 56)))
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

        if (COLOUR(move->promotion_piece) != turn)
            return 0;
    }
    else if (move->promotion_piece != NONE)
        /* Promotion piece specified for non-promotion move. */
        return 0;

    switch (PIECE(square[move->source]))
    {
    case KNIGHT:
        if ((HOR != 1) && (HOR != 2))
            return 0;
        if ((HOR == 1) && (VERT != 2))
            return 0;
        if ((HOR == 2) && (VERT != 1))
            return 0;
        if (square[move->destination] == NONE)
            break;
        if (COLOUR(square[move->destination]) == COLOUR(square[move->source]))
            return 0;
        break;
    case BISHOP:
        if (HOR != VERT)
            return 0;
        if (!rayOk(move))
            return 0;
        break;
    case ROOK:
        if ((HOR != 0) && (VERT != 0))
            return 0;
        if (!rayOk(move))
            return 0;
        break;
    case QUEEN:
        if ((HOR != 0) && (VERT != 0) && (HOR != VERT))
            return 0;
        if (!rayOk(move))
            return 0;
        break;
    case PAWN:
        /* Catch moves in wrong direction. */
        if ((move->destination > move->source) && (COLOUR(square[move->source]) == BLACK))
            return 0;
        if ((move->destination < move->source) && (COLOUR(square[move->source]) == WHITE))
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
            if (!rayOk(move) || (square[move->destination] != NONE))
                return 0;
        }
        else
        {
            if (VERT != 1)
                return 0;
            if (!rayOk(move))
                return 0;
            /* En-passant move. */
            if (square[move->destination] == NONE)
            {
                if ((COLOUR(square[move->source]) == WHITE) && !((move->source >= 32) && (move->source < 40)))
                    return 0;
                if ((COLOUR(square[move->source]) == BLACK) && !((move->source >= 24) && (move->source < 32)))
                    return 0;

                if (square[move->destination + (COLOUR(square[move->source]) == WHITE ? -8 : 8)] !=
                        PAWN + OPPONENT(COLOUR(square[move->source])))
                    return 0;
            }

        }
        break;
    case KING:
        if (HOR > 2)
            return 0;
        else if (HOR == 2)
        {
            int white = COLOUR(square[move->source]) == WHITE;

            int i, step = (move->destination > move->source ? 1 : -1);
            int rook = (step == 1 ? (white ? 7 : 63) : (white ? 0 : 56));

            /* Castling. */
            if (VERT != 0)
                return 0;

            if (move->source != (white ? 4 : 60))
                return 0;

            if (square[rook] != ROOK + COLOUR(square[move->source]))
                return 0;

            i = move->source + step;

            while (i != rook)
            {
                if (square[i] != NONE)
                    return 0;
                i += step;
            }

            /* Check whether any of the squares the king moves over is under
            ** attack. Note that destination square is checked later.
            */
            if (squareAttacked(move->source, (white ? BLACK : WHITE)))
                return 0;
            if (squareAttacked(move->source + step, (white ? BLACK : WHITE)))
                return 0;
        }
        else
        {
            if (VERT > 1)
                return 0;
            if (!rayOk(move))
                return 0;
        }
    }
    return 1;
}

int Board::inCheck(int turn)
{
    int i;
    for (i = 0; i < 64; i++)
        if (square[i] ==  KING + turn)
            break;

    if (i == 64) {
        //DBG_ERROR("board is missing a king");
        exit(1);
    }

    return squareAttacked(i, OPPONENT(turn));
}

int Board::moveIsValid(move_t *move)
{
    if (!moveIsSemiValid(move))
        return 0;

    makeMove(move);

    return !inCheck(turn);
}

int Board::isMated(int side)
{
    int src, dest;
    move_t move;

    for (src = 0; src < 64; src++)
        if (COLOUR(square[src]) == side)
            for (dest = 0; dest < 64; dest++)
            {
                move.source = src;
                move.destination = dest;
                if ((PIECE(square[src]) == PAWN) && ((dest < 8) || (dest >= 56)))
                    move.promotion_piece = QUEEN + side;
                else
                    move.promotion_piece = NONE;
                if (moveIsValid(&move))
                    return 0;
            }
    return 1;
}

void Board::moveSetAttr(move_t *move)
{
    int check, mated;

    if (moveIsCapture(move))
        move->type = CAPTURE;
    else
        move->type = NORMAL;

    if (PIECE(square[move->source]) == KING)
    {
        int hor = move->destination % 8 - move->source % 8;

        if (hor > 1)
            move->type = KINGSIDE_CASTLE;
        else if (hor < -1)
            move->type = QUEENSIDE_CASTLE;
    }

    makeMove(move);

    check = inCheck(turn);
    mated = isMated(turn);

    if (check && mated)
        move->state = MOVE_CHECKMATE;
    else if (check)
        move->state = MOVE_CHECK;
    else if (mated)
        move->state = MOVE_STALEMATE;
    else
        move->state = MOVE_NORMAL;

}

move_t *Board::fullalgToMove(char *move_s)
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
        move->promotion_piece += turn;
    }
    else
        move->promotion_piece = NONE;

    if (!moveIsValid(move))
    {
        free(move);
        return NULL;
    }

    moveSetAttr(move);

    return move;
}

static int ui_piece(int san_piece)
{
    switch (san_piece)
    {
    case SAN_PAWN:
        return PAWN;
    case SAN_KNIGHT:
        return KNIGHT;
    case SAN_BISHOP:
        return BISHOP;
    case SAN_ROOK:
        return ROOK;
    case SAN_QUEEN:
        return QUEEN;
    case SAN_KING:
        return KING;
    case SAN_NOT_SPECIFIED:
        return NONE;
    }

    //DBG_ERROR("failed to convert SAN piece to user interface piece");
    exit(1);
}

move_t *Board::findUniqueMove(san_move_t *san_move) {
    int squarei;
    int piece;
    int found = 0;
    move_t move, *retval;

    if (san_move->type == SAN_QUEENSIDE_CASTLE)
    {
        san_move->source_file = 4;
        san_move->source_rank = (turn == WHITE ? 0 : 7);
        san_move->destination = (turn == WHITE ? 2 : 58);
        piece = KING;
    }
    else if (san_move->type == SAN_KINGSIDE_CASTLE)
    {
        san_move->source_file = 4;
        san_move->source_rank = (turn == WHITE ? 0 : 7);
        san_move->destination = (turn == WHITE ? 6 : 62);
        piece = KING;
    }
    else
        piece = ui_piece(san_move->piece);

    piece += turn;

    for (squarei = 0; squarei < 64; squarei++)
    {
        move_t m;

        if (square[squarei] != piece)
            continue;

        /* We found a piece. */

        if (san_move->source_file != SAN_NOT_SPECIFIED)
            if (san_move->source_file != squarei % 8)
                continue;

        if (san_move->source_rank != SAN_NOT_SPECIFIED)
            if (san_move->source_rank != squarei / 8)
                continue;

        m.source = squarei;
        m.destination = san_move->destination;
        m.promotion_piece = ui_piece(san_move->promotion_piece);

        if (m.promotion_piece != NONE)
            m.promotion_piece += turn;

        if (moveIsValid(&m))
        {
            move = m;
            found++;
            if (found > 1)
                return NULL;
        }
    }

    if (!found)
    {
        //DBG_ERROR("failed to find a legal move corresponding to SAN move");
        return NULL;
    }

    retval = (move_t *) malloc(sizeof(move_t));
    *retval = move;
    return retval;
}

char *Board::moveToSan(move_t *move)
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

    san_move.piece = san_piece(square[move->source]);
    san_move.source_file = SAN_NOT_SPECIFIED;
    san_move.source_rank = SAN_NOT_SPECIFIED;
    san_move.destination = move->destination;
    if (move->promotion_piece != NONE)
        san_move.promotion_piece = san_piece(move->promotion_piece);
    else
        san_move.promotion_piece = SAN_NOT_SPECIFIED;

    if (san_move.piece == SAN_PAWN)
    {
        if (move->source % 8 != move->destination % 8)
            san_move.source_file = move->source % 8;
    }
    else
    {
        move_t *u_move;
        u_move = findUniqueMove(&san_move);

        if (!u_move)
        {
            san_move.source_file = move->source % 8;
            u_move = findUniqueMove(&san_move);
            if (!u_move)
            {
                san_move.source_file = SAN_NOT_SPECIFIED;
                san_move.source_rank = move->source / 8;
                u_move = findUniqueMove(&san_move);
                if (!u_move)
                {
                    san_move.source_file = move->source % 8;
                    u_move = findUniqueMove(&san_move);
                    if (!u_move)
                    {
                        char *move_s = moveToFullalg(move);

                        //DBG_ERROR("failed to convert move %s to SAN notation", move_s);

                        free(move_s);
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

move_t* Board::sanToMove(char *move_s)
{
    san_move_t *san_move = san_parse(move_s);

    if (!san_move)
    {
        //DBG_LOG("failed to parse SAN move string '%s'", move_s);
        return NULL;
    }

    return findUniqueMove(san_move);
}

char* Board::sanToFan(char *move_s)
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
