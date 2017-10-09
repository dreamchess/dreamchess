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

#include <SDL.h>
#include <SDL_Image.h>
#include <GL/glew.h>
#include "Model.h"
#include "System.h"

#include <stdio.h>
#include <string>

int Model::load(const char *mesh) { //, const char *texture) {
	return true;
}

Mesh *Model::loadDCM(const char *filename) {
    FILE *f;
    Mesh *mesh;
    int version;
    char id[3];
    int vertices;
    int i;

    f = fopen(filename, "r");

    if (!f) {
        //DBG_ERROR("couldn't open %s", filename);
        return NULL;
    }

    if ((fscanf(f, "%c%c%c %d\n", &id[0], &id[1], &id[2], &version) != 4)
            || ((id[0] != 'D') || (id[1] != 'C') || (id[2] != 'M'))) {

        //DBG_ERROR("invalid DCM file header");
        return NULL;
    }

    if (version != 100) {
        //DBG_ERROR( "DCM version %i not supported", version);
        return NULL;
    }

    if (fscanf(f, "%d\n", &vertices) != 1) {
        //DBG_ERROR("error reading DCM file");
        return NULL;
    }

    mesh = new Mesh(); //(Mesh *)malloc(sizeof(mesh_t));

    mesh->has_bones = 0;
    mesh->vertices = vertices;
    mesh->vertex = (GLfloat *)malloc(sizeof(GLfloat) * vertices * 3);

    for (i = 0; i < vertices * 3; i++) {
        if (fscanf(f, "%f\n", &mesh->vertex[i]) != 1) {
            //DBG_ERROR("error reading DCM file");
            exit(1);
        }
    }

    mesh->normal = (GLfloat *)malloc(sizeof(GLfloat) * vertices * 3);

    for (i = 0; i < vertices * 3; i++) {
        if (fscanf(f, "%f\n", &mesh->normal[i]) != 1) {
            //DBG_ERROR("error reading DCM file");
            exit(1);
        }
    }

    mesh->tex_coord = (GLfloat *)malloc(sizeof(GLfloat) * vertices * 2);

    for (i = 0; i < vertices * 2; i++) {
        if (fscanf(f, "%f\n", &mesh->tex_coord[i]) != 1) {
            //DBG_ERROR("error reading DCM file");
            exit(1);
        }
    }

    /* As we don't flip our images we flip our u coordinates instead. */
    for (i = 1; i < vertices * 2; i += 2)
        mesh->tex_coord[i] = 1.0f - mesh->tex_coord[i];

    if (fscanf(f, "%d\n", &mesh->groups) != 1) {
        //DBG_ERROR("error reading DCM file");
        exit(1);
    }

    mesh->group = (group_t *)malloc(sizeof(group_t) * mesh->groups);

    for (i = 0; i < mesh->groups; i++) {
        char line[11];
        int group_len;
        int j;

        fgets(line, 11, f);

        if (!strcmp(line, "STRIP\n"))
            mesh->group[i].type = PRIM_STRIP;
        else if (!strcmp(line, "TRIANGLES\n"))
            mesh->group[i].type = PRIM_TRIANGLES;
        else
        {
            //DBG_ERROR("error reading DCM file");
            exit(1);
        }

        if (fscanf(f, "%d\n", &group_len) != 1)
        {
            //DBG_ERROR("error reading DCM file");
            exit(1);
        }

        mesh->group[i].len = group_len;

        mesh->group[i].data = (GLuint *)malloc(sizeof(GLuint) * group_len);

        for (j = 0; j < group_len; j++)
        {
            if (fscanf(f, "%u\n", &mesh->group[i].data[j]) != 1)
            {
                //DBG_ERROR("error reading DCM file");
                exit(1);
            }
        }
    }

    fclose(f);

    return mesh;
}