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

%union {
    int yyint;
    char *yycharp;
}

%token STRING
%token INTEGER
%token NAG
%token SYMBOL
%token GAMETERM

%{
#include "dreamchess.h"

extern char *yystring;

int yylex(void);
int yyerror(const char *s);
%}

%%

input                    : pgn_game
;

pgn_game                 : tag_section movetext_section
;

tag_section              : tag_pair tag_section
                         | empty
;

tag_pair                 : '[' tag_name tag_value ']'
;

tag_name                 : SYMBOL {/*printf("Tag name: %s\n", yylval.yycharp);*/}
;

tag_value                : STRING {/*printf("Tag value: %s\n", yylval.yycharp);*/}
;

movetext_section         : element_sequence game_termination
;

element_sequence         : element element_sequence
                         | recursive_variation element_sequence
                         | empty
;

element                  : move_number_indication
                         | san_move
                         | numeric_annotation_glyph
;

recursive_variation      : '(' element_sequence ')'
;

game_termination         : GAMETERM
;

san_move                 : SYMBOL {game_make_move_str(yylval.yycharp, 0);/*printf("Found move: %s\n", yylval.yycharp);*/}
;

move_number_indication   : INTEGER periods
;

periods                  : '.' periods
                         | empty
;

numeric_annotation_glyph : NAG
;

empty                    :
;

%%

int yyerror(const char *s)
{
    printf("Error: %s\n", s);
    return 1;
}
