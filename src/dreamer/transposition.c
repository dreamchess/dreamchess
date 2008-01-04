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

#include <stdlib.h>
#include <stdio.h>

#include "board.h"
#include "hashing.h"
#include "transposition.h"
#include "search.h"
#include "move.h"

/* #define DEBUG */

#define ENTRIES (1 << power_of_two)
int power_of_two;

#ifdef DEBUG
int queries;
int hits;
#endif

int collisions;

typedef struct entry
{
    int eval_type;
    int eval;
    int depth;
    unsigned long long hash_key;
    int time_stamp;
    move_t move;
}
entry_t;

entry_t *table;

void
store_board(board_t *board, int eval, int eval_type, int depth, int ply,
            int time_stamp, move_t move)
{
    int index = board->hash_key & (ENTRIES - 1);

    if ((table[index].eval_type != EVAL_NONE) && (table[index].hash_key == board->hash_key)
            && (table[index].depth > depth))
        /* Do not overwrite entries for this board at greater depth. */
        return;

    /* Make mate-in-n values relative to board that's to be stored */
    if (eval < ALPHABETA_MIN + 1000)
        eval -= ply;
    else if (eval > ALPHABETA_MAX - 1000)
        eval += ply;

    table[index].hash_key = board->hash_key;
    table[index].eval = eval;
    table[index].eval_type = eval_type;
    table[index].depth = depth;
    table[index].time_stamp = time_stamp;
    table[index].move = move;
}

void
set_best_move(board_t *board, move_t move)
{
    int index = board->hash_key & (ENTRIES - 1);

    if ((table[index].eval_type == EVAL_NONE) || (table[index].hash_key != board->hash_key))
        store_board(board, 0, EVAL_PV, 0, 0, 0, move);
    else
        table[index].move = move;
}

int
lookup_board(board_t *board, int depth, int ply, int *eval)
{
    int index = board->hash_key & (ENTRIES - 1);

#ifdef DEBUG
    if (queries == 100000) {e_comm_send("TT hit rate:: %.2f%%\n", hits / (float) queries * 100); queries = 0; hits = 0;}
    queries++;
#endif
    if (table[index].eval_type == EVAL_NONE)
        return EVAL_NONE;

    if (table[index].hash_key != board->hash_key)
        return EVAL_NONE;
#ifdef DEBUG
    hits++;
#endif

    if (table[index].depth < depth || table[index].eval_type == EVAL_PV)
        return EVAL_NONE;

    *eval = table[index].eval;

    /* Make mate-in-n values relative to current game position */
    if (*eval < ALPHABETA_MIN + 1000)
        *eval += ply;
    else if (*eval > ALPHABETA_MAX - 1000)
        *eval -= ply;

    return table[index].eval_type;
}

move_t
lookup_best_move(board_t *board)
{
    int index = board->hash_key & (ENTRIES - 1);

    if ((table[index].eval_type == EVAL_NONE) || (table[index].hash_key != board->hash_key))
        return NO_MOVE;

    return table[index].move;
}

void
clear_table()
{
    int i;
    for (i = 0; i < ENTRIES; i++)
    {
        table[i].eval_type = EVAL_NONE;
    }
}

void transposition_init(int megabytes)
{
    int i = 0;
    int x = 2;

    int max_entries = megabytes * 1024768 / sizeof(entry_t);

    while (x <= max_entries) {
        x *= 2;
        i++;
    }

    x /= 2;
    power_of_two = i;

    printf("Hash table size: %i MB\n", x * sizeof(entry_t) / 1024768);
    table = malloc(x * sizeof(entry_t));

    if (!table)
    {
         fprintf(stderr, "Failed to allocate memory for hash table\n");
         exit(1);
    }
}

void transposition_exit()
{
    free(table);
}
