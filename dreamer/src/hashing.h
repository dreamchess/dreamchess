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

#ifndef DREAMER_HASHING_H
#define DREAMER_HASHING_H

#include "board.h"

extern unsigned long long random_seed_64;
extern unsigned long long pieces_hash[12][64];
extern unsigned long long castle_hash[4];
extern unsigned long long ep_hash[64];
extern unsigned long long black_to_move;

void random_init_64(unsigned long long seed);

unsigned long long random_rand_64(void);

void init_hash(void);

unsigned long long hash_key(const Board &board);

#endif
