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

#include "gamegui/queue.h"

enum {
	AUDIO_MENU_MOVE,
	AUDIO_MENU_CHANGE,
	AUDIO_MOVE,
	AUDIO_SOUNDS /* Must be last */
};

#define AUDIO_MAX_VOL 10

typedef struct sound {
	int id;
	char *filename;
} sound_t;

typedef void (*audio_music_callback_t)(char *title, char *artist, char *album);

void audio_init();

void audio_exit();

void audio_play_music();

void audio_poll(int title);

void audio_set_music_callback(audio_music_callback_t callback);

void audio_play_sound(int id);

void audio_set_sound_volume(int vol);

void audio_set_music_volume(int vol);
