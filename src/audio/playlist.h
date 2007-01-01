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

typedef struct playlist_entry {
	int index;
	char *title;
	char *artist;
	char *album;
	char *filename;
	TAILQ_ENTRY(playlist_entry) entries;
} playlist_entry_t;

typedef TAILQ_HEAD(playlist, playlist_entry) playlist_t;

playlist_t *playlist_create();
void playlist_destroy(playlist_t *playlist);
void playlist_add_tracks(playlist_t *playlist, char *filename);
