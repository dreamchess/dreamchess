/* Copyright (C) 2004 Alistair Riddoch <alriddoch@zepler.org>
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#ifndef LIBMD3_LOADER_H
#define LIBMD3_LOADER_H

struct _md3_header;
struct _md3_frame;
struct _md3_tag;
struct _libmd3_mesh;

typedef struct _libmd3_file {
    struct _md3_header *    header;   /* Indexes of vertices */
    struct _md3_frame *     frames;   /* frames */
    struct _md3_tag *       tags;     /* tags */
    struct _libmd3_mesh *   meshes;   /* meshes */
    union {
        void * p;
        int i;
        unsigned u;
    } user;                           /* User data */
} libmd3_file;

libmd3_file * libmd3_file_load(const char * filename);
void libmd3_file_free(libmd3_file * file);

#endif /* LIBMD3_LOADER_H */
