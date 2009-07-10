
#include "md3.h"

static void fixPath(char * filename)
{
    int i;
    for(i = 0; i < strlen(filename); ++i) {
        if (filename[i] == '\\') {
            filename[i] = '/';
        }
    }
}

static void draw_one_mesh(libmd3_mesh * mesh, int index,  int tx )
{
    /*if (mesh->mesh_header->skin_count != 0) {
        if (mesh->user.u == 0) {
            fixPath((char*)mesh->skins[0].name);
            mesh->user.u = tx; //LoadGLTexture(mesh->skins[0].name);
        }
    }*/

    mesh->user.u = tx;

    if (mesh->user.u != 0) 
    {
        glEnable(GL_TEXTURE_2D);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glBindTexture(GL_TEXTURE_2D, mesh->user.u);
    }

    glEnableClientState(GL_NORMAL_ARRAY);

    int vertex_offset=(mesh->mesh_header->vertex_count*3)*index;

    //printf

    glVertexPointer(3, GL_SHORT, 0, mesh->vertices+vertex_offset);
    glTexCoordPointer(2, GL_FLOAT, 0, mesh->texcoords);
    glNormalPointer(GL_FLOAT, 0, mesh->normals);
    glDrawElements(GL_TRIANGLES, mesh->mesh_header->triangle_count * 3,
                   GL_UNSIGNED_INT, mesh->triangles);

    glDisableClientState(GL_NORMAL_ARRAY);

    if (mesh->user.u != 0) {
        glDisable(GL_TEXTURE_2D);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
}

void md3::render( texture *tx )
{
    int i;
    libmd3_mesh * meshp;

    glPushMatrix();

    glScalef(0.001, 0.001, 0.001);

    if (modelFile->header->mesh_count == 0) 
    {
        printf("[No meshes in file]\n");
        return;
    }


    meshp = modelFile->meshes;    
    for(i = 0; i < modelFile->header->mesh_count; ++i, ++meshp) 
    {
        draw_one_mesh(meshp, 0, tx->texture_data.id);
    }

    glPopMatrix();      
}

md3::md3( std::string filename )
{
    int i;

    modelFile = libmd3_file_load((char*)filename.c_str());
    if (modelFile == NULL) 
    {
        //return 1;
    }

    for(i = 0; i < modelFile->header->mesh_count; ++i) 
    {
        libmd3_unpack_normals(&modelFile->meshes[i]);
    }

    //dump_frames(libmd3_file_load((char*)filename.c_str()));
}
