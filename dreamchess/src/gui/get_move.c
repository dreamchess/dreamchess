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

#include "ui_sdlgl.h"

#define KEYBOARD_CAM_SPEED (60 / get_fps())
#define MOUSE_CAM_SPEED 1

/** @brief Main input routine.
 *
 *  Handles keyboard commands. When the user selects a chess piece
 *  selected_piece is updated.
 *
 *  @return If the user selected a chess piece a value between 0 (A1) and 63
 *          (H8) is returned. -1 if no chess piece was selected.
 */
int last_mousex = 0;
int last_mousey = 0;

int get_move(void) {
	int retval = -1;
	int mousex, mousey;
	SDL_Event event;
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);
	Uint8 mousestate = SDL_GetMouseState(&mousex, &mousey);

	if (mousestate & SDL_BUTTON_MIDDLE) {
		move_camera(-(mousey - last_mousey) * MOUSE_CAM_SPEED, -(mousex - last_mousex) * MOUSE_CAM_SPEED);
		last_mousex = mousex;
		last_mousey = mousey;
	} else {
		last_mousex = mousex;
		last_mousey = mousey;
	}

	if (keystate[SDL_SCANCODE_LCTRL]) {
		if (keystate[SDL_SCANCODE_DOWN])
			move_camera(-KEYBOARD_CAM_SPEED, 0.0f);
		if (keystate[SDL_SCANCODE_LEFT])
			move_camera(0.0f, -KEYBOARD_CAM_SPEED);
		if (keystate[SDL_SCANCODE_RIGHT])
			move_camera(0.0f, KEYBOARD_CAM_SPEED);
		if (keystate[SDL_SCANCODE_UP])
			move_camera(KEYBOARD_CAM_SPEED, 0.0f);

		while (SDL_PollEvent(&event))
			if (event.type == SDL_QUIT)
				/* FIXME */
				exit(0);
	}

	while (SDL_PollEvent(&event)) {
		gg_event_t gg_event;

		if (event.type == SDL_QUIT)
			/* FIXME */
			exit(0);

		if (event.type == SDL_MOUSEMOTION)
			set_mouse_pos(event.motion.x, event.motion.y);

		if (!gg_dialog_get_active() && event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
			retval = get_mouse_square();
			if (retval != -1)
				select_piece(retval);

			continue;
		}

		if (!gg_dialog_get_active() && event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT) {
			gg_dialog_open(dialog_system_create());

			continue;
		}

		if ((event.type == SDL_KEYDOWN && event.key.keysym.mod & KMOD_ALT && event.key.keysym.sym == SDLK_RETURN) ||
			(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_F11)) {
			DBG_LOG("toggled fullscreen");
			toggle_fullscreen();
			continue;
		}

		gg_event = convert_event(&event);

		if (gg_event.type == GG_EVENT_NONE)
			continue;

		if (gg_dialog_get_active())
			gg_dialog_input_current(gg_event);

		/* In the promote dialog */
		else if (gg_event.type == GG_EVENT_KEY)
			switch (gg_event.key) {
			case GG_KEY_LEFT:
				move_selector(SELECTOR_LEFT);
				break;
			case GG_KEY_RIGHT:
				move_selector(SELECTOR_RIGHT);
				break;
			case GG_KEY_UP:
				move_selector(SELECTOR_UP);
				break;
			case GG_KEY_DOWN:
				move_selector(SELECTOR_DOWN);
				break;
			case GG_KEY_ACTION:
				retval = get_selector();
				select_piece(retval);
				break;
			case GG_KEY_ESCAPE:
				gg_dialog_open(dialog_system_create());
				break;
			case 'g':
			case GG_KEY_EXTRA3:
				/* gg_dialog_open(dialog_ingame_create()); */
				break;
			case 'p':
				game_view_prev();
				break;
			case 'n':
				game_view_next();
				break;
			case 'r':
				game_retract_move();
				break;
			default:
				break;
			}
	}
	return retval;
}
