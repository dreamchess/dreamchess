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

#include <stdlib.h>

#include "credits.h"
#include "config.h"

static const char* programming[] = { "Programming",
                                     "Kris McAulay",
                                     "Walter van Niftrik",
                                     "Lawrence Sebald",
                                     NULL };

static const char* graphics[] = { "Graphics",
                                  "Drew 'Hawke' Ball",
                                  "Annie Kirkpatrick",
                                  "Brandon May (melancholy)",
                                  "Kris McAulay",
                                  "Rogier van Schaijk",
                                  NULL };

static const char* modelling[] = { "3D Modelling",
                                   "Walter van Niftrik",
                                   "Rogier van Schaijk",
                                   NULL };

static const char* thanks[] = { "Special Thanks To",
                                "Fran\xe7ois-Dominic Laram\xe9""e",
                                "Armando Hern\xe1ndez Marroqu\xedn",
                                "Chris Noll (Christuserloeser)",
                                NULL };

static const char* copyright[] = { "DreamChess",
                                   "\xa9 2003-" COPYRIGHT_YEAR,
                                   "https://www.dreamchess.org",
                                   NULL };

static const char* music[] = { "Music",
                               "Matthew P. Smith",
                               NULL };

static const char **credits[] = { copyright, programming, music, graphics, modelling, thanks, NULL };

const char ***get_credits(void)
{
    return credits;
}
