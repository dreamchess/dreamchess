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
#include <stdlib.h>
#include <string.h>

#include "makebook.h"
#include "board.h"
#include "move.h"
#include "commands.h"
#include "pgn_scanner.h"

typedef struct
{
    Move move;
    int count;
} makebook_move_data;

typedef struct
{
    unsigned long long hash;
    makebook_move_data *move;
    int moves;
} makebook_entry;

static makebook_entry *table;
static int table_entries;
static int table_size;

static Board board;

static int moves_done;

static unsigned short move_to_short(Move move)
{
    unsigned short m;

    m = move.getDest() | (move.getSource() << 6);

    if (move.doesPromotion()) {
        switch(move.getPieceKind()) {
        case KNIGHT:
            m |= MAKEBOOK_KNIGHT << 12;
            break;
        case BISHOP:
            m |= MAKEBOOK_BISHOP << 12;
            break;
        case ROOK:
            m |= MAKEBOOK_ROOK << 12;
            break;
        case QUEEN:
            m |= MAKEBOOK_QUEEN << 12;    
        }
    }

    return m;
}

static int makebook_find(long long hash)
{
    int low = 0;
    int high = table_entries;

    while (low < high) {
        int mid = (low + high) / 2;
        if (table[mid].hash < hash)
            low = mid + 1;
        else
            high = mid;
    }

    return low;
}

static void makebook_add_move(makebook_entry *entry, Move move)
{
    int i;

    for (i = 0; i < entry->moves; i++)
        if (entry->move[i].move == move)
        {
            entry->move[i].count++;
            return;
        }

    entry->move = (makebook_move_data *)realloc(entry->move, sizeof(makebook_move_data) * (entry->moves + 1));
    entry->move[entry->moves].move = move;
    entry->move[entry->moves++].count = 1;
}

static void makebook_add(unsigned long long hash, Move move)
{
    int index = makebook_find(hash);

    if (table[index].hash != hash)
    {
        if (table_entries == table_size)
        {
            table_size *= 2;
            table = (makebook_entry *)realloc(table, sizeof(makebook_entry) * table_size);

            if (!table)
            {
                fprintf(stderr, "Out of memory\n");
                exit(1);
            }
        }

        if (index < table_entries)
            memmove(&table[index + 1], &table[index], (table_entries - index) * sizeof(makebook_entry));

        table[index].hash = hash;
        table[index].move = NULL;
        table[index].moves = 0;
        table_entries++;
    }

    makebook_add_move(&table[index], move);
   
}

void makebook_reset(void)
{
    board.setup();
    moves_done = 0;
}

void makebook_move(char *str)
{
    Move move;

    if (!parse_move(board, 0, str, &move))
    {
        if (moves_done++ < 20)
        {
            makebook_add(board.hash_key, move);
            board.makeMove(move);
        }
    }
    else
    {
        fprintf(stderr, "move %s is invalid\n", str);
        exit(1);
    }
}

static int compare(const void *p1, const void *p2)
{
    if (((makebook_move_data *) p1)->count > ((makebook_move_data *) p2)->count)
        return -1;

    if (((makebook_move_data *) p1)->count < ((makebook_move_data *) p2)->count)
        return 1;

    return 0;
}

static void makebook_normalize_moves(makebook_entry *entry)
{
    int i;
    int max = 0;

    for (i = 0; i < entry->moves; i++)
        if (entry->move[i].count > max)
            max = entry->move[i].count;

    for (i = 0; i < entry->moves; i++)
        entry->move[i].count = entry->move[i].count * 255 / max;

    qsort(entry->move, entry->moves, sizeof(makebook_move_data), compare);

    for (i = 0; i < entry->moves; i++)
        if (entry->move[i].count == 0)
            break;

    entry->moves = i;
}

static void makebook_cleanup_moves(void)
{
    int i;

    for (i = 0; i < table_entries; i++)
    {
        makebook_normalize_moves(&table[i]);
    }
}

#if 0
static void makebook_print(long long hash)
{
    int index = makebook_find(hash);
    int i;

    if (table[index].hash != hash)
        return;

    for (i = 0; i < table[index].moves; i++)
    {
        char *s = coord_move_str(table[index].move[i].move);

        fprintf(stderr, "%s (%i)\n", s, table[index].move[i].count);
    }
}
#endif

static void write_uint8(FILE *f, unsigned char c)
{
    if (fwrite(&c, 1, 1, f) < 1)
    {
        fprintf(stderr, "Error writing to opening book\n");
        fclose(f);
        exit(1);
    }
}

static void write_uint16(FILE *f, unsigned short i)
{
    write_uint8(f, i >> 8);
    write_uint8(f, i);
}

static void write_uint32(FILE *f, unsigned int i)
{
    write_uint16(f, i >> 16);
    write_uint16(f, i);
}

static void write_uint64(FILE *f, unsigned long long ll)
{
    write_uint32(f, ll >> 32);
    write_uint32(f, ll);
}

static void makebook_write(char *file)
{
    FILE *f;
    int i;
    int offset;

    f = fopen(file, "wb+");

    if (!f)
    {
        fprintf(stderr, "Failed to open %s for writing\n", file);
        exit(1);
    }

    fprintf(f, "DCB 0000");

    /* Write number of entries */
    write_uint32(f, table_entries);

    offset = 12 + table_entries * 12;
    for (i = 0; i < table_entries; i++)
    {
        write_uint64(f, table[i].hash);
        write_uint32(f, offset);
        offset += table[i].moves * 3;
    }

    for (i = 0; i < table_entries; i++)
    {
        int j;
        for (j = 0; j < table[i].moves; j++)
        {
            unsigned short move = move_to_short(table[i].move[j].move);

            if (j == table[i].moves - 1)
                move |= MAKEBOOK_LAST;
            write_uint16(f, move);
            write_uint8(f, table[i].move[j].count);
        }
    }

    fclose(f);
}

void makebook(char *pgnfile, char *binfile)
{
    makebook_reset();
    table_size = 256;
    table_entries = 0;
    table = (makebook_entry *)malloc(sizeof(makebook_entry) * table_size);

    pgn_parse_file(pgnfile);
    makebook_cleanup_moves();
    makebook_write(binfile);
    return;
}
