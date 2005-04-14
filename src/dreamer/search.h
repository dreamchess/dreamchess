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

#ifndef SEARCH_H
#define SEARCH_H

#include "board.h"

#define ALPHABETA_MIN -30000
#define ALPHABETA_MAX 30000
#define ALPHABETA_ILLEGAL -31000

#define ALPHABETA_CHECKMATE -29000

#define MAX_NODE 0
#define MIN_NODE 1

move_t
find_best_move(board_t *board, int depth);

#endif /* SEARCH_H */
