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

#ifndef MAKEBOOK_H
#define MAKEBOOK_H

#define MAKEBOOK_KNIGHT 0
#define MAKEBOOK_BISHOP 1
#define MAKEBOOK_ROOK 2
#define MAKEBOOK_QUEEN 3

#define MAKEBOOK_LAST (1 << 15)

void makebook(char *pgnfile, char *binfile);
void makebook_reset(void);
void makebook_move(char *str);

#endif
