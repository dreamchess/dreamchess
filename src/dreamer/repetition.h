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

#ifndef REPETITION_H
#define REPETITION_H

#include "board.h"

extern long long repetition_list[99];
extern int repetition_head;

int is_repetition(board_t *board, int ply);

int is_draw(board_t *board);

void repetition_init();

void repetition_exit();

void repetition_add(board_t *board, move_t *move);

void repetition_remove();

#endif /* REPETITION_H */
