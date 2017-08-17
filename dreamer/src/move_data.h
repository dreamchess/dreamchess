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

#ifndef MOVE_DATA_H
#define MOVE_DATA_H

int **all_knight_moves(void);
int **all_king_moves(void);
int ***all_bishop_moves(void);
int ***all_rook_moves(void);
int ***all_queen_moves(void);
int **all_white_pawn_capture_moves(void);
int **all_black_pawn_capture_moves(void);

#endif /* MOVE_DATA_H */
