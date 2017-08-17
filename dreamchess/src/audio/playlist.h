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

#ifndef AUDIO_PLAYLIST_H
#define AUDIO_PLAYLIST_H

#include "gamegui/queue.h"

typedef struct playlist_entry {
	int index;
	char *title;
	char *artist;
	char *album;
	char *filename;
	TAILQ_ENTRY(playlist_entry) entries;
} playlist_entry_t;

typedef TAILQ_HEAD(playlist, playlist_entry) playlist_t;

playlist_t *playlist_create(void);
void playlist_destroy(playlist_t *playlist);
void playlist_add_tracks(playlist_t *playlist, char *filename);

#endif
