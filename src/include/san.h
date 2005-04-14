/*  DreamChess
**  Copyright (C) 2004  The DreamChess project
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

#ifndef _SAN_H
#define _SAN_H

#define SAN_PAWN 0
#define SAN_KNIGHT 1
#define SAN_BISHOP 2
#define SAN_ROOK 3
#define SAN_QUEEN 4
#define SAN_KING 5

#define SAN_NORMAL 0
#define SAN_CAPTURE 1
#define SAN_QUEENSIDE_CASTLE 2
#define SAN_KINGSIDE_CASTLE 3

#define SAN_STATE_NORMAL 0
#define SAN_STATE_CHECK 1
#define SAN_STATE_CHECKMATE 2

#define SAN_NOT_SPECIFIED -1

typedef struct san_move
{
    int type;
    int state;
    int piece;
    int source_file, source_rank;
    int destination;
    int promotion_piece;
} san_move_t;

san_move_t *san_parse(char *s);
char *san_string(san_move_t *move);

#endif /* _SAN_H */
