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

%option noyywrap
%option nounput
%option nounistd
%option never-interactive

%{
#include "pgn_scanner.h"
#include "pgn_parser.h"

static char *make_string(void);
extern int pgn_parse(void);

int pgn_parse_file(char *filename)
{
	int retval = 0;

	pgn_in = fopen(filename, "r");

	if (!pgn_in)
	{
		printf("Couldn't open file\n");
		return 1;
	}

	YY_NEW_FILE;

	if (pgn_parse())
	{
		retval = 1;
		printf("Parse error\n");
	}

	fclose(pgn_in);

	return retval;
}
%}

%%

\"([^\t\n\r\f\v\\\"]|(\\[\\\"]))*\" {
		pgn_lval.yycharp = make_string();
		return STRING;
	}

\*|1-0|0-1|1\/2-1\/2 {
		return GAMETERM;
	}

[0-9]+ {
		pgn_lval.yyint = atoi(pgn_text);
		return INTEGER;
	}

$[[:digit:]]+ {
		pgn_lval.yyint = atoi(pgn_text + 1);
		return NAG;
	}

[[:alnum:]][[:alnum:]_\+#=:-]* {
		pgn_lval.yycharp = strdup(pgn_text);
		return SYMBOL;
	}

[ \t\n\r]
		/* Skip whitespace. */

\{[^\{]*\} printf("Skipping: %s\n", pgn_text);
		/* Skip multiline comments. */

;.*\n
		/* Skip end-of-line comments. */

.       return pgn_text[0];

%%

static char *make_string(void)
{
	int i;
	int len = 0;
	char *string;

	/* We need at least yyleng bytes, plus 1 for the '\0' and minus 2 for the
	 * opening and closing quotes that will be removed.
	 */
	string = malloc(pgn_leng - 1);

	/* Skip quotes. */
	for (i = 1; i < pgn_leng - 1; i++)
	{
		/* Remove escapes. */
		if (pgn_text[i] == '\\')
			i++;

		string[len++] = pgn_text[i];
	}

	string[len] = '\0';
	return string;
}
