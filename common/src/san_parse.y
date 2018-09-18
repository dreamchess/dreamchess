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

%token CFILE
%token CRANK

%{
#include <stdlib.h>
#include <string.h>

#include "san.h"

int yylex(void);
int yyerror(const char *s);

san_move_t san_move;
%}

%%

input   : san
;

san     : mark move
;

move    /* Queenside castle. */
        : 'O' '-' 'O' '-' 'O'     { san_move.type = SAN_QUEENSIDE_CASTLE; }
        /* Kingside castle. */
        | 'O' '-' 'O'             { san_move.type = SAN_KINGSIDE_CASTLE; }
        /* Non-pawn capture without rank. */
        | dest capt op_file piece
        /* Non-pawn move without rank. */
        | dest op_file piece
        /* Non-pawn move or capture with rank. */
        | dest op_capt rank op_file piece
        /* Pawn move with promotion and capture. */
        | promo '=' dest capt file
        /* Pawn move with promotion. */
        | promo '=' dest
        /* Pawn move with capture. */
        | dest capt file
        /* Pawn move without capture or promotion. */
        | dest
;

piece   : 'Q'                     { san_move.piece = SAN_QUEEN; }
        | 'R'                     { san_move.piece = SAN_ROOK; }
        | 'B'                     { san_move.piece = SAN_BISHOP; }
        | 'N'                     { san_move.piece = SAN_KNIGHT; }
        | 'K'                     { san_move.piece = SAN_KING; }
;

op_capt :
        | capt
;

op_file :
        | file
;

file    : CFILE                   { san_move.source_file = $1; }
;

rank    : CRANK                   { san_move.source_rank = $1; }
;

capt    : 'x'                     { san_move.type = SAN_CAPTURE; }
;

dest    : CRANK CFILE             { san_move.destination = 8 * $1 + $2; }
;

mark    :
        | '+'                     { san_move.state = SAN_STATE_CHECK; }
        | '#'                     { san_move.state = SAN_STATE_CHECKMATE; }
;

promo   : 'Q'                     { san_move.promotion_piece = SAN_QUEEN; }
        | 'R'                     { san_move.promotion_piece = SAN_ROOK; }
        | 'B'                     { san_move.promotion_piece = SAN_BISHOP; }
        | 'N'                     { san_move.promotion_piece = SAN_KNIGHT; }
;
%%

#include <stdio.h>
#include <ctype.h>

#include "san.h"

char* move;
int ptr;

san_move_t *san_parse(char *s)
{
    move = s;
    ptr = strlen(move) - 1;

    san_move.type = SAN_NORMAL;
    san_move.state = SAN_STATE_NORMAL;
    san_move.piece = SAN_PAWN;
    san_move.destination = SAN_NOT_SPECIFIED;
    san_move.source_rank = SAN_NOT_SPECIFIED;
    san_move.source_file = SAN_NOT_SPECIFIED;
    san_move.promotion_piece = SAN_NOT_SPECIFIED;

    if (!yyparse())
    {
        san_move_t *retval = malloc(sizeof(san_move_t));
        *retval = san_move;
        return retval;
    }

    return NULL;
}

static void add_piece(char *s, int piece)
{
    switch(piece)
    {
    case SAN_QUEEN:
        *s = 'Q';
        break;
    case SAN_ROOK:
        *s = 'R';
        break;
    case SAN_BISHOP:
        *s = 'B';
        break;
    case SAN_KNIGHT:
        *s = 'N';
        break;
    case SAN_KING:
        *s = 'K';
    }
}

char *san_string(san_move_t *move)
{
    char *s = malloc(8);
    int i = 0;

    switch (move->type)
    {
    case SAN_NORMAL:
    case SAN_CAPTURE:
        {
            if ((move->piece != SAN_NOT_SPECIFIED) && (move->piece != SAN_PAWN))
                add_piece(s + i++, move->piece);

            if (move->source_file != SAN_NOT_SPECIFIED)
                s[i++] = move->source_file + 'a';

            if (move->source_rank != SAN_NOT_SPECIFIED)
                s[i++] = move->source_rank + '1';

            if (move->type == SAN_CAPTURE)
                s[i++] = 'x';

            s[i++] = move->destination % 8 + 'a';
            s[i++] = move->destination / 8 + '1';

            if (move->promotion_piece != SAN_NOT_SPECIFIED)
            {
                s[i++] = '=';
                add_piece(s + i++, move->promotion_piece);
            }
        }
        break;
    case SAN_QUEENSIDE_CASTLE:
        strcpy(s, "O-O-O");
        i = 5;
        break;
    case SAN_KINGSIDE_CASTLE:
        strcpy(s, "O-O");
        i = 3;
    }

    if (move->state == SAN_STATE_CHECK)
        s[i++] = '+';
    else if (move->state == SAN_STATE_CHECKMATE)
        s[i++] = '#';

    s[i] = '\0';

    return s;
}

int yyerror(const char *s)
{
    return 1;
}

int yylex(void)
{
    int c;

    if (ptr >= 0)
    {
        c = move[ptr--];

        if ((c >= '1') && (c <= '8'))
        {
            yylval = c - '1';
            return CRANK;
        }

        if ((c >= 'a') && (c <= 'h'))
        {
            yylval = c - 'a';
            return CFILE;
        }

        return c;
    }

    return 0;
}
