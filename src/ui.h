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

#ifndef _UI_H
#define _UI_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "board.h"
#include "dreamchess.h"

typedef struct ui_driver
{
    char *name;
    int (* init) (void);
    int (* exit) (void);
    config_t *( *config) (void);
    void (* update) (board_t *board, move_t *move);
    void (* poll) (void);
    void (* show_message) (char *msg);
    void (* show_result) (result_t *result);
}
ui_driver_t;

#ifdef WITH_UI_SDLGL
extern ui_driver_t ui_sdlgl;
#endif /* WITH_UI_SDLGL */

#ifdef WITH_UI_SDL
extern ui_driver_t ui_sdl;
#endif /* WITH_UI_SDL */

extern ui_driver_t *ui_driver[];

ui_driver_t *ui_find_driver(char *name);
void ui_list_drivers();

#endif /* _UI_H */
