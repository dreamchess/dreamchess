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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

static char* programming[] = { "Programming",
                               "Kris McAulay",
                               "Walter van Niftrik",
                               "Lawrence Sebald",
                               NULL };

static char* graphics[] = { "Graphics",
                            "Drew 'Hawke' Ball",
                            "Brandon May (melancholy)",
                            "Kris McAulay",
                            "Rogier van Schaijk",
                            "Laranesight",
                            NULL };

static char* modelling[] = { "3D Modelling",
                            "Walter van Niftrik",
                            "Rogier van Schaijk",
                            NULL };

static char* thanks[] = { "Special Thanks To",
                          "François-Dominic Laramée",
                          "Armando Hernández Marroquín",
                          "Chris Noll (Christuserloeser)",
                          NULL };

static char* copyright[] = { "DreamChess",
                             "© 2003-2007 The DreamChess project",
                             "http://www.dreamchess.org",
                             NULL };

static char* music[] = { "Music",
                         "Matthew P. Smith",
                         NULL };

static char **credits[] = { copyright, programming, music, graphics, modelling, thanks, NULL };

char ***get_credits()
{
    return credits;
}
