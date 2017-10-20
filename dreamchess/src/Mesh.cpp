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
#include <SDL_image.h>
#include <GL/glew.h>
#include "Mesh.h"
#include "System.h"

#include <stdio.h>
#include <string>

void Mesh::makeList() {
    int g;

    list = glGenLists(1);
    glNewList(list, GL_COMPILE);

    for (g = 0; g < groups; g++)
    {
        int i;

        switch (group[g].type)
        {
        case PRIM_TRIANGLES:
            glBegin(GL_TRIANGLES);
            break;
        case PRIM_STRIP:
            glBegin(GL_TRIANGLE_STRIP);
        }

        for (i = 0; i < group[g].len; i++)
        {
            unsigned int *data = group[g].data;

            glTexCoord2fv(tex_coord + data[i] * 2);
            glNormal3fv(normal + data[i] * 3);
            glVertex3fv(vertex + data[i] * 3);
        }

        glEnd();
    }

    glEndList();
}

int Mesh::loadDCM(const char *filename) {
    FILE *f;
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

    //mesh = new Mesh(); //(Mesh *)malloc(sizeof(mesh_t));

    has_bones = 0;
    vertices = vertices;
    vertex = (GLfloat *)malloc(sizeof(GLfloat) * vertices * 3);

    for (i = 0; i < vertices * 3; i++) {
        if (fscanf(f, "%f\n", &vertex[i]) != 1) {
            //DBG_ERROR("error reading DCM file");
            exit(1);
        }
    }

    normal = (GLfloat *)malloc(sizeof(GLfloat) * vertices * 3);

    for (i = 0; i < vertices * 3; i++) {
        if (fscanf(f, "%f\n", &normal[i]) != 1) {
            //DBG_ERROR("error reading DCM file");
            exit(1);
        }
    }

    tex_coord = (GLfloat *)malloc(sizeof(GLfloat) * vertices * 2);

    for (i = 0; i < vertices * 2; i++) {
        if (fscanf(f, "%f\n", &tex_coord[i]) != 1) {
            //DBG_ERROR("error reading DCM file");
            exit(1);
        }
    }

    /* As we don't flip our images we flip our u coordinates instead. */
    for (i = 1; i < vertices * 2; i += 2)
        tex_coord[i] = 1.0f - tex_coord[i];

    if (fscanf(f, "%d\n", &groups) != 1) {
        //DBG_ERROR("error reading DCM file");
        exit(1);
    }

    group = (group_t *)malloc(sizeof(group_t) * groups);

    for (i = 0; i < groups; i++) {
        char line[11];
        int group_len;
        int j;

        fgets(line, 11, f);

        if (!strcmp(line, "STRIP\n"))
            group[i].type = PRIM_STRIP;
        else if (!strcmp(line, "TRIANGLES\n"))
            group[i].type = PRIM_TRIANGLES;
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

        group[i].len = group_len;

        group[i].data = (GLuint *)malloc(sizeof(GLuint) * group_len);

        for (j = 0; j < group_len; j++)
        {
            if (fscanf(f, "%u\n", &group[i].data[j]) != 1)
            {
                //DBG_ERROR("error reading DCM file");
                exit(1);
            }
        }
    }

    fclose(f);

    makeList();

    return true;
}