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

void Board::addPiece(int square, int piece)
/* Adds a piece to a board.
** Parameters: (board_t *) board: Pointer to the board to add the piece to.
**             (int) piece: The piece to add.
**             (int) square: The square where the piece should be added.
*/
{
    bitboard[piece] |= square_bit[square];
    bitboard[ALL + (piece & 1)] |= square_bit[square];
    material_value[piece & 1] += piece_value[piece];

    if ((piece & PIECE_MASK) == PAWN)
        num_pawns[piece & 1]++;

    hash_key ^= pieces_hash[piece][square];
}

void Board::removePiece(int square, int piece)
/* Removes a piece from the board.
** Parameters: (board_t *) board: Pointer to the board to remove the piece
**                from.
**             (int) piece: The piece to remove.
**             (int) square: The square where to remove the piece from.
*/
{
    bitboard[piece] ^= square_bit[square];
    bitboard[ALL + (piece & 1)] ^= square_bit[square];
    material_value[piece & 1] -= piece_value[piece];

    if ((piece & PIECE_MASK) == PAWN)
        num_pawns[piece & 1]--;

    hash_key ^= pieces_hash[piece][square];
}

void Board::setup()
{
    int i;

    clear();

    addPiece(SQUARE_A1, WHITE_ROOK);
    addPiece(SQUARE_B1, WHITE_KNIGHT);
    addPiece(SQUARE_C1, WHITE_BISHOP);
    addPiece(SQUARE_D1, WHITE_QUEEN);
    addPiece(SQUARE_E1, WHITE_KING);
    addPiece(SQUARE_F1, WHITE_BISHOP);
    addPiece(SQUARE_G1, WHITE_KNIGHT);
    addPiece(SQUARE_H1, WHITE_ROOK);
    addPiece(SQUARE_A8, BLACK_ROOK);
    addPiece(SQUARE_B8, BLACK_KNIGHT);
    addPiece(SQUARE_C8, BLACK_BISHOP);
    addPiece(SQUARE_D8, BLACK_QUEEN);
    addPiece(SQUARE_E8, BLACK_KING);
    addPiece(SQUARE_F8, BLACK_BISHOP);
    addPiece(SQUARE_G8, BLACK_KNIGHT);
    addPiece(SQUARE_H8, BLACK_ROOK);

    for (i = SQUARE_A2; i <= SQUARE_H2; i++)
        addPiece(i, WHITE_PAWN);

    for (i = SQUARE_A7; i <= SQUARE_H7; i++)
        addPiece(i, BLACK_PAWN);

    castle_flags = WHITE_CAN_CASTLE_KINGSIDE
                   | BLACK_CAN_CASTLE_KINGSIDE
                   | WHITE_CAN_CASTLE_QUEENSIDE
                   | BLACK_CAN_CASTLE_QUEENSIDE;

    en_passant = 0LL;

    current_player = SIDE_WHITE;

    hash_key = ::hash_key(*this);

    fifty_moves = 0;
}

bool Board::setupFEN(const char *fen)
{
	int i = 0;
	int square = 56;
	int len = strlen(fen);
	char *endptr;
	int done = 0;

	clear();

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

		addPiece(square++, piece);
		done++;
	}

	/* Skip space */
	if (++i >= len)
		return 1;

	if (fen[i] == 'w')
		current_player = SIDE_WHITE;
	else if (fen[i] == 'b')
		current_player = SIDE_BLACK;
	else
		return 1;

	i += 2;

	if (i >= len)
		return 1;

	castle_flags = 0;

	if (fen[i] != '-')
		while ((i < len) && (fen[i] != ' '))
			switch (fen[i++])
			{
			case 'K':
				castle_flags |= WHITE_CAN_CASTLE_KINGSIDE;
				break;
			case 'Q':
				castle_flags |= WHITE_CAN_CASTLE_QUEENSIDE;
				break;
			case 'k':
				castle_flags |= BLACK_CAN_CASTLE_KINGSIDE;
				break;
			case 'q':
				castle_flags |= BLACK_CAN_CASTLE_QUEENSIDE;
			}
	else
		i++;

	/* Skip space */
	if (++i >= len)
		return 1;

	en_passant = 0LL;

	if (fen[i] != '-')
	{
		if ((fen[i] < 'a') || (fen[i] > 'h'))
			return 1;

		en_passant = 1LL << (fen[i++] - 'a');

		if (i >= len)
			return 1;

		if ((fen[i] < '1') || (fen[i] > '8'))
			return 1;

		en_passant <<= 8 * (fen[i] - '1');
	}

	i += 2;

	if (i >= len)
		return 1;

	errno = 0;
	fifty_moves = strtol(fen + i, &endptr, 10);
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

void Board::clear()
{
    int i;
    for (i = 0; i < NR_BITBOARDS; i++)
        bitboard[i] = 0LL;

    num_pawns[SIDE_WHITE] = 0;
    num_pawns[SIDE_BLACK] = 0;

    material_value[SIDE_WHITE] = 0;
    material_value[SIDE_BLACK] = 0;
}

void Board::makeMove(Move move)
{
    if (current_player)
    {
        /* Black is the side to move. Remove white phantom kings from the
        ** board.
        */
        if (castle_flags & WHITE_PHANTOM_KINGS_KINGSIDE)
        {
            castle_flags ^= WHITE_PHANTOM_KINGS_KINGSIDE;
            bitboard[WHITE_KING] ^= WHITE_PHANTOM_KINGSIDE;
            bitboard[WHITE_ALL] ^= WHITE_PHANTOM_KINGSIDE;
            bitboard[WHITE_ALL] |= bitboard[WHITE_ROOK];
        }
        else if (castle_flags & WHITE_PHANTOM_KINGS_QUEENSIDE)
        {
            castle_flags ^= WHITE_PHANTOM_KINGS_QUEENSIDE;
            bitboard[WHITE_KING] ^= WHITE_PHANTOM_QUEENSIDE;
            bitboard[WHITE_ALL] ^= WHITE_PHANTOM_QUEENSIDE;
            bitboard[WHITE_ALL] |= bitboard[WHITE_ROOK];
        }
    }
    else
    {
        /* White is the side to move. Remove black phantom kings from the
        ** board.
        */
        if (castle_flags & BLACK_PHANTOM_KINGS_KINGSIDE)
        {
            castle_flags ^= BLACK_PHANTOM_KINGS_KINGSIDE;
            bitboard[BLACK_KING] ^= BLACK_PHANTOM_KINGSIDE;
            bitboard[BLACK_ALL] ^= BLACK_PHANTOM_KINGSIDE;
            bitboard[BLACK_ALL] |= bitboard[BLACK_ROOK];
        }
        else if (castle_flags & BLACK_PHANTOM_KINGS_QUEENSIDE)
        {
            castle_flags ^= BLACK_PHANTOM_KINGS_QUEENSIDE;
            bitboard[BLACK_KING] ^= BLACK_PHANTOM_QUEENSIDE;
            bitboard[BLACK_ALL] ^= BLACK_PHANTOM_QUEENSIDE;
            bitboard[BLACK_ALL] |= bitboard[BLACK_ROOK];
        }
    }

    switch (move.getType())
    {
    case Move::Type::Normal:
        removePiece(move.getSource(), move.getPiece());
        addPiece(move.getDest(), move.getPiece());
        if (move.getPieceKind() == PAWN)
            fifty_moves = 0;
        else
            fifty_moves++;
        break;

    case Move::Type::Capture:
        removePiece(move.getSource(), move.getPiece());
        removePiece(move.getDest(), move.getCapturedPiece());
        addPiece(move.getDest(), move.getPiece());
        fifty_moves = 0;
        break;

    case Move::Type::Promotion:
        removePiece(move.getSource(), PAWN + current_player);
        addPiece(move.getDest(), move.getPiece());
        fifty_moves = 0;
        break;

    case Move::Type::PromotionCapture:
        removePiece(move.getSource(), PAWN + current_player);
        removePiece(move.getDest(), move.getCapturedPiece());
        addPiece(move.getDest(), move.getPiece());
        fifty_moves = 0;
        break;

    case Move::Type::EnPassant:
        removePiece(move.getSource(), move.getPiece());

        /* Remove the captured pawn. */
        removePiece(move.getDest() + (move.getPieceColour() == SIDE_BLACK ? 8 : -8),
                     move.getCapturedPiece());
        addPiece(move.getDest(), move.getPiece());
        fifty_moves = 0;
        break;

    case Move::Type::KingsideCastle:
        {
            /* We have to move the rook as well. */
            int rook = ROOK + current_player;

            removePiece(move.getSource(), move.getPiece());
            addPiece(move.getDest(), move.getPiece());
            removePiece(move.getDest() + 1, rook);
            addPiece(move.getDest() - 1, rook);

            /* Add phantom kings to detect whether or not
            ** this move is legal.
            */
            if (!current_player)
            {
                /* White castling. */
                castle_flags |= WHITE_PHANTOM_KINGS_KINGSIDE | WHITE_HAS_CASTLED;
                bitboard[WHITE_KING] |= WHITE_PHANTOM_KINGSIDE;
                bitboard[WHITE_ALL] |= WHITE_PHANTOM_KINGSIDE;
            }
            else
            {
                /* Black castling. */
                castle_flags |= BLACK_PHANTOM_KINGS_KINGSIDE | BLACK_HAS_CASTLED;
                bitboard[BLACK_KING] |= BLACK_PHANTOM_KINGSIDE;
                bitboard[BLACK_ALL] |= BLACK_PHANTOM_KINGSIDE;
            }
            fifty_moves++;
            break;
        }
    case Move::Type::QueensideCastle:
        {
            /* We have to move the rook as well. */
            int rook = ROOK + current_player;

            removePiece(move.getSource(), move.getPiece());
            addPiece(move.getDest(), move.getPiece());
            removePiece(move.getDest() - 2, rook);
            addPiece(move.getDest() + 1, rook);

            /* Add phantom kings to detect whether or not
            ** this move is legal.
            */
            if (!current_player)
            {
                /* White castling. */
                castle_flags |= WHITE_PHANTOM_KINGS_QUEENSIDE | WHITE_HAS_CASTLED;
                bitboard[WHITE_KING] |= WHITE_PHANTOM_QUEENSIDE;
                bitboard[WHITE_ALL] |= WHITE_PHANTOM_QUEENSIDE;
            }
            else
            {
                /* Black castling. */
                castle_flags |= BLACK_PHANTOM_KINGS_QUEENSIDE | BLACK_HAS_CASTLED;
                bitboard[BLACK_KING] |= BLACK_PHANTOM_QUEENSIDE;
                bitboard[BLACK_ALL] |= BLACK_PHANTOM_QUEENSIDE;
            }
            fifty_moves++;
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
    if (en_passant)
    {
        int square;
        for (square = 0; square < 64; square++)
        {
            if (en_passant & square_bit[square])
            {
                hash_key ^= ep_hash[square];
                break;
            }
        }
        en_passant = 0LL;
    }

    /* Set en passant possibility in case of a double pawn push. */
    if ((move.getPiece() == WHITE_PAWN) && (move.getDest() -
        move.getSource() == 16))
    {
        en_passant = square_bit[move.getSource() + 8];
        hash_key ^= ep_hash[move.getSource() + 8];
    }
    else if ((move.getPiece() == BLACK_PAWN) && (move.getSource() -
             move.getDest() == 16))
    {
        en_passant = square_bit[move.getDest() + 8];
        hash_key ^= ep_hash[move.getDest() + 8];
    }

    /* Set castling possibilities. If the king moves castling is not allowed. */
    switch (move.getPiece())
    {
    case WHITE_KING:
        if (castle_flags & WHITE_CAN_CASTLE_KINGSIDE)
        {
            hash_key ^= castle_hash[SIDE_WHITE + CASTLE_KINGSIDE];
            castle_flags ^= WHITE_CAN_CASTLE_KINGSIDE;
        }
        if (castle_flags & WHITE_CAN_CASTLE_QUEENSIDE)
        {
            hash_key ^= castle_hash[SIDE_WHITE + CASTLE_QUEENSIDE];
            castle_flags ^= WHITE_CAN_CASTLE_QUEENSIDE;
        }
        break;
    case BLACK_KING:
        if (castle_flags & BLACK_CAN_CASTLE_KINGSIDE)
        {
            hash_key ^= castle_hash[SIDE_BLACK + CASTLE_KINGSIDE];
            castle_flags ^= BLACK_CAN_CASTLE_KINGSIDE;
        }
        if (castle_flags & BLACK_CAN_CASTLE_QUEENSIDE)
        {
            hash_key ^= castle_hash[SIDE_BLACK + CASTLE_QUEENSIDE];
            castle_flags ^= BLACK_CAN_CASTLE_QUEENSIDE;
        }
    }

    /* Any activety in the corners will make castling impossible, either
    ** because the rook moves, or because it is captured.
    */
    switch (move.getSource())
    {
    case SQUARE_A1:
        if (castle_flags & WHITE_CAN_CASTLE_QUEENSIDE)
        {
            hash_key ^= castle_hash[SIDE_WHITE + CASTLE_QUEENSIDE];
            castle_flags ^= WHITE_CAN_CASTLE_QUEENSIDE;
        }
        break;
    case SQUARE_H1:
        if (castle_flags & WHITE_CAN_CASTLE_KINGSIDE)
        {
            hash_key ^= castle_hash[SIDE_WHITE + CASTLE_KINGSIDE];
            castle_flags ^= WHITE_CAN_CASTLE_KINGSIDE;
        }
        break;
    case SQUARE_A8:
        if (castle_flags & BLACK_CAN_CASTLE_QUEENSIDE)
        {
            hash_key ^= castle_hash[SIDE_BLACK + CASTLE_QUEENSIDE];
            castle_flags ^= BLACK_CAN_CASTLE_QUEENSIDE;
        }
        break;
    case SQUARE_H8:
        if (castle_flags & BLACK_CAN_CASTLE_KINGSIDE)
        {
            hash_key ^= castle_hash[SIDE_BLACK + CASTLE_KINGSIDE];
            castle_flags ^= BLACK_CAN_CASTLE_KINGSIDE;
        }
    }
    switch (move.getDest())
    {
    case SQUARE_A1:
        if (castle_flags & WHITE_CAN_CASTLE_QUEENSIDE)
        {
            hash_key ^= castle_hash[SIDE_WHITE + CASTLE_QUEENSIDE];
            castle_flags ^= WHITE_CAN_CASTLE_QUEENSIDE;
        }
        break;
    case SQUARE_H1:
        if (castle_flags & WHITE_CAN_CASTLE_KINGSIDE)
        {
            hash_key ^= castle_hash[SIDE_WHITE + CASTLE_KINGSIDE];
            castle_flags ^= WHITE_CAN_CASTLE_KINGSIDE;
        }
        break;
    case SQUARE_A8:
        if (castle_flags & BLACK_CAN_CASTLE_QUEENSIDE)
        {
            hash_key ^= castle_hash[SIDE_BLACK + CASTLE_QUEENSIDE];
            castle_flags ^= BLACK_CAN_CASTLE_QUEENSIDE;
        }
        break;
    case SQUARE_H8:
        if (castle_flags & BLACK_CAN_CASTLE_KINGSIDE)
        {
            hash_key ^= castle_hash[SIDE_BLACK + CASTLE_KINGSIDE];
            castle_flags ^= BLACK_CAN_CASTLE_KINGSIDE;
        }
    }

    /* Switch players. */
    current_player = OPPONENT(current_player);
    hash_key ^= black_to_move;
}

void Board::unmakeMove(Move move, bitboard_t
                 old_en_passant,                int
                 old_castle_flags, int old_fifty_moves)
{
    int castle_diff;

    /* Switch players. */
    current_player = OPPONENT(current_player);
    hash_key ^= black_to_move;

    switch (move.getType())
    {
    case Move::Type::Normal:
        removePiece(move.getDest(), move.getPiece());
        addPiece(move.getSource(), move.getPiece());
        break;

    case Move::Type::Capture:
        removePiece(move.getDest(), move.getPiece());
        addPiece(move.getDest(), move.getCapturedPiece());
        addPiece(move.getSource(), move.getPiece());
        break;

    case Move::Type::Promotion:
        removePiece(move.getDest(), move.getPiece());
        addPiece(move.getSource(), PAWN + current_player);
        break;

    case Move::Type::PromotionCapture:
        removePiece(move.getDest(), move.getPiece());
        addPiece(move.getDest(), move.getCapturedPiece());
        addPiece(move.getSource(), PAWN + current_player);
        break;

    case Move::Type::EnPassant:
        removePiece(move.getDest(), move.getPiece());
        addPiece(move.getDest() + (move.getPiece() & 1 ? 8 : -8),
                  move.getCapturedPiece());
        addPiece(move.getSource(), move.getPiece());
        break;

    case Move::Type::KingsideCastle:
        {
            /* We have to move the rook as well. */
            int rook = ROOK + current_player;

            /* Remove the phantom kings. */
            if (!current_player)
            {
                bitboard[WHITE_KING] ^= WHITE_PHANTOM_KINGSIDE;
                bitboard[WHITE_ALL] ^= WHITE_PHANTOM_KINGSIDE;

                /* Put back the rook. */
                bitboard[WHITE_ALL] |= bitboard[WHITE_ROOK];
                castle_flags ^= WHITE_HAS_CASTLED
                                       | WHITE_PHANTOM_KINGS_KINGSIDE;
            }
            else
            {
                bitboard[BLACK_KING] ^= BLACK_PHANTOM_KINGSIDE;
                bitboard[BLACK_ALL] ^= BLACK_PHANTOM_KINGSIDE;

                /* Put back the rook. */
                bitboard[BLACK_ALL] |= bitboard[BLACK_ROOK];
                castle_flags ^= BLACK_HAS_CASTLED
                                       | BLACK_PHANTOM_KINGS_KINGSIDE;
            }

            removePiece(move.getDest() - 1, rook);
            addPiece(move.getDest() + 1, rook);
            removePiece(move.getDest(), move.getPiece());
            addPiece(move.getSource(), move.getPiece());

            break;
        }
    case Move::Type::QueensideCastle:
        {
            /* We have to move the rook as well. */
            int rook = ROOK + current_player;

            /* Remove the phantom kings. */
            if (!current_player)
            {
                bitboard[WHITE_KING] ^= WHITE_PHANTOM_QUEENSIDE;
                bitboard[WHITE_ALL] ^= WHITE_PHANTOM_QUEENSIDE;

                /* Put back the rook. */
                bitboard[WHITE_ALL] |= bitboard[WHITE_ROOK];
                castle_flags ^= WHITE_HAS_CASTLED | WHITE_PHANTOM_KINGS_QUEENSIDE;
            }
            else
            {
                bitboard[BLACK_KING] ^= BLACK_PHANTOM_QUEENSIDE;
                bitboard[BLACK_ALL] ^= BLACK_PHANTOM_QUEENSIDE;

                /* Put back the rook. */
                bitboard[BLACK_ALL] |= bitboard[BLACK_ROOK];
                castle_flags ^= BLACK_HAS_CASTLED | BLACK_PHANTOM_KINGS_QUEENSIDE;
            }

            removePiece(move.getDest() + 1, rook);
            addPiece(move.getDest() - 2, rook);
            removePiece(move.getDest(), move.getPiece());
            addPiece(move.getSource(), move.getPiece());

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
    if (en_passant || old_en_passant)
    {
        int square;
        for (square = 0; square < 64; square++)
        {
            if (en_passant & square_bit[square])
                hash_key ^= ep_hash[square];
            if (old_en_passant & square_bit[square])
                hash_key ^= ep_hash[square];
        }
        en_passant = old_en_passant;
    }

    castle_diff = castle_flags ^ old_castle_flags;

    /* Restore phantom kings. */
    switch ((castle_diff & PHANTOM_FLAGS) & old_castle_flags)
    {
    case WHITE_PHANTOM_KINGS_KINGSIDE:
        castle_flags |= WHITE_PHANTOM_KINGS_KINGSIDE;
        bitboard[WHITE_KING] |= WHITE_PHANTOM_KINGSIDE;
        bitboard[WHITE_ALL] |= WHITE_PHANTOM_KINGSIDE;
        break;

    case WHITE_PHANTOM_KINGS_QUEENSIDE:
        castle_flags |= WHITE_PHANTOM_KINGS_QUEENSIDE;
        bitboard[WHITE_KING] |= WHITE_PHANTOM_QUEENSIDE;
        bitboard[WHITE_ALL] |= WHITE_PHANTOM_QUEENSIDE;
        break;

    case BLACK_PHANTOM_KINGS_KINGSIDE:
        castle_flags |= BLACK_PHANTOM_KINGS_KINGSIDE;
        bitboard[BLACK_KING] |= BLACK_PHANTOM_KINGSIDE;
        bitboard[BLACK_ALL] |= BLACK_PHANTOM_KINGSIDE;
        break;

    case BLACK_PHANTOM_KINGS_QUEENSIDE:
        castle_flags |= BLACK_PHANTOM_KINGS_QUEENSIDE;
        bitboard[BLACK_KING] |= BLACK_PHANTOM_QUEENSIDE;
        bitboard[BLACK_ALL] |= BLACK_PHANTOM_QUEENSIDE;
        break;
    }

    /* Restore castle flags. */
    if (castle_diff & 15)
    {
        int i;
        for (i = 0; i < 4; i++)
            if (castle_diff & (1 << i))
                hash_key ^= castle_hash[i];
        castle_flags = old_castle_flags;
    }
    fifty_moves = old_fifty_moves;
}
