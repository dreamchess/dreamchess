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
#include <mxml.h>

#include "debug.h"
#include "playlist.h"

static char *read_opaque(mxml_node_t *top, char *name)
{
    mxml_node_t *node = mxmlFindElement(top, top, name, NULL, NULL, MXML_DESCEND);
    if (node)
    {
        node = mxmlWalkNext(node, node, MXML_DESCEND);
        if (node && node->type == MXML_OPAQUE)
        {
            return node->value.opaque;
        }
    }
    return NULL;
}

playlist_t *playlist_create()
{
	playlist_t *playlist = malloc(sizeof(playlist_t));

	TAILQ_INIT(playlist);

	return playlist;
}

void playlist_destroy(playlist_t *playlist)
{
	playlist_entry_t *entry;

	while ((entry = TAILQ_FIRST(playlist))) {
		free(entry->title);
		free(entry->album);
		free(entry->artist);
		free(entry->filename);
		TAILQ_REMOVE(playlist, entry, entries);
		free(entry);
	}
}

static void playlist_add_track(playlist_t *playlist, char *title, char *album,
		  char *artist, char *filename)
{
	playlist_entry_t *entry = malloc(sizeof(playlist_entry_t));

	entry->title = strdup(title);
	entry->album = strdup(album);
	entry->artist = strdup(artist);
	entry->filename = strdup(filename);

	TAILQ_INSERT_TAIL(playlist, entry, entries);
}

void playlist_add_tracks(playlist_t *playlist, char *dir)
{
	FILE *f;
	mxml_node_t *tree = NULL, *track = NULL;
	char *filename = malloc(strlen(dir) + strlen("/tracks.xml") + 1);

	strcpy(filename, dir);
	strcat(filename, "/tracks.xml");

	f = fopen(filename, "r");
	if (f)
		tree = mxmlLoadFile(NULL, f, MXML_OPAQUE_CALLBACK);
	else
		DBG_ERROR("could not open tracks file");

	track = tree;

	while ((track = mxmlFindElement(track, tree, "track", NULL, NULL, MXML_DESCEND)))
	{
		char *title, *album, *artist, *filename, *fullname;

		title = read_opaque(track, "title");
		if (!title)
			title = "Unknown title";

		album = read_opaque(track, "album");
		if (!album)
			album = "Unknown album";

		artist = read_opaque(track, "artist");
		if (!artist)
			artist = "Unknown artist";

		filename = read_opaque(track, "filename");

		if (!filename)
			DBG_ERROR("could not parse XML file");

		fullname = malloc(strlen(dir) + strlen(filename) + 2);
		strcpy(fullname, dir);
		strcat(fullname, "/");
		strcat(fullname, filename);

		playlist_add_track(playlist, title, album, artist, fullname);

		free(fullname);
	}

	free(filename);
	fclose(f);
}
