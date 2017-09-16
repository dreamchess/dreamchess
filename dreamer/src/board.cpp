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
#include <errno.h>
#include <string.h>
#include <ctype.h>

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

int setup_board_fen(board_t *board, const char *fen)
{
	int i = 0;
	int square = 56;
	int len = strlen(fen);
	char *endptr;
	int done = 0;

	clear_board(board);

	while (done < 64)
	{
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
		while ((i < len) && (fen[i] != ' '))
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

void board_init(void)
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

void execute_move(board_t *board, Move move)
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

    switch (move.getType())
    {
    case Move::Type::Normal:
        remove_piece(board, move.getSource(), move.getPiece());
        add_piece(board, move.getDest(), move.getPiece());
        if (move.getPieceKind() == PAWN)
            board->fifty_moves = 0;
        else
            board->fifty_moves++;
        break;

    case Move::Type::Capture:
        remove_piece(board, move.getSource(), move.getPiece());
        remove_piece(board, move.getDest(), move.getCapturedPiece());
        add_piece(board, move.getDest(), move.getPiece());
        board->fifty_moves = 0;
        break;

    case Move::Type::Promotion:
        remove_piece(board, move.getSource(), PAWN + board->current_player);
        add_piece(board, move.getDest(), move.getPiece());
        board->fifty_moves = 0;
        break;

    case Move::Type::PromotionCapture:
        remove_piece(board, move.getSource(), PAWN + board->current_player);
        remove_piece(board, move.getDest(), move.getCapturedPiece());
        add_piece(board, move.getDest(), move.getPiece());
        board->fifty_moves = 0;
        break;

    case Move::Type::EnPassant:
        remove_piece(board, move.getSource(), move.getPiece());

        /* Remove the captured pawn. */
        remove_piece(board, move.getDest() + (move.getPieceColour() == SIDE_BLACK ? 8 : -8),
                     move.getCapturedPiece());
        add_piece(board, move.getDest(), move.getPiece());
        board->fifty_moves = 0;
        break;

    case Move::Type::KingsideCastle:
        {
            /* We have to move the rook as well. */
            int rook = ROOK + board->current_player;

            remove_piece(board, move.getSource(), move.getPiece());
            add_piece(board, move.getDest(), move.getPiece());
            remove_piece(board, move.getDest() + 1, rook);
            add_piece(board, move.getDest() - 1, rook);

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
    case Move::Type::QueensideCastle:
        {
            /* We have to move the rook as well. */
            int rook = ROOK + board->current_player;

            remove_piece(board, move.getSource(), move.getPiece());
            add_piece(board, move.getDest(), move.getPiece());
            remove_piece(board, move.getDest() - 2, rook);
            add_piece(board, move.getDest() + 1, rook);

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
    case Move::Type::Resign:
        /* Resign or checkmate, do nothing. */
    case Move::Type::Stalemate:
        /* Stalemate, do nothing. */
    case Move::Type::None:
        return;
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
    if ((move.getPiece() == WHITE_PAWN) && (move.getDest() -
        move.getSource() == 16))
    {
        board->en_passant = square_bit[move.getSource() + 8];
        board->hash_key ^= ep_hash[move.getSource() + 8];
    }
    else if ((move.getPiece() == BLACK_PAWN) && (move.getSource() -
             move.getDest() == 16))
    {
        board->en_passant = square_bit[move.getDest() + 8];
        board->hash_key ^= ep_hash[move.getDest() + 8];
    }

    /* Set castling possibilities. If the king moves castling is not allowed. */
    switch (move.getPiece())
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
    switch (move.getSource())
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
    switch (move.getDest())
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

void unmake_move(board_t *board, Move move, bitboard_t
                 old_en_passant,                int
                 old_castle_flags, int old_fifty_moves)
{
    int castle_diff;

    /* Switch players. */
    board->current_player = OPPONENT(board->current_player);
    board->hash_key ^= black_to_move;

    switch (move.getType())
    {
    case Move::Type::Normal:
        remove_piece(board, move.getDest(), move.getPiece());
        add_piece(board, move.getSource(), move.getPiece());
        break;

    case Move::Type::Capture:
        remove_piece(board, move.getDest(), move.getPiece());
        add_piece(board, move.getDest(), move.getCapturedPiece());
        add_piece(board, move.getSource(), move.getPiece());
        break;

    case Move::Type::Promotion:
        remove_piece(board, move.getDest(), move.getPiece());
        add_piece(board, move.getSource(), PAWN + board->current_player);
        break;

    case Move::Type::PromotionCapture:
        remove_piece(board, move.getDest(), move.getPiece());
        add_piece(board, move.getDest(), move.getCapturedPiece());
        add_piece(board, move.getSource(), PAWN + board->current_player);
        break;

    case Move::Type::EnPassant:
        remove_piece(board, move.getDest(), move.getPiece());
        add_piece(board, move.getDest() + (move.getPiece() & 1 ? 8 : -8),
                  move.getCapturedPiece());
        add_piece(board, move.getSource(), move.getPiece());
        break;

    case Move::Type::KingsideCastle:
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

            remove_piece(board, move.getDest() - 1, rook);
            add_piece(board, move.getDest() + 1, rook);
            remove_piece(board, move.getDest(), move.getPiece());
            add_piece(board, move.getSource(), move.getPiece());

            break;
        }
    case Move::Type::QueensideCastle:
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

            remove_piece(board, move.getDest() + 1, rook);
            add_piece(board, move.getDest() - 2, rook);
            remove_piece(board, move.getDest(), move.getPiece());
            add_piece(board, move.getSource(), move.getPiece());

            break;
        }
    case Move::Type::Resign:
        /* Resign or checkmate, do nothing. */
    case Move::Type::Stalemate:
        /* Stalemate, do nothing. */
    case Move::Type::None:
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
