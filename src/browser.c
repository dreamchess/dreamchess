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

#include <string.h>
#include <stdlib.h>
#include "debug.h"

void browser_open(const char *url) {
	char *cmd;
#ifdef __WIN32__
	cmd = "cmd /c start";
#elif defined __APPLE__
	cmd = "open";
#else
	cmd = "xdg-open";
#endif

	if (strncmp(url, "http://", 7))
		return;

	char *run = malloc(strlen(cmd) + strlen(url) + 4);
	strcpy(run, cmd);
	strcat(run, " \"");
	strcat(run, url);
	strcat(run, "\"");

	DBG_LOG("running system command %s", run);
	system(run);
	free(run);
}
