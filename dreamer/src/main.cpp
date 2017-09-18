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

#include <stdio.h>

#include "board.h"
#include "hashing.h"
#include "move.h"
#include "ttable.h"
#include "git_rev.h"

int engine(void *data);

Dreamer *g_dreamer;

int main(int argc, char **argv)
{
    fprintf(stderr, "Dreamer %s\n", g_version);

    board_init();
    init_hash();
    g_moveGenerator = new MoveGenerator();
    g_transTable = new TTable(64);

    /* return makebook("/home/walter/tmp/GM2001.pgn", "/home/walter/tmp/opening.dcb"); */

    g_dreamer = new Dreamer;
    g_dreamer->run();

    delete g_dreamer;
    delete g_transTable;
    delete g_moveGenerator;
}
