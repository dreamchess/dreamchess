
#include "model.h"
#include "texture.h"
#include "mesh.h"
#include "scene.h"

// Doesn't really work.. yet...
void get_size( float *xs, float *ys, float *zs, std::string msh )
{
    *xs=1.0f;
    *ys=1.0f;
    *zs=1.0f;
}

void model::generate_bbox()
{
	
}

model::model(std::string name2, std::string msh2, std::string tx2, scene *parent)
{
    msh = msh2;
    tx = tx2;
    name = name2;
    alpha=1.0f;
    specular=1;
    parent_scene=parent;

    get_size( &xsize, &ysize, &zsize, msh );

    //printf( "Size: %f, %f, %f\n", xsize, ysize, zsize );
}

void model::render()
{
    float specReflection[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    float nospecReflection[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float mcolor[4] = { 1.0f, 1.0f, 1.0f };

    glPushMatrix();

        glEnable(GL_LIGHTING);

        glTranslatef( xpos, ypos, zpos );
        glScalef(xscale, yscale, zscale);
        glRotatef( xrot, 1, 0, 0 );
        glRotatef( yrot, 0, 1, 0 );
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
        glBindTexture(GL_TEXTURE_2D, ((texture*)((resource*)((scene*)parent_scene)->get_resource(tx,"TEXTURE")))->texture_data.id);
        glCallList( ((mesh*)((resource*)((scene*)parent_scene)->get_resource(msh,"MESH")))->mesh_data->list);
        glDisable(GL_TEXTURE_2D);

        glDisable(GL_LIGHTING);
 
    glPopMatrix();           
}
