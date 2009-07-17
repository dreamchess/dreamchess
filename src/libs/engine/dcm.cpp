
#include "dcm.h"

void mesh_make_list(mesh_t *msh)
{
    int g;

    msh->list = glGenLists(1);
    glNewList(msh->list, GL_COMPILE);

    for (g = 0; g < msh->groups; g++)
    {
        int i;

        switch (msh->group[g].type)
        {
        case PRIM_TRIANGLES:
            glBegin(GL_TRIANGLES);
            break;
        case PRIM_STRIP:
            glBegin(GL_TRIANGLE_STRIP);
        }

        for (i = 0; i < msh->group[g].len; i++)
        {
            unsigned int *data = (unsigned int*)msh->group[g].data;

            glTexCoord2fv(msh->tex_coord + data[i] * 2);
            glNormal3fv(msh->normal + data[i] * 3);
            glVertex3fv(msh->vertex + data[i] * 3);
        }

        glEnd();
    }

    glEndList();
}

mesh_t *dcm_load(char *filename)
{
    FILE *f;
    mesh_t *msh;
    int version;
    char id[3];
    int vertices;
    int i;

    f = fopen(filename, "r");

    if (!f)
    {
        //DBG_ERROR("couldn't open %s", filename);
        return NULL;
    }

    if ((fscanf(f, "%c%c%c %d\n", &id[0], &id[1], &id[2], &version) != 4)
            || ((id[0] != 'D') || (id[1] != 'C') || (id[2] != 'M')))
    {
        //DBG_ERROR("invalid DCM file header");
        return NULL;
    }

    if (version != 100)
    {
        //DBG_ERROR( "DCM version %i not supported", version);
        return NULL;
    }

    if (fscanf(f, "%d\n", &vertices) != 1)
    {
        //DBG_ERROR("error reading DCM file");
        return NULL;
    }

    msh = (mesh_t*)malloc(sizeof(mesh_t));

    msh->has_bones = 0;
    msh->vertices = vertices;
    msh->vertex = (GLfloat*)malloc(sizeof(GLfloat) * vertices * 3);

    for (i = 0; i < vertices * 3; i++)
    {
        if (fscanf(f, "%f\n", &msh->vertex[i]) != 1)
        {
            //DBG_ERROR("error reading DCM file");
            exit(1);
        }
    }

    msh->normal = (GLfloat*)malloc(sizeof(GLfloat) * vertices * 3);

    for (i = 0; i < vertices * 3; i++)
    {
        if (fscanf(f, "%f\n", &msh->normal[i]) != 1)
        {
            //DBG_ERROR("error reading DCM file");
            exit(1);
        }
    }

    msh->tex_coord = (GLfloat*)malloc(sizeof(GLfloat) * vertices * 2);

    for (i = 0; i < vertices * 2; i++)
    {
        if (fscanf(f, "%f\n", &msh->tex_coord[i]) != 1)
        {
            //DBG_ERROR("error reading DCM file");
            exit(1);
        }
    }

    /* As we don't flip our images we flip our u coordinates instead. */
    for (i = 1; i < vertices * 2; i += 2)
        msh->tex_coord[i] = 1.0f - msh->tex_coord[i];

    if (fscanf(f, "%d\n", &msh->groups) != 1)
    {
        //DBG_ERROR("error reading DCM file");
        exit(1);
    }

    msh->group = (group_t*)malloc(sizeof(group_t) * msh->groups);

    for (i = 0; i < msh->groups; i++)
    {
        char line[11];
        int group_len;
        int j;

        fgets(line, 11, f);

        if (!strcmp(line, "STRIP\n"))
            msh->group[i].type = PRIM_STRIP;
        else if (!strcmp(line, "TRIANGLES\n"))
            msh->group[i].type = PRIM_TRIANGLES;
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

        msh->group[i].len = group_len;

        msh->group[i].data = (GLuint*)malloc(sizeof(GLuint) * group_len);

        for (j = 0; j < group_len; j++)
        {
            if (fscanf(f, "%u\n", &msh->group[i].data[j]) != 1)
            {
                //DBG_ERROR("error reading DCM file");
                exit(1);
            }
        }
    }

    fclose(f);
    
    return msh;
}

void dcm::render( texture *tx )
{
    float specReflection[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    float nospecReflection[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float mcolor[4] = { 1.0f, 1.0f, 1.0f };

    glPushMatrix();

        glEnable(GL_LIGHTING);

        mcolor[3] = alpha;
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mcolor);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL);

        if (specular)
            glMaterialfv(GL_FRONT, GL_SPECULAR, specReflection);
        else
            glMaterialfv(GL_FRONT, GL_SPECULAR, nospecReflection);
        glMateriali(GL_FRONT, GL_SHININESS, 128);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, tx->texture_data.id);
        glCallList(mesh_data->list);
        glDisable(GL_TEXTURE_2D);

        glDisable(GL_LIGHTING);

    glPopMatrix();           
}

dcm::dcm( std::string filename )
{
    mesh_data=dcm_load((char*)filename.c_str());
    mesh_make_list(mesh_data);
    alpha=1.0f;
    specular=1;
}
