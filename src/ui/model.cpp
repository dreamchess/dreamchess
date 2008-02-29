
#include "model.h"

model::model(char *msh2, char *tx2)
{
    msh = new mesh(msh2);
    tx = new texture(tx2);
    alpha=1.0f;
    specular=0;
}

void model::render()
{
    float specReflection[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    float nospecReflection[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float mcolor[4] = { 1.0f, 1.0f, 1.0f };

    glPushMatrix();
        glScalef(xscale, yscale, zscale);
        glRotatef( xrot, 1, 0, 0 );
        glRotatef( yrot, 1, 1, 0 );
        glRotatef( zrot, 0, 0, 1 );

        glColor4f(1.0f,1.0f,1.0f,1.0f);

        mcolor[3] = alpha;
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mcolor);

        if (specular)
            glMaterialfv(GL_FRONT, GL_SPECULAR, specReflection);
        else
            glMaterialfv(GL_FRONT, GL_SPECULAR, nospecReflection);
        glMateriali(GL_FRONT, GL_SHININESS, 128);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, tx->texture_data.id);
        glCallList(msh->mesh_data->list);
        glDisable(GL_TEXTURE_2D); 
    glPopMatrix();              
}

void model::update()
{
    xrot+=0.1f;
}
