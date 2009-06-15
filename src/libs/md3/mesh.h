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

/* Modified for Dreamchess */

#ifndef LIBMD3_MESH_H
#define LIBMD3_MESH_H

#include <inttypes.h>

typedef struct _libmd3_mesh {
    struct _md3_mesh * mesh_header;   /* Header for this mesh */
    struct _md3_skin * skins;         /* skins */
    int32_t *          triangles;
    float *            texcoords;
    int16_t *          vertices;
    float *            normals;
    union {
        void * p;
        int i;
        unsigned u;
    } user;                           /* User data */
} libmd3_mesh;

#endif /* LIBMD3_MESH_H */
