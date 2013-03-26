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

#ifndef SEARCH_H
#define SEARCH_H

#include "dreamer.h"

#define ALPHABETA_MIN -30000
#define ALPHABETA_MAX 30000
#define ALPHABETA_ILLEGAL -31000

#define ALPHABETA_CHECKMATE -29000

#define MAX_NODE 0
#define MIN_NODE 1

move_t
find_best_move(state_t *state);

void
pv_clear(void);

move_t
ponder(state_t *state);

#endif /* SEARCH_H */
