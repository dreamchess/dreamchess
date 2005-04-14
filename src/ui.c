/*  DreamChess
**  Copyright (C) 2003-2004  The DreamChess project
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <string.h>

#include "ui.h"

ui_driver_t *ui_driver[] =
    {
#ifdef WITH_UI_SDLGL
        &ui_sdlgl,
#endif /* WITH_UI_SDLGL */
#ifdef WITH_UI_SDL
        &ui_sdl,
#endif /* WITH_UI_SDL */
        NULL
    };

ui_driver_t *ui_find_driver(char *name)
{
    int i = 0;

    while (ui_driver[i])
    {
        if (!strcmp(ui_driver[i]->name, name))
            return ui_driver[i];
        i++;
    }

    return NULL;
}

void ui_list_drivers()
{
    int i = 0;

    printf("User interface: ");
    while (ui_driver[i])
    {
        if (i > 0)
            printf(", ");
        printf(ui_driver[i]->name);
        i++;
    }
    printf("\n");
}
