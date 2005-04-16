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

typedef enum ui_event
{
    UI_EVENT_NONE,
    UI_EVENT_ACTION,
    UI_EVENT_ESCAPE,
    UI_EVENT_EXTRA1,
    UI_EVENT_EXTRA2,
    UI_EVENT_EXTRA3,
    UI_EVENT_UP,
    UI_EVENT_RIGHT,
    UI_EVENT_DOWN,
    UI_EVENT_LEFT,
    UI_EVENT_CHAR_a,
    UI_EVENT_CHAR_b,
    UI_EVENT_CHAR_c,
    UI_EVENT_CHAR_d,
    UI_EVENT_CHAR_e,
    UI_EVENT_CHAR_f,
    UI_EVENT_CHAR_g,
    UI_EVENT_CHAR_h,
    UI_EVENT_CHAR_i,
    UI_EVENT_CHAR_j,
    UI_EVENT_CHAR_k,
    UI_EVENT_CHAR_l,
    UI_EVENT_CHAR_m,
    UI_EVENT_CHAR_n,
    UI_EVENT_CHAR_o,
    UI_EVENT_CHAR_p,
    UI_EVENT_CHAR_q,
    UI_EVENT_CHAR_r,
    UI_EVENT_CHAR_s,
    UI_EVENT_CHAR_t,
    UI_EVENT_CHAR_u,
    UI_EVENT_CHAR_v,
    UI_EVENT_CHAR_w,
    UI_EVENT_CHAR_x,
    UI_EVENT_CHAR_y,
    UI_EVENT_CHAR_z,
    UI_EVENT_CHAR_A,
    UI_EVENT_CHAR_B,
    UI_EVENT_CHAR_C,
    UI_EVENT_CHAR_D,
    UI_EVENT_CHAR_E,
    UI_EVENT_CHAR_F,
    UI_EVENT_CHAR_G,
    UI_EVENT_CHAR_H,
    UI_EVENT_CHAR_I,
    UI_EVENT_CHAR_J,
    UI_EVENT_CHAR_K,
    UI_EVENT_CHAR_L,
    UI_EVENT_CHAR_M,
    UI_EVENT_CHAR_N,
    UI_EVENT_CHAR_O,
    UI_EVENT_CHAR_P,
    UI_EVENT_CHAR_Q,
    UI_EVENT_CHAR_R,
    UI_EVENT_CHAR_S,
    UI_EVENT_CHAR_T,
    UI_EVENT_CHAR_U,
    UI_EVENT_CHAR_V,
    UI_EVENT_CHAR_W,
    UI_EVENT_CHAR_X,
    UI_EVENT_CHAR_Y,
    UI_EVENT_CHAR_Z
}
ui_event_t;

#endif /* _UI_H */
