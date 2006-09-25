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
#include <errno.h>
#include <string.h>

#include "board.h"
#include "move.h"
#include "hashing.h"

/* square_bit[i] is a bitboard that marks square 'i' on the board. */
bitboard_t square_bit[64];

/* Pieces values are used to calculate the material balance of the board. */
static int piece_value[] = {100, 100, 300, 300, 350, 350, 500, 500, 900, 900,
                            2000, 2000};

static void add_piece(board_t *board, int square, int piece)
/* Adds a piece to a board.
** Parameters: (board_t *) board: Pointer to the board to add the piece to.
**             (int) piece: The piece to add.
**             (int) square: The square where the piece should be added.
*/
{
    board->bitboard[piece] |= square_bit[square];
    board->bitboard[ALL + (piece & 1)] |= square_bit[square];
    board->material_value[piece & 1] += piece_value[piece];

    if ((piece & PIECE_MASK) == PAWN)
        board->num_pawns[piece & 1]++;

    board->hash_key ^= pieces_hash[piece][square];
}

static void remove_piece(board_t *board, int square, int piece)
/* Removes a piece from the board.
** Parameters: (board_t *) board: Pointer to the board to remove the piece
**                from.
**             (int) piece: The piece to remove.
**             (int) square: The square where to remove the piece from.
*/
{
    board->bitboard[piece] ^= square_bit[square];
    board->bitboard[ALL + (piece & 1)] ^= square_bit[square];
    board->material_value[piece & 1] -= piece_value[piece];

    if ((piece & PIECE_MASK) == PAWN)
        board->num_pawns[piece & 1]--;

    board->hash_key ^= pieces_hash[piece][square];
}

void setup_board(board_t *board)
{
    int i;

    clear_board(board);

    add_piece(board, SQUARE_A1, WHITE_ROOK);
    add_piece(board, SQUARE_B1, WHITE_KNIGHT);
    add_piece(board, SQUARE_C1, WHITE_BISHOP);
    add_piece(board, SQUARE_D1, WHITE_QUEEN);
    add_piece(board, SQUARE_E1, WHITE_KING);
    add_piece(board, SQUARE_F1, WHITE_BISHOP);
    add_piece(board, SQUARE_G1, WHITE_KNIGHT);
    add_piece(board, SQUARE_H1, WHITE_ROOK);
    add_piece(board, SQUARE_A8, BLACK_ROOK);
    add_piece(board, SQUARE_B8, BLACK_KNIGHT);
    add_piece(board, SQUARE_C8, BLACK_BISHOP);
    add_piece(board, SQUARE_D8, BLACK_QUEEN);
    add_piece(board, SQUARE_E8, BLACK_KING);
    add_piece(board, SQUARE_F8, BLACK_BISHOP);
    add_piece(board, SQUARE_G8, BLACK_KNIGHT);
    add_piece(board, SQUARE_H8, BLACK_ROOK);

    for (i = SQUARE_A2; i <= SQUARE_H2; i++)
        add_piece(board, i, WHITE_PAWN);

    for (i = SQUARE_A7; i <= SQUARE_H7; i++)
        add_piece(board, i, BLACK_PAWN);

    board->castle_flags = WHITE_CAN_CASTLE_KINGSIDE
                          | BLACK_CAN_CASTLE_KINGSIDE
                          | WHITE_CAN_CASTLE_QUEENSIDE
                          | BLACK_CAN_CASTLE_QUEENSIDE;

    board->en_passant = 0LL;

    board->current_player = SIDE_WHITE;

    board->hash_key = hash_key(board);

    board->fifty_moves = 0;
}

int setup_board_fen(board_t *board, char *fen)
{
	int i = 0;
	int square = 56;
	int len = strlen(fen);
	char *endptr;
	int done = 0;

	clear_board(board);

	while (done < 64)
	{
		int j;
		char c;
		char piece=0;

		if (i == len)
			return 1;

		c = fen[i++];

		if ((c >= '1') && (c <= '8'))
		{
			square += c - '0';
			done += c - '0';
			continue;
		}

		switch(toupper(c))
		{
			case 'P':
				piece = PAWN;
				break;
			case 'N':
				piece = KNIGHT;
				break;
			case 'B':
				piece = BISHOP;
				break;
			case 'R':
				piece = ROOK;
				break;
			case 'Q':
				piece = QUEEN;
				break;
			case 'K':
				piece = KING;
				break;
			case '/':
				square -= 16;
				continue;
		}

		if (c >= 'a')
			piece += SIDE_BLACK;
		else
			piece += SIDE_WHITE;

		if (square < 0 || square > 63)
			return 1;

		add_piece(board, square++, piece);
		done++;
	}

	/* Skip space */
	if (++i >= len)
		return 1;

	if (fen[i] == 'w')
		board->current_player = SIDE_WHITE;
	else if (fen[i] == 'b')
		board->current_player = SIDE_BLACK;
	else
		return 1;

	i += 2;

	if (i >= len)
		return 1;

	board->castle_flags = 0;

	if (fen[i] != '-')
		while ((fen[i] != ' ') && (i < len))
			switch (fen[i++])
			{
			case 'K':
				board->castle_flags |= WHITE_CAN_CASTLE_KINGSIDE;
				break;
			case 'Q':
				board->castle_flags |= WHITE_CAN_CASTLE_QUEENSIDE;
				break;
			case 'k':
				board->castle_flags |= BLACK_CAN_CASTLE_KINGSIDE;
				break;
			case 'q':
				board->castle_flags |= BLACK_CAN_CASTLE_QUEENSIDE;
			}
	else
		i++;

	/* Skip space */
	if (++i >= len)
		return 1;

	board->en_passant = 0LL;

	if (fen[i] != '-')
	{
		if ((fen[i] < 'a') || (fen[i] > 'h'))
			return 1;

		board->en_passant = 1LL << (fen[i++] - 'a');

		if (i >= len)
			return 1;

		if ((fen[i] < '1') || (fen[i] > '8'))
			return 1;

		board->en_passant <<= 8 * (fen[i] - '1');
	}

	i += 2;

	if (i >= len)
		return 1;

	errno = 0;
	board->fifty_moves = strtol(fen + i, &endptr, 10);
	if (errno || (fen + i == endptr))
		return 1;

	i = endptr - fen + 1;

	if (i >= len)
		return 1;

	/* FIXME Implement move counter, legality check */

	return 0;
}

void board_init()
{
    int i;
    for (i = 0; i < 64; i++)
        square_bit[i] = 1LL << i;
}

void clear_board(board_t *board)
{
    int i;
    for (i = 0; i < NR_BITBOARDS; i++)
        board->bitboard[i] = 0LL;

    board->num_pawns[SIDE_WHITE] = 0;
    board->num_pawns[SIDE_BLACK] = 0;

    board->material_value[SIDE_WHITE] = 0;
    board->material_value[SIDE_BLACK] = 0;
}

int find_black_piece(board_t *board, int square)
{
    bitboard_t mask = square_bit[square];

    /* We need to check kings first because a fake king might be on the
    ** same square as a rook. In that case we want to find the king so
    ** that the illegality of the previous move can be detected.
    */
    if (board->bitboard[BLACK_KING] & square_bit[square])
        return BLACK_KING;

    /* Check for other pieces in order of frequency. */
    if (board->bitboard[BLACK_PAWN] & mask)
        return BLACK_PAWN;
    if (board->bitboard[BLACK_KNIGHT] & mask)
        return BLACK_KNIGHT;
    if (board->bitboard[BLACK_BISHOP] & mask)
        return BLACK_BISHOP;
    if (board->bitboard[BLACK_ROOK] & mask)
        return BLACK_ROOK;
    if (board->bitboard[BLACK_QUEEN] & mask)
        return BLACK_QUEEN;

    return NONE;
}

int find_white_piece(board_t *board, int square)
{
    bitboard_t mask = square_bit[square];

    /* We need to check kings first because a fake king might be on the
    ** same square as a rook. In that case we want to find the king so
    ** that the illegality of the previous move can be detected.
    */
    if (board->bitboard[WHITE_KING] & mask)
        return WHITE_KING;

    /* Check for other pieces in order of frequency. */
    if (board->bitboard[WHITE_PAWN] & mask)
        return WHITE_PAWN;
    if (board->bitboard[WHITE_KNIGHT] & mask)
        return WHITE_KNIGHT;
    if (board->bitboard[WHITE_BISHOP] & mask)
        return WHITE_BISHOP;
    if (board->bitboard[WHITE_ROOK] & mask)
        return WHITE_ROOK;
    if (board->bitboard[WHITE_QUEEN] & mask)
        return WHITE_QUEEN;

    return NONE;
}

void execute_move(board_t *board, move_t *move)
{
    if (board->current_player)
    {
        /* Black is the side to move. Remove white phantom kings from the
        ** board.
        */
        if (board->castle_flags & WHITE_PHANTOM_KINGS_KINGSIDE)
        {
            board->castle_flags ^= WHITE_PHANTOM_KINGS_KINGSIDE;
            board->bitboard[WHITE_KING] ^= WHITE_PHANTOM_KINGSIDE;
            board->bitboard[WHITE_ALL] ^= WHITE_PHANTOM_KINGSIDE;
            board->bitboard[WHITE_ALL] |= board->bitboard[WHITE_ROOK];
        }
        else if (board->castle_flags & WHITE_PHANTOM_KINGS_QUEENSIDE)
        {
            board->castle_flags ^= WHITE_PHANTOM_KINGS_QUEENSIDE;
            board->bitboard[WHITE_KING] ^= WHITE_PHANTOM_QUEENSIDE;
            board->bitboard[WHITE_ALL] ^= WHITE_PHANTOM_QUEENSIDE;
            board->bitboard[WHITE_ALL] |= board->bitboard[WHITE_ROOK];
        }
    }
    else
    {
        /* White is the side to move. Remove black phantom kings from the
        ** board.
        */
        if (board->castle_flags & BLACK_PHANTOM_KINGS_KINGSIDE)
        {
            board->castle_flags ^= BLACK_PHANTOM_KINGS_KINGSIDE;
            board->bitboard[BLACK_KING] ^= BLACK_PHANTOM_KINGSIDE;
            board->bitboard[BLACK_ALL] ^= BLACK_PHANTOM_KINGSIDE;
            board->bitboard[BLACK_ALL] |= board->bitboard[BLACK_ROOK];
        }
        else if (board->castle_flags & BLACK_PHANTOM_KINGS_QUEENSIDE)
        {
            board->castle_flags ^= BLACK_PHANTOM_KINGS_QUEENSIDE;
            board->bitboard[BLACK_KING] ^= BLACK_PHANTOM_QUEENSIDE;
            board->bitboard[BLACK_ALL] ^= BLACK_PHANTOM_QUEENSIDE;
            board->bitboard[BLACK_ALL] |= board->bitboard[BLACK_ROOK];
        }
    }

    switch (move->type & MOVE_NO_PROMOTION_MASK)
    {
    case NORMAL_MOVE:
        remove_piece(board, move->source, move->piece);
        add_piece(board, move->destination, move->piece);
        if ((move->piece & PIECE_MASK) == PAWN)
            board->fifty_moves = 0;
        else
            board->fifty_moves++;
        break;

    case CAPTURE_MOVE:
        remove_piece(board, move->source, move->piece);
        remove_piece(board, move->destination, move->captured_piece);
        add_piece(board, move->destination, move->piece);
        board->fifty_moves = 0;
        break;

    case CAPTURE_MOVE_EN_PASSENT:
        remove_piece(board, move->source, move->piece);

        /* Remove the captured pawn. */
        remove_piece(board, move->destination + (move->piece & 1? 8 : -8),
                     move->captured_piece);
        add_piece(board, move->destination, move->piece);
        board->fifty_moves = 0;
        break;

    case CASTLING_MOVE_KINGSIDE:
        {
            /* We have to move the rook as well. */
            int rook = ROOK + board->current_player;

            remove_piece(board, move->source, move->piece);
            add_piece(board, move->destination, move->piece);
            remove_piece(board, move->destination + 1, rook);
            add_piece(board, move->destination - 1, rook);

            /* Add phantom kings to detect whether or not
            ** this move is legal.
            */
            if (!board->current_player)
            {
                /* White castling. */
                board->castle_flags |= WHITE_PHANTOM_KINGS_KINGSIDE
                                       | WHITE_HAS_CASTLED;
                board->bitboard[WHITE_KING] |= WHITE_PHANTOM_KINGSIDE;
                board->bitboard[WHITE_ALL] |= WHITE_PHANTOM_KINGSIDE;
            }
            else
            {
                /* Black castling. */
                board->castle_flags |= BLACK_PHANTOM_KINGS_KINGSIDE
                                       | BLACK_HAS_CASTLED;
                board->bitboard[BLACK_KING] |= BLACK_PHANTOM_KINGSIDE;
                board->bitboard[BLACK_ALL] |= BLACK_PHANTOM_KINGSIDE;
            }
            board->fifty_moves++;
            break;
        }
    case CASTLING_MOVE_QUEENSIDE:
        {
            /* We have to move the rook as well. */
            int rook = ROOK + board->current_player;

            remove_piece(board, move->source, move->piece);
            add_piece(board, move->destination, move->piece);
            remove_piece(board, move->destination - 2, rook);
            add_piece(board, move->destination + 1, rook);

            /* Add phantom kings to detect whether or not
            ** this move is legal.
            */
            if (!board->current_player)
            {
                /* White castling. */
                board->castle_flags |= WHITE_PHANTOM_KINGS_QUEENSIDE
                                       | WHITE_HAS_CASTLED;
                board->bitboard[WHITE_KING] |= WHITE_PHANTOM_QUEENSIDE;
                board->bitboard[WHITE_ALL] |= WHITE_PHANTOM_QUEENSIDE;
            }
            else
            {
                /* Black castling. */
                board->castle_flags |= BLACK_PHANTOM_KINGS_QUEENSIDE
                                       | BLACK_HAS_CASTLED;
                board->bitboard[BLACK_KING] |= BLACK_PHANTOM_QUEENSIDE;
                board->bitboard[BLACK_ALL] |= BLACK_PHANTOM_QUEENSIDE;
            }
            board->fifty_moves++;
            break;
        }
    case RESIGN_MOVE:
        /* Resign or checkmate, do nothing. */
        return;
    case STALEMATE_MOVE:
        /* Stalemate, do nothing. */
        return;
    }

    /* Promotion moves. */
    switch (move->type & MOVE_PROMOTION_MASK)
    {
    case PROMOTION_MOVE_KNIGHT:
        remove_piece(board, move->destination, move->piece);
        add_piece(board, move->destination, KNIGHT + board->current_player);
        break;
    case PROMOTION_MOVE_BISHOP:
        remove_piece(board, move->destination, move->piece);
        add_piece(board, move->destination, BISHOP + board->current_player);
        break;
    case PROMOTION_MOVE_ROOK:
        remove_piece(board, move->destination, move->piece);
        add_piece(board, move->destination, ROOK + board->current_player);
        break;
    case PROMOTION_MOVE_QUEEN:
        remove_piece(board, move->destination, move->piece);
        add_piece(board, move->destination, QUEEN + board->current_player);
    }

    /* Reset en passant possibility. */
    if (board->en_passant)
    {
        int square;
        for (square = 0; square < 64; square++)
        {
            if (board->en_passant & square_bit[square])
            {
                board->hash_key ^= ep_hash[square];
                break;
            }
        }
        board->en_passant = 0LL;
    }

    /* Set en passant possibility in case of a double pawn push. */
    if ((move->piece == WHITE_PAWN) && (move->destination - move->source == 16))
    {
        board->en_passant = square_bit[move->source + 8];
        board->hash_key ^= ep_hash[move->source + 8];
    }
    else if ((move->piece == BLACK_PAWN) && (move->source -
             move->destination == 16))
    {
        board->en_passant = square_bit[move->destination + 8];
        board->hash_key ^= ep_hash[move->destination + 8];
    }

    /* Set castling possibilities. If the king moves castling is not allowed. */
    switch (move->piece)
    {
    case WHITE_KING:
        if (board->castle_flags & WHITE_CAN_CASTLE_KINGSIDE)
        {
            board->hash_key ^= castle_hash[SIDE_WHITE + CASTLE_KINGSIDE];
            board->castle_flags ^= WHITE_CAN_CASTLE_KINGSIDE;
        }
        if (board->castle_flags & WHITE_CAN_CASTLE_QUEENSIDE)
        {
            board->hash_key ^= castle_hash[SIDE_WHITE + CASTLE_QUEENSIDE];
            board->castle_flags ^= WHITE_CAN_CASTLE_QUEENSIDE;
        }
        break;
    case BLACK_KING:
        if (board->castle_flags & BLACK_CAN_CASTLE_KINGSIDE)
        {
            board->hash_key ^= castle_hash[SIDE_BLACK + CASTLE_KINGSIDE];
            board->castle_flags ^= BLACK_CAN_CASTLE_KINGSIDE;
        }
        if (board->castle_flags & BLACK_CAN_CASTLE_QUEENSIDE)
        {
            board->hash_key ^= castle_hash[SIDE_BLACK + CASTLE_QUEENSIDE];
            board->castle_flags ^= BLACK_CAN_CASTLE_QUEENSIDE;
        }
    }

    /* Any activety in the corners will make castling impossible, either
    ** because the rook moves, or because it is captured.
    */
    switch (move->source)
    {
    case SQUARE_A1:
        if (board->castle_flags & WHITE_CAN_CASTLE_QUEENSIDE)
        {
            board->hash_key ^= castle_hash[SIDE_WHITE + CASTLE_QUEENSIDE];
            board->castle_flags ^= WHITE_CAN_CASTLE_QUEENSIDE;
        }
        break;
    case SQUARE_H1:
        if (board->castle_flags & WHITE_CAN_CASTLE_KINGSIDE)
        {
            board->hash_key ^= castle_hash[SIDE_WHITE + CASTLE_KINGSIDE];
            board->castle_flags ^= WHITE_CAN_CASTLE_KINGSIDE;
        }
        break;
    case SQUARE_A8:
        if (board->castle_flags & BLACK_CAN_CASTLE_QUEENSIDE)
        {
            board->hash_key ^= castle_hash[SIDE_BLACK + CASTLE_QUEENSIDE];
            board->castle_flags ^= BLACK_CAN_CASTLE_QUEENSIDE;
        }
        break;
    case SQUARE_H8:
        if (board->castle_flags & BLACK_CAN_CASTLE_KINGSIDE)
        {
            board->hash_key ^= castle_hash[SIDE_BLACK + CASTLE_KINGSIDE];
            board->castle_flags ^= BLACK_CAN_CASTLE_KINGSIDE;
        }
    }
    switch (move->destination)
    {
    case SQUARE_A1:
        if (board->castle_flags & WHITE_CAN_CASTLE_QUEENSIDE)
        {
            board->hash_key ^= castle_hash[SIDE_WHITE + CASTLE_QUEENSIDE];
            board->castle_flags ^= WHITE_CAN_CASTLE_QUEENSIDE;
        }
        break;
    case SQUARE_H1:
        if (board->castle_flags & WHITE_CAN_CASTLE_KINGSIDE)
        {
            board->hash_key ^= castle_hash[SIDE_WHITE + CASTLE_KINGSIDE];
            board->castle_flags ^= WHITE_CAN_CASTLE_KINGSIDE;
        }
        break;
    case SQUARE_A8:
        if (board->castle_flags & BLACK_CAN_CASTLE_QUEENSIDE)
        {
            board->hash_key ^= castle_hash[SIDE_BLACK + CASTLE_QUEENSIDE];
            board->castle_flags ^= BLACK_CAN_CASTLE_QUEENSIDE;
        }
        break;
    case SQUARE_H8:
        if (board->castle_flags & BLACK_CAN_CASTLE_KINGSIDE)
        {
            board->hash_key ^= castle_hash[SIDE_BLACK + CASTLE_KINGSIDE];
            board->castle_flags ^= BLACK_CAN_CASTLE_KINGSIDE;
        }
    }

    /* Switch players. */
    board->current_player = OPPONENT(board->current_player);
    board->hash_key ^= black_to_move;
}

void unmake_move(board_t *board, move_t *move, bitboard_t
                 old_en_passant,                int
                 old_castle_flags, int old_fifty_moves)
{
    int castle_diff;

    /* Switch players. */
    board->current_player = OPPONENT(board->current_player);
    board->hash_key ^= black_to_move;

    /* Promotion moves. */
    switch (move->type & MOVE_PROMOTION_MASK)
    {
    case PROMOTION_MOVE_KNIGHT:
        remove_piece(board, move->destination, KNIGHT + board->current_player);
        add_piece(board, move->destination, move->piece);
        break;
    case PROMOTION_MOVE_BISHOP:
        remove_piece(board, move->destination, BISHOP + board->current_player);
        add_piece(board, move->destination, move->piece);
        break;
    case PROMOTION_MOVE_ROOK:
        remove_piece(board, move->destination, ROOK + board->current_player);
        add_piece(board, move->destination, move->piece);
        break;
    case PROMOTION_MOVE_QUEEN:
        remove_piece(board, move->destination, QUEEN + board->current_player);
        add_piece(board, move->destination, move->piece);
    }

    switch (move->type & MOVE_NO_PROMOTION_MASK)
    {
    case NORMAL_MOVE:
        remove_piece(board, move->destination, move->piece);
        add_piece(board, move->source, move->piece);
        break;

    case CAPTURE_MOVE:
        remove_piece(board, move->destination, move->piece);
        add_piece(board, move->destination, move->captured_piece);
        add_piece(board, move->source, move->piece);
        break;

    case CAPTURE_MOVE_EN_PASSENT:
        remove_piece(board, move->destination, move->piece);
        add_piece(board, move->destination + (move->piece & 1? 8 : -8),
                  move->captured_piece);
        add_piece(board, move->source, move->piece);
        break;

    case CASTLING_MOVE_KINGSIDE:
        {
            /* We have to move the rook as well. */
            int rook = ROOK + board->current_player;

            /* Remove the phantom kings. */
            if (!board->current_player)
            {
                board->bitboard[WHITE_KING] ^= WHITE_PHANTOM_KINGSIDE;
                board->bitboard[WHITE_ALL] ^= WHITE_PHANTOM_KINGSIDE;

                /* Put back the rook. */
                board->bitboard[WHITE_ALL] |= board->bitboard[WHITE_ROOK];
                board->castle_flags ^= WHITE_HAS_CASTLED
                                       | WHITE_PHANTOM_KINGS_KINGSIDE;
            }
            else
            {
                board->bitboard[BLACK_KING] ^= BLACK_PHANTOM_KINGSIDE;
                board->bitboard[BLACK_ALL] ^= BLACK_PHANTOM_KINGSIDE;

                /* Put back the rook. */
                board->bitboard[BLACK_ALL] |= board->bitboard[BLACK_ROOK];
                board->castle_flags ^= BLACK_HAS_CASTLED
                                       | BLACK_PHANTOM_KINGS_KINGSIDE;
            }

            remove_piece(board, move->destination - 1, rook);
            add_piece(board, move->destination + 1, rook);
            remove_piece(board, move->destination, move->piece);
            add_piece(board, move->source, move->piece);

            break;
        }
    case CASTLING_MOVE_QUEENSIDE:
        {
            /* We have to move the rook as well. */
            int rook = ROOK + board->current_player;

            /* Remove the phantom kings. */
            if (!board->current_player)
            {
                board->bitboard[WHITE_KING] ^= WHITE_PHANTOM_QUEENSIDE;
                board->bitboard[WHITE_ALL] ^= WHITE_PHANTOM_QUEENSIDE;

                /* Put back the rook. */
                board->bitboard[WHITE_ALL] |= board->bitboard[WHITE_ROOK];
                board->castle_flags ^= WHITE_HAS_CASTLED
                                       | WHITE_PHANTOM_KINGS_QUEENSIDE;
            }
            else
            {
                board->bitboard[BLACK_KING] ^= BLACK_PHANTOM_QUEENSIDE;
                board->bitboard[BLACK_ALL] ^= BLACK_PHANTOM_QUEENSIDE;

                /* Put back the rook. */
                board->bitboard[BLACK_ALL] |= board->bitboard[BLACK_ROOK];
                board->castle_flags ^= BLACK_HAS_CASTLED
                                       | BLACK_PHANTOM_KINGS_QUEENSIDE;
            }

            remove_piece(board, move->destination + 1, rook);
            add_piece(board, move->destination - 2, rook);
            remove_piece(board, move->destination, move->piece);
            add_piece(board, move->source, move->piece);

            break;
        }
    case RESIGN_MOVE:
        /* Resign or checkmate, do nothing. */
        return;
    case STALEMATE_MOVE:
        /* Stalemate, do nothing. */
        return;
    }

    /* Restore en passant possibility. */
    if (board->en_passant || old_en_passant)
    {
        int square;
        for (square = 0; square < 64; square++)
        {
            if (board->en_passant & square_bit[square])
                board->hash_key ^= ep_hash[square];
            if (old_en_passant & square_bit[square])
                board->hash_key ^= ep_hash[square];
        }
        board->en_passant = old_en_passant;
    }

    castle_diff = board->castle_flags ^ old_castle_flags;

    /* Restore phantom kings. */
    switch ((castle_diff & PHANTOM_FLAGS) & old_castle_flags)
    {
    case WHITE_PHANTOM_KINGS_KINGSIDE:
        board->castle_flags |= WHITE_PHANTOM_KINGS_KINGSIDE;
        board->bitboard[WHITE_KING] |= WHITE_PHANTOM_KINGSIDE;
        board->bitboard[WHITE_ALL] |= WHITE_PHANTOM_KINGSIDE;
        break;

    case WHITE_PHANTOM_KINGS_QUEENSIDE:
        board->castle_flags |= WHITE_PHANTOM_KINGS_QUEENSIDE;
        board->bitboard[WHITE_KING] |= WHITE_PHANTOM_QUEENSIDE;
        board->bitboard[WHITE_ALL] |= WHITE_PHANTOM_QUEENSIDE;
        break;

    case BLACK_PHANTOM_KINGS_KINGSIDE:
        board->castle_flags |= BLACK_PHANTOM_KINGS_KINGSIDE;
        board->bitboard[BLACK_KING] |= BLACK_PHANTOM_KINGSIDE;
        board->bitboard[BLACK_ALL] |= BLACK_PHANTOM_KINGSIDE;
        break;

    case BLACK_PHANTOM_KINGS_QUEENSIDE:
        board->castle_flags |= BLACK_PHANTOM_KINGS_QUEENSIDE;
        board->bitboard[BLACK_KING] |= BLACK_PHANTOM_QUEENSIDE;
        board->bitboard[BLACK_ALL] |= BLACK_PHANTOM_QUEENSIDE;
        break;
    }

    /* Restore castle flags. */
    if (castle_diff & 15)
    {
        int i;
        for (i = 0; i < 4; i++)
            if (castle_diff & (1 << i))
                board->hash_key ^= castle_hash[i];
        board->castle_flags = old_castle_flags;
    }
    board->fifty_moves = old_fifty_moves;
}
