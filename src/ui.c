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

void ui_list_drivers(void)
{
    int i = 0;

    printf("User interface: ");
    while (ui_driver[i])
    {
        if (i > 0)
            printf(", ");
        printf("%s", ui_driver[i]->name);
        i++;
    }
    printf("\n");
}
