/*  DreamChess
**  Copyright (C) 2006  The DreamChess project
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

#include "gamegui/queue.h"

enum {
	AUDIO_MENU_MOVE,
	AUDIO_MENU_CHANGE,
	AUDIO_MOVE,
	AUDIO_SOUNDS /* Must be last */
};

typedef struct sound {
	int id;
	char *filename;
} sound_t;

typedef void (*audio_music_callback)(char *title, char *artist, char *album);

void audio_init();

void audio_exit();

void audio_play_music();

void audio_poll();

void audio_set_music_callback(audio_music_callback callback);

void audio_play_sound(int id);
