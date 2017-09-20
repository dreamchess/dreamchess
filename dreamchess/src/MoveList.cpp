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

#include <string.h>
#include <cstdlib>

#include "MoveList.h"

void MoveList::play(char *m) {
    if (entries == max_entries) {
        max_entries *= 2;
        move = (char **)realloc(move, max_entries * sizeof(char *));
    }
    move[entries++] = strdup(m);
    view = entries - 1;
}

void MoveList::undo() {
    if (entries > 0) {
        entries--;
        free(move[entries]);
        view = entries - 1;
    }
}

MoveList::MoveList() {
    max_entries = 20;
    move = (char **)malloc(max_entries * sizeof(char *));
    entries = 0;
    view = -1;
}

MoveList::~MoveList() {
    while (entries > 0)
        undo();
    free(move);
}

void MoveList::viewNext() {
    if (view < entries - 1)
        view++;
}

void MoveList::viewPrev() {
    if (view >= 0)
        view--;
}
