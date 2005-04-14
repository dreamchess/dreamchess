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

#ifndef MOVE_H
#define MOVE_H

#include "board.h"

#define NORMAL_MOVE 0
#define CAPTURE_MOVE 1
#define CAPTURE_MOVE_EN_PASSENT 2
#define CASTLING_MOVE_KINGSIDE 4
#define CASTLING_MOVE_QUEENSIDE 8
#define RESIGN_MOVE 16
#define STALEMATE_MOVE 32
#define PROMOTION_MOVE_KNIGHT 64
#define PROMOTION_MOVE_BISHOP 128
#define PROMOTION_MOVE_ROOK 256
#define PROMOTION_MOVE_QUEEN 512
#define THREEFOLD_REPETITION_MOVE 1024
#define FIFTY_MOVES_MOVE 2048

#define MOVE_NO_PROMOTION_MASK 63
#define MOVE_PROMOTION_MASK 960

void
move_init();

void
move_exit();

int
compute_legal_moves(board_t *board, move_t *move);

#endif /* MOVE_H */
