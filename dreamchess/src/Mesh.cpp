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

    for (g = 0; g < _groups.size(); g++)
    {
        int i;

        switch (_groups[g].type) {
            case PRIM_TRIANGLES:
                glBegin(GL_TRIANGLES);
                break;
            case PRIM_STRIP:
                glBegin(GL_TRIANGLE_STRIP);
        }

        for (i = 0; i < _groups[g].len; i++) {
            unsigned int *data = _groups[g].data;

            glTexCoord2f(_uv.at(data[i]).x, _uv.at(data[i]).y);
            glNormal3f(_normals.at(data[i]).x, _normals.at(data[i]).y, _normals.at(data[i]).z);
            glVertex3f(_verticies.at(data[i]).x, _verticies.at(data[i]).y, _verticies.at(data[i]).z);
        }

        glEnd();
    }

    glEndList();
}

int Mesh::loadDCM(const char *filename) {
    std::string buf;
    std::ifstream fstream;

    fstream.open(filename, std::ifstream::in);

    if (fstream.fail()) {
        printf("Failed to open DCM file: '%s'\n", filename);
        return false;
    }

    fstream >> buf;
    if (buf != "DCM") {
        printf("Invalid DCM file header.\n");
        return false;
    }

    fstream >> buf;
    if (std::stoi(buf) != 100) {
        printf("DCM version %i is not supported.\n", std::stoi(buf));    
    }

    fstream >> buf;
    int vertexCount = std::stoi(buf);

    for (int i = 0; i < vertexCount; i++) {
        glm::vec3 vert;
        for (int j = 0; j < 3; j++) {
            fstream >> buf;
            vert[j] = std::stof(buf);
        }
        _verticies.push_back(vert);
    }

    for (int i = 0; i < vertexCount; i++) {
        glm::vec3 normal;
        for (int j = 0; j < 3; j++) {
            fstream >> buf;
            normal[j] = std::stof(buf);
        }
        _normals.push_back(normal);
    }

    for (int i = 0; i < vertexCount; i++) {
        glm::vec2 uv;
        for (int j = 0; j < 2; j++) {
            fstream >> buf;
            uv[j] = std::stof(buf);
        }
        _uv.push_back(uv);
    }

    fstream >> buf;
    int groupCount = std::stoi(buf);

    for (int i = 0; i < groupCount; i++) {
        group_t group;

        fstream >> buf;

        if (buf == "STRIP") {
            group.type = PRIM_STRIP;
        }
        else if (buf == "TRIANGLES") {
            group.type = PRIM_TRIANGLES;
        }

        fstream >> buf;
        group.len = std::stoi(buf);
        group.data = (GLuint *)malloc(sizeof(GLuint) * group.len);
        for (int j = 0; j < group.len; j++)
        {
            fstream >> buf;
            group.data[j] = std::stoi(buf);
        }

        _groups.push_back(group);
    }

    fstream.close(); 

    makeList();

    return true;
}