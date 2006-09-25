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

#include "board.h"
#include "hashing.h"

long long random_seed_64;
long long pieces_hash[12][64];
long long castle_hash[4];
long long ep_hash[64];
long long black_to_move;

void
random_init_64(long long seed)
{
    random_seed_64 = seed;
}

long long
random_rand_64()
{
    const long long mul_64 = 8LL * 893636665409536LL - 3LL;
    const long long add_64 = 4354685564936845319LL;

    random_seed_64 = random_seed_64 * mul_64 + add_64;
    return random_seed_64;
}

void
init_hash()
{
    int i,j;
    for (i = 0; i < 12; i++)
        for (j = 0; j < 64; j++)
            pieces_hash[i][j] = random_rand_64();
    for (i = 0; i < 4; i++)
        castle_hash[i] = random_rand_64();
    for (i = 0; i < 64; i++)
        ep_hash[i] = random_rand_64();
    black_to_move = random_rand_64();
}

long long
hash_key(board_t *board)
{
    int piece;
    int square;
    int i;
    long long hash = 0;
    bitboard_t bitboard;
    for (piece = 0; piece < ALL; piece++)
    {
        bitboard = board->bitboard[piece];
        if ((piece == WHITE_KING) && (board->castle_flags &
                                      WHITE_PHANTOM_KINGS_KINGSIDE))
            bitboard ^= WHITE_PHANTOM_KINGSIDE;
        if ((piece == WHITE_KING) && (board->castle_flags &
                                      WHITE_PHANTOM_KINGS_QUEENSIDE))
            bitboard ^= WHITE_PHANTOM_QUEENSIDE;
        if ((piece == BLACK_KING) && (board->castle_flags &
                                      BLACK_PHANTOM_KINGS_KINGSIDE))
            bitboard ^= BLACK_PHANTOM_KINGSIDE;
        if ((piece == BLACK_KING) && (board->castle_flags &
                                      BLACK_PHANTOM_KINGS_QUEENSIDE))
            bitboard ^= BLACK_PHANTOM_QUEENSIDE;
        if (bitboard)
            for (square = 0; square < 64; square++)
                if (bitboard & square_bit[square])
                    hash ^= pieces_hash[piece][square];
    }
    for (i = 0; i < 4; i++)
        if (board->castle_flags & (1 << i))
            hash ^= castle_hash[i];

    bitboard = board->en_passant;
    if (bitboard)
        for (square = 0; square < 64; square++)
            if (bitboard & square_bit[square])
                hash ^= ep_hash[square];

    if (board->current_player)
        hash ^= black_to_move;

    return hash;
}
