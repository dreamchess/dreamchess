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

#include <mxml.h>
#include <limits.h>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#else
#include <unistd.h>
#include <dirent.h>
#endif

#include "dreamchess.h"
#include "theme.h"
#include "debug.h"
#include "options.h"
#include "dir.h"

static music_packs_t music_packs;

static int load_opaque(mxml_node_t *top, char *name, char **dest)
{
    mxml_node_t *node = mxmlFindElement(top, top, name, NULL, NULL, MXML_DESCEND);
    if (node)
    {
        node = mxmlWalkNext(node, node, MXML_DESCEND);
        if (node && node->type == MXML_OPAQUE)
        {
            free(*dest);
            *dest = strdup(node->value.opaque);
            return 0;
        }
    }
    return 1;
}

static void theme_add_theme( char *xmlfile, option_t *option )
{
    FILE *fp = NULL;
    mxml_node_t *tree=NULL, *theme=NULL;

    fp = fopen(xmlfile, "r");
    if (fp)
        tree = mxmlLoadFile(NULL, fp, MXML_OPAQUE_CALLBACK);
    else
        DBG_ERROR("error opening theme file" );

    fclose(fp);

    theme = tree;

    DBG_LOG("loading %s", xmlfile );
    while ((theme = mxmlFindElement(theme, tree, "theme", NULL, NULL, MXML_DESCEND)))
    {
        struct theme_struct *cur_theme = malloc(sizeof(struct theme_struct));
        mxml_node_t *node, *node2;
        /* Set theme to defaults.. incase we have missing bits..*/
        cur_theme->name = strdup("Untitled");
        cur_theme->style = strdup("default");
        cur_theme->pieces = strdup("classiclow");
        cur_theme->board = strdup("classic");
        cur_theme->white_name = strdup("White");
        cur_theme->black_name = strdup("Black");
        cur_theme->lighting=TRUE;
        cur_theme->selector.colour[0]=1.0;
        cur_theme->selector.colour[1]=1.0;
        cur_theme->selector.colour[2]=1.0;
        cur_theme->selector.colour[3]=1.0;  
        cur_theme->selector.spinspeed=0;
        cur_theme->selector.size=1;
        cur_theme->selector.bouncespeed=0;

        load_opaque(theme, "name", &cur_theme->name);
        load_opaque(theme, "style", &cur_theme->style);
        load_opaque(theme, "pieces", &cur_theme->pieces);
        load_opaque(theme, "board", &cur_theme->board);
        load_opaque(theme, "white_name", &cur_theme->white_name);
        load_opaque(theme, "black_name", &cur_theme->black_name);

        node = mxmlFindElement(theme, theme, "selector", NULL, NULL, MXML_DESCEND);
        if (node)
        {
            char *temp=(char*)mxmlElementGetAttr(node, "spinspeed");
            if ( temp )
                cur_theme->selector.spinspeed=atof(temp);

            temp=(char*)mxmlElementGetAttr(node, "size");
            if ( temp )
                cur_theme->selector.size=atof(temp);

            temp=(char*)mxmlElementGetAttr(node, "bouncespeed");
            if ( temp )
            {
                cur_theme->selector.bouncespeed=atof(temp);
            }

            node = mxmlWalkNext(node, node, MXML_DESCEND);
            node = mxmlFindElement(node, node, "colour", NULL, NULL, MXML_DESCEND);
            if (node)
            {
                node2 = mxmlWalkNext(node, node, MXML_DESCEND);
                node2 = mxmlFindElement(node2, node2, "red", NULL, NULL, MXML_DESCEND);  
                if (node2)
                {
                    node2 = mxmlWalkNext(node2, node2, MXML_DESCEND);
                    cur_theme->selector.colour[0]=atof(node2->value.opaque);
                }       

                node2 = mxmlWalkNext(node, node, MXML_DESCEND);
                node2 = mxmlFindElement(node2, node2, "green", NULL, NULL, MXML_DESCEND);  
                if (node2)
                {
                    node2 = mxmlWalkNext(node2, node2, MXML_DESCEND);
                    cur_theme->selector.colour[1]=atof(node2->value.opaque);
                }       

                node2 = mxmlWalkNext(node, node, MXML_DESCEND);
                node2 = mxmlFindElement(node2, node2, "blue", NULL, NULL, MXML_DESCEND);  
                if (node2)
                {
                    node2 = mxmlWalkNext(node2, node2, MXML_DESCEND);
                    cur_theme->selector.colour[2]=atof(node2->value.opaque);
                }        

                node2 = mxmlWalkNext(node, node, MXML_DESCEND);
                node2 = mxmlFindElement(node2, node2, "alpha", NULL, NULL, MXML_DESCEND);  
                if (node2)
                {
                    node2 = mxmlWalkNext(node2, node2, MXML_DESCEND);
                    cur_theme->selector.colour[3]=atof(node2->value.opaque);
                }               
            }
        }

        DBG_LOG("added theme: %s %s", cur_theme->name, cur_theme->style );
        option_add_value(option, cur_theme->name, cur_theme);
    }

    mxmlDelete(tree);
}

#ifdef _WIN32

static void find_themes(option_t *option)
{
    HANDLE hFind;
    WIN32_FIND_DATA ffd;

    hFind = FindFirstFile("themes\\*.xml", &ffd);

    if (hFind == INVALID_HANDLE_VALUE)
        return;

    do {
        char *filename = malloc(strlen(ffd.cFileName) + 7 + 1);
        strcpy(filename, "themes\\");
        strcat(filename, ffd.cFileName);
        theme_add_theme(filename, option);
        free(filename);
    } while (FindNextFile(hFind, &ffd) != 0);

    FindClose(hFind);
}

#else

static void find_themes(option_t *option)
{
    DIR* themedir;
    struct dirent* themedir_entry;

    if ( (themedir=opendir("themes")) != NULL )
    {
        while ((themedir_entry = readdir(themedir)) != NULL)
        {
            char temp[80];
            if ( themedir_entry->d_name[0] != '.' )
            {
                sprintf( temp, "themes/%s", themedir_entry->d_name );
                theme_add_theme(temp, option);
            }
        }
        closedir(themedir);
    }
}

#endif

void theme_find_themes(option_t *option)
{
    ch_datadir();
    find_themes(option);

    ch_userdir();
    find_themes(option);

    option_select_value_by_name(option, "Classic Wooden");
}

static void add_music_pack(char *dir)
{
    music_pack_t *music = malloc(sizeof(music_pack_t));

    music->dir = strdup(dir);

    TAILQ_INSERT_TAIL(&music_packs, music, entries);
}

#ifdef _WIN32

static void find_music_packs(void)
{
    char *cur_dir = getcwd(NULL, 0);

    if (!cur_dir)
        DBG_ERROR("could not determine current directory");

    HANDLE hFind;
    WIN32_FIND_DATA ffd;

    hFind = FindFirstFile("music\\*", &ffd);

    if (hFind == INVALID_HANDLE_VALUE)
        return;

    do {
        if ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && ffd.cFileName[0] != '.') {
            char *dirname = malloc(strlen(cur_dir) + 7 + strlen(ffd.cFileName) + 1);

            strcpy(dirname, cur_dir);
            strcat(dirname, "\\music\\");
            strcat(dirname, ffd.cFileName);

            add_music_pack(dirname);
            free(dirname);
        }
    } while (FindNextFile(hFind, &ffd) != 0);

    FindClose(hFind);
    free(cur_dir);
}

#else

static void find_music_packs(void)
{
    DIR *dir;

#ifdef PATH_MAX
    char cur_dir[PATH_MAX];

    if (!getcwd(cur_dir, PATH_MAX))
        DBG_ERROR("could not determine current directory");
#else
    /* PATH_MAX not defined, try getcwd extension */
    char *cur_dir = getcwd(NULL, 0);

    if (!cur_dir)
        DBG_ERROR("could not determine current directory");
#endif

    if ((dir = opendir("music")) != NULL)
    {
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL)
        {
            if (entry->d_name[0] != '.')
            {
                char *dirname = malloc(strlen(cur_dir) + strlen(entry->d_name) + 8);

                strcpy(dirname, cur_dir);
                strcat(dirname, "/music/");
                strcat(dirname, entry->d_name);

                add_music_pack(dirname);
                free(dirname);
            }
        }
        closedir(dir);
    }

#ifndef PATH_MAX
    free(cur_dir);
#endif
}

#endif

void theme_find_music_packs(void)
{
    TAILQ_INIT(&music_packs);

    ch_datadir();
    find_music_packs();

    ch_userdir();
    find_music_packs();

#ifdef _WIN32
    {
        HKEY key;
        char data[MAX_PATH];
        DWORD size = MAX_PATH - 1;

        if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
            "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\DreamChess Music",
            0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS)
        {
            DBG_LOG("couldn't find music pack in registry");
            return;
        }

        if (RegQueryValueEx(key, "InstallLocation", NULL, NULL, data, &size)
            != ERROR_SUCCESS)
        {
            DBG_LOG("couldn't find music pack in registry");
            return;
        }

        data[size] = 0;
        DBG_LOG("music pack location: %s", data);

        chdir(data);
        find_music_packs();
    }
#elif defined(__APPLE__)
    {
        if (chdir("/Library/Application Support/DreamChess") == 0)
            find_music_packs();
    }
#endif
}

music_packs_t *theme_get_music_packs(void)
{
    return &music_packs;
}
