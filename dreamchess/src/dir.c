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

#include "dir.h"
#include "i18n.h"
#include "debug.h"

#ifdef _WIN32

#define USERDIR "DreamChess"

#include "shlobj.h"
#include "shlwapi.h"
#include <io.h>
#include <windows.h>
#include <strsafe.h>

static void get_module_dir(LPSTR buf, size_t size) {
	GetModuleFileName(NULL, buf, size);
	buf[MAX_PATH - 1] = '\0';
	PathRemoveFileSpec(buf);
}

int ch_datadir(void) {
	TCHAR datadir[MAX_PATH];
	get_module_dir(datadir, MAX_PATH);
	StringCbCatA(datadir, MAX_PATH, "\\data");
	return chdir(datadir);
}

int ch_userdir(void) {
	TCHAR appdir[MAX_PATH];

	if (SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, appdir))
		return -1;

	if (chdir(appdir))
		return -1;

	if (chdir(USERDIR)) {
		if (mkdir(USERDIR))
			return -1;

		return chdir(USERDIR);
	}

	return 0;
}

void init_i18n(void) {
	TCHAR localedir[MAX_PATH];
	get_module_dir(localedir, MAX_PATH);
	StringCbCatA(localedir, MAX_PATH, "\\locale");
	bindtextdomain("dreamchess", localedir);
	textdomain("dreamchess");
}

#elif defined __APPLE__

#include "CoreFoundation/CoreFoundation.h"
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

#define USERDIR "Library/Application Support/DreamChess"

int ch_datadir(void) {
	CFBundleRef mainBundle = CFBundleGetMainBundle();
	CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);

	if (!resourcesURL)
		return -1;

	char path[PATH_MAX];
	if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX)) {
		CFRelease(resourcesURL);
		return -1;
	}

	CFRelease(resourcesURL);
	return chdir(path);
}

int ch_userdir(void) {
	char *home = getenv("HOME");

	if (!home)
		return -1;

	if (chdir(home))
		return -1;

	if (chdir(USERDIR)) {
		if (mkdir(USERDIR, 0755))
			return -1;

		return chdir(USERDIR);
	}

	return 0;
}

void init_i18n(void) {
	CFBundleRef mainBundle = CFBundleGetMainBundle();
	CFURLRef sharedSupportURL = CFBundleCopySharedSupportURL(mainBundle);

	if (!sharedSupportURL)
		return;

	CFURLRef localeURL = CFURLCreateCopyAppendingPathComponent(NULL, sharedSupportURL, CFSTR("locale"), TRUE);

	CFRelease(sharedSupportURL);

	if (!localeURL)
		return;

	char path[PATH_MAX];
	if (CFURLGetFileSystemRepresentation(localeURL, TRUE, (UInt8 *)path, PATH_MAX)) {
		setlocale(LC_MESSAGES, "");
		setlocale(LC_CTYPE, "");
		bindtextdomain("dreamchess", path);
		textdomain("dreamchess");
	}

	CFRelease(localeURL);
}

#else /* !_WIN32 */

#define USERDIR ".dreamchess"

#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <locale.h>

int ch_datadir(void) {
	return chdir(DATADIR);
}

int ch_userdir(void) {
	char *home = getenv("HOME");

	if (!home)
		return -1;

	if (chdir(home))
		return -1;

	if (chdir(USERDIR)) {
		if (mkdir(USERDIR, 0755))
			return -1;

		return chdir(USERDIR);
	}

	return 0;
}

void init_i18n(void) {
	setlocale(LC_MESSAGES, "");
	setlocale(LC_CTYPE, "");
	bindtextdomain("dreamchess", LOCALEDIR);
	textdomain("dreamchess");
}

#endif

#ifdef _WIN32

char *dir_get_real_path(const char *filename) {
	return _fullpath(NULL, filename, 0);
}

#else

char *dir_get_real_path(const char *filename) {
	return realpath(filename, NULL);
}

#endif
