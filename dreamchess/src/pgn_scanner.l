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

%option noyywrap
%option nounput
%option nounistd
%option never-interactive

%{
#include "pgn_scanner.h"
#include "pgn_parser.h"

static char *make_string(void);
int yyparse(void);

int pgn_parse_file(char *filename)
{
    int retval = 0;

    yyin = fopen(filename, "r");

    if (!yyin)
    {
        printf("Couldn't open file\n");
        return 1;
    }

    YY_NEW_FILE;

    if (yyparse())
    {
        retval = 1;
        printf("Parse error\n");
    }

    fclose(yyin);

    return retval;
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

static char *make_string(void)
{
    int i;
    int len = 0;
    char *string;

    /* We need at least yyleng bytes, plus 1 for the '\0' and minus 2 for the
     * opening and closing quotes that will be removed.
     */
    string = (char *)malloc(yyleng - 1);

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
