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

#include <convert.h>
#include <mesh.h>
#include <structure.h>

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <assert.h>

void libmd3_unpack_normals(libmd3_mesh * mesh)
{
    int i;
    uint8_t lat, lng;
    float flat, flng;

    if (mesh->mesh_header->vertex_count < 2) {
        return;
    }

    mesh->normals = (float*)calloc(mesh->mesh_header->vertex_count * 3, sizeof(float));

    for(i = 0; i < mesh->mesh_header->vertex_count; ++i) {
        lat = (mesh->vertices[i * 4 + 3] >> 8) & 0xff;
        lng = (mesh->vertices[i * 4 + 3]) & 0xff;

        flat = lat * (3.14159265f / 128.f);
        flng = lng * (3.14159265f / 128.f);

        mesh->normals[i * 3 + 0] = cos(flat) * sin(flng);
        mesh->normals[i * 3 + 1] = sin(flat) * sin(flng);
        mesh->normals[i * 3 + 2] =             cos(flng);

        if (i == 0) { continue; }

        memmove(&mesh->vertices[i * 3],
                &mesh->vertices[i * 4],
                3 * sizeof(int16_t));
    }
}
