/*  DreamChess
**  Copyright (C) 2005  The DreamChess project
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

%option noyywrap

%{
#include "pgn_parser.h"

static char *make_string();

void pgn_parse_file(char *filename)
{
    yyin = fopen(filename, "r");

    if (!yyin)
    {
        printf("Couldn't open file\n");
        return;
    }

    YY_FLUSH_BUFFER;

    if (yyparse())
        printf("Parse error\n");
    else
        printf("File ok!\n");
}
%}

%%

\"([^\t\n\r\f\v\\\"]|(\\[\\\"]))*\" {
        yylval.yycharp = make_string();
        return STRING;
    }

\*|1-0|0-1|1\/2-1\/2 {
        return GAMETERM;
    }

[0-9]+ {
        yylval.yyint = atoi(yytext);
        return INTEGER;
    }

$[[:digit:]]+ {
        yylval.yyint = atoi(yytext + 1);
        return NAG;
    }

[[:alnum:]][[:alnum:]_\+#=:-]* {
        yylval.yycharp = strdup(yytext);
        return SYMBOL;
    }

[ \t\n\r]
        /* Skip whitespace. */

\{[^\{]*\} printf("Skipping: %s\n", yytext);
        /* Skip multiline comments. */

;.*\n
        /* Skip end-of-line comments. */

.       return yytext[0];

%%

static char *make_string()
{
    int i;
    int len = 0;
    char *string;

    /* We need at least yyleng bytes, plus 1 for the '\0' and minus 2 for the
     * opening and closing quotes that will be removed.
     */
    string = malloc(yyleng - 1);

    /* Skip quotes. */
    for (i = 1; i < yyleng - 1; i++)
    {
        /* Remove escapes. */
        if (yytext[i] == '\\')
            i++;

        string[len++] = yytext[i];
    }

    string[len] = '\0';
    return string;
}
