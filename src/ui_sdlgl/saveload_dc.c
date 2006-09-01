/*  DreamChess
 *  Copyright (C) 2006  The DreamChess project
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifdef _arch_dreamcast

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dc/vmu_pkg.h>
#include "minizip/zip.h"
#include "minizip/unzip.h"

#define FILENAME "DRMCHESS.SYS"
#define ZIPNAME "save.zip"
#define MAX_NAME_LEN 13
#define BUF_SIZE 1024

static char palette[] =
    {
        0x32, 0xf4, /*  0 */
        0x31, 0xf4, /*  1 */
        0x33, 0xf3, /*  2 */
        0x42, 0xf5, /*  3 */
        0x43, 0xf5, /*  4 */
        0x42, 0xf6, /*  5 */
        0x54, 0xf6, /*  6 */
        0x53, 0xf7, /*  7 */
        0x63, 0xf9, /*  8 */
        0x64, 0xf8, /*  9 */
        0x74, 0xfa, /* 10 */
        0x84, 0xfa, /* 11 */
        0x84, 0xfb, /* 12 */
        0x96, 0xfc, /* 13 */
        0xb8, 0xfe, /* 14 */
        0xf0, 0x00  /* 15 */
    };

static char icon[] =
    {
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf2, 0x44,
        0x66, 0x4f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xf2, 0x37, 0x8b,
        0xdc, 0xa8, 0x4f, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x03, 0x78,
        0xaa, 0x87, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x23, 0x7a,
        0xba, 0x86, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xf4, 0x33, 0x55,
        0x53, 0x33, 0x4f, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0x26, 0x63, 0x13, 0x57, 0x8a, 0xa8,
        0x75, 0xab, 0xba, 0x99, 0x8b, 0x94, 0xff, 0xff,
        0xff, 0xff, 0x03, 0x73, 0x33, 0x77, 0x8d, 0xdd,
        0xd9, 0xad, 0xcb, 0xaa, 0xbd, 0xa6, 0xff, 0xff,
        0xff, 0xff, 0x21, 0x33, 0x33, 0x77, 0x8b, 0xee,
        0xeb, 0xab, 0xbb, 0xab, 0xcb, 0x74, 0xff, 0xff,
        0xff, 0xff, 0xf1, 0x13, 0x13, 0x57, 0x7a, 0xdd,
        0xdb, 0xab, 0xbb, 0xab, 0xba, 0x7f, 0xff, 0xff,
        0xff, 0xff, 0xf2, 0x11, 0x13, 0x57, 0x78, 0xaa,
        0xbb, 0xbb, 0xbb, 0xaa, 0xa8, 0x6f, 0xff, 0xff,
        0xff, 0xff, 0xf2, 0x11, 0x13, 0x57, 0x78, 0x8a,
        0xaa, 0xcc, 0xca, 0xaa, 0x87, 0x4f, 0xff, 0xff,
        0xff, 0xff, 0xff, 0x11, 0x13, 0x57, 0x78, 0x8a,
        0xac, 0xcc, 0xcb, 0xaa, 0x87, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0x01, 0x13, 0x57, 0x78, 0x8a,
        0xac, 0xcc, 0xcc, 0xaa, 0x76, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0x21, 0x13, 0x57, 0x88, 0x8a,
        0xbc, 0xcc, 0xcc, 0xa8, 0x74, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0x21, 0x13, 0x57, 0x88, 0xab,
        0xcc, 0xdc, 0xcc, 0xa8, 0x72, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xf1, 0x13, 0x57, 0x88, 0xab,
        0xcd, 0xdd, 0xcc, 0xa8, 0x7f, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xf1, 0x13, 0x57, 0x8a, 0xbc,
        0xde, 0xed, 0xcc, 0xaa, 0x7f, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xf1, 0x13, 0x57, 0x8a, 0xbd,
        0xde, 0xed, 0xcc, 0xca, 0x7f, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xf1, 0x13, 0x57, 0x8a, 0xbd,
        0xde, 0xed, 0xcc, 0xa8, 0x7f, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xf1, 0x13, 0x57, 0x8a, 0xbd,
        0xde, 0xed, 0xcc, 0xa8, 0x7f, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xf1, 0x13, 0x57, 0x88, 0xbb,
        0xdd, 0xdd, 0xcc, 0xa8, 0x7f, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xf0, 0x11, 0x35, 0x78, 0x8a,
        0xbb, 0xbb, 0xaa, 0xa8, 0x6f, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xf2, 0x11, 0x35, 0x57, 0x78,
        0x8a, 0xa8, 0x88, 0x77, 0x4f, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xf2, 0x11, 0x33, 0x57, 0x8b,
        0xbb, 0xba, 0x88, 0x75, 0x4f, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0x03, 0x57, 0x8b, 0xde,
        0xee, 0xed, 0xcb, 0xa7, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xf2, 0x33, 0x77, 0x8b, 0xdd,
        0xee, 0xed, 0xcb, 0xa8, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0x23, 0x57, 0x78, 0xaa, 0xbb,
        0xbb, 0xcc, 0xcc, 0xba, 0x86, 0xff, 0xff, 0xff,
        0xff, 0xff, 0x01, 0x35, 0x77, 0x8a, 0xbd, 0xde,
        0xee, 0xee, 0xed, 0xcc, 0xba, 0x97, 0xff, 0xff,
        0xff, 0xff, 0x22, 0x24, 0x46, 0x57, 0x9a, 0xbb,
        0xdd, 0xdd, 0xba, 0x89, 0x99, 0x64, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0x11, 0x13, 0x33,
        0x33, 0x31, 0x11, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0x33, 0x33,
        0x33, 0x33, 0x10, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0x01, 0x13, 0x35,
        0x55, 0x53, 0x30, 0xff, 0xff, 0xff, 0xff, 0xff
    };

static int zip_savegames()
{
    zipFile zip;
    file_t dir;
    dirent_t *dirent;

    zip = zipOpen("/ram/" ZIPNAME, APPEND_STATUS_CREATE);
    if (zip == NULL)
    {
        printf("Failed to create zip file\n");
        return -1;
    }

    fs_chdir("/ram");
    dir = fs_open(".", O_RDONLY | O_DIR);

    if (!dir)
    {
        printf("Failed to read /ram directory\n");
        return -1;
    }

    while ((dirent = fs_readdir(dir)))
    {
        void *data;
        int len;
        file_t file;

        if (!strcmp(dirent->name, ZIPNAME))
            continue;

        if (!(file = fs_open(dirent->name, O_RDONLY)))
        {
            printf("Failed to open %s\n", dirent->name);
            return -1;
        }

        len = fs_total(file);
        data = fs_mmap(file);

        if (!data || (len < 0))
            return -1;
        if (zipOpenNewFileInZip(zip, dirent->name, NULL, NULL, 0, NULL, 0, NULL, Z_DEFLATED, 9) != ZIP_OK)
            return -1;
        if (zipWriteInFileInZip(zip, data, len) != ZIP_OK)
            return -1;
        if (zipCloseFileInZip(zip) != ZIP_OK)
            return -1;

        fs_close(file);
    }

    fs_close(dir);
    if (zipClose(zip, NULL) != ZIP_OK)
        return -1;

    return 0;
}

static int package_savegames(char *vmupath)
{
    file_t inf, outf;
    uint8 *data, *pkg_out;
    vmu_pkg_t pkg;
    int pkg_size;
    char *outfname;

    if (!(inf = fs_open("/ram/" ZIPNAME, O_RDONLY)))
    {
        printf("Failed to open /ram/" ZIPNAME "\n");
        return -1;
    }

    if (!(data = fs_mmap(inf)))
    {
        printf("Failed to mmap zip file\n");
        fs_close(inf);
        return -1;
    }

    strcpy(pkg.desc_short, "Save Game");
    strcpy(pkg.desc_long, "DreamChess");
    strcpy(pkg.app_id, "DreamChess");

    pkg.icon_cnt = 1;
    pkg.icon_anim_speed = 0;
    pkg.icon_data = icon;
    memcpy(pkg.icon_pal, palette, 32);

    pkg.eyecatch_type = VMUPKG_EC_NONE;
    pkg.data = data;
    pkg.data_len = fs_total(inf);

    if (vmu_pkg_build(&pkg, &pkg_out, &pkg_size) < 0)
    {
        printf("Failed to build package\n");
        fs_close(inf);
        return -1;
    }

    outfname = malloc(strlen(vmupath) + strlen(FILENAME) + 2);
    strcpy(outfname, vmupath);
    strcat(outfname, "/" FILENAME);

    if (!(outf = fs_open(outfname, O_WRONLY | O_TRUNC)))
    {
        printf("Failed to create file on VMU\n");
        fs_close(inf);
        free(outfname);
        return -1;
    }

    free(outfname);

    if (fs_write(outf, pkg_out, pkg_size) < pkg_size)
    {
        printf("Failed to write to VMU file\n");
        fs_close(inf);
        fs_close(outf);
        return -1;
    }

    fs_close(inf);
    fs_close(outf);

    return 0;
}

static int unzip_savegames()
{
    unzFile zip;
    char *buf = malloc(BUF_SIZE);

    zip = unzOpen("/ram/" ZIPNAME);
    if (zip == NULL)
    {
        printf("Failed to open zip file\n");
        return -1;
    }

    fs_chdir("/ram");

    if (unzGoToFirstFile(zip) != ZIP_OK)
        return -1;

    while (1)
    {
        char filename[MAX_NAME_LEN];
        file_t f;

        if (unzGetCurrentFileInfo(zip, NULL, filename, MAX_NAME_LEN, NULL, 0, NULL, 0) != ZIP_OK)
            return -1;

        if (!(f = fs_open(filename, O_WRONLY)))
        {
            printf("Failed to create %s\n", filename);
            return -1;
        }

        if (unzOpenCurrentFile(zip) != ZIP_OK)
            return -1;

        while (1)
        {
            int bytes = unzReadCurrentFile(zip, buf, BUF_SIZE);

            if (bytes < 0)
                return -1;

            if (bytes == 0)
                break;

            if (fs_write(f, buf, bytes) < bytes)
                return -1;
        }

        fs_close(f);

        if (unzGoToNextFile(zip) != ZIP_OK)
            break;
    }

    free(buf);

    if (unzClose(zip) != ZIP_OK)
        return -1;

    return 0;
}

static int unpackage_savegames(char *vmupath)
{
    char *infname;
    char *data;
    file_t inf, outf;
    vmu_pkg_t pkg;

    infname = malloc(strlen(vmupath) + strlen(FILENAME) + 2);
    strcpy(infname, vmupath);
    strcat(infname, "/" FILENAME);

    if (!(inf = fs_open(infname, O_RDONLY)))
    {
        printf("Failed to open file on VMU\n");
        free(infname);
        return -1;
    }

    free(infname);

    if (!(data = fs_mmap(inf)))
    {
        printf("Failed to mmap VMU file\n");
        fs_close(inf);
        return -1;
    }

    if (vmu_pkg_parse(data, &pkg) == -1) {
        printf("Failed to parse VMU file header\n");
        fs_close(inf);
        return -1;
    }

    if (!(outf = fs_open("/ram/" ZIPNAME, O_WRONLY | O_TRUNC))) {
        printf("Failed to create zip file\n");
        fs_close(inf);
        return -1;
    }

    if (fs_write(outf, pkg.data, pkg.data_len) < pkg.data_len) {
        printf("Failed to write zip file\n");
        fs_close(inf);
        fs_close(outf);
        return -1;
    }

    fs_close(inf);
    fs_close(outf);
    return 0;
}

static char *find_first_vmu() {
    file_t dir;
    dirent_t *dirent;
    char *retval = NULL;

    dir = fs_open("/vmu", O_RDONLY | O_DIR);

    if (!dir)
    {
        printf("Failed to read /vmu directory\n");
        return NULL;
    }

    if (!(dirent = fs_readdir(dir)) || (strlen(dirent->name) != 2))
    {
        printf("No VMU found\n");
        return NULL;
    }

    retval = malloc(5 + strlen(dirent->name) + 1);
    strcpy(retval, "/vmu/");
    strcat(retval, dirent->name);

    fs_close(dir);
    return retval;
}

static void clear_ramdisk()
{
    file_t dir;
    dirent_t *dirent;

    fs_chdir("/ram");
    dir = fs_open(".", O_RDONLY | O_DIR);

    if (!dir)
    {
        printf("Failed to read /ram directory\n");
        return;
    }

    while ((dirent = fs_readdir(dir)))
    {
        if (fs_unlink(dirent->name))
            printf("Failed to delete file /ram/%s\n", dirent->name);
    }

    fs_close(dir);
}

int dc_store_savegames()
{
    int retval;
    char *vmu;

    vmu = find_first_vmu();

    if (!vmu)
        return -1;

    if (!zip_savegames())
        retval = package_savegames(vmu);
    else
        retval = -1;

    free(vmu);
    return retval;
}

int dc_restore_savegames()
{
    int retval;
    char *vmu;

    clear_ramdisk();
    vmu = find_first_vmu();

    if (!vmu)
        return -1;

    if (!unpackage_savegames(vmu))
        retval = unzip_savegames();
    else
        retval = -1;

    free(vmu);
    return retval;
}

#endif /* !_arch_dreamcast */
