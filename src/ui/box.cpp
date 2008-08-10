
#include <GL/gl.h>
#include <GL/glu.h>
#include "box.h"
#include "scene.h"
#include "resource.h"

box::box( float xsize, float ysize, std::string tex, scene *parent )
{
    xscale=xsize;
    yscale=ysize;    

    tx = tex;
    parent_scene=parent;
}

void box::render()
{
    float tc = 46 / 512.0f;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, ((texture*)((resource*)((scene*)parent_scene)->get_resource(tx,"TEXTURE"))->data)->texture_data.id);

    glBegin(GL_QUADS);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glTexCoord2f(tc, 1 - tc);
    glNormal3f(0, 0, 1);
    glVertex3f(-4, -4, 0);
    glTexCoord2f(1 - tc, 1 - tc);
    glNormal3f(0, 0, 1);
    glVertex3f(4, -4, 0);
    glTexCoord2f(1 - tc, tc);
    glNormal3f(0, 0, 1);
    glVertex3f(4, 4, 0);
    glTexCoord2f(tc, tc);
    glNormal3f(0, 0, 1);
    glVertex3f(-4, 4, 0);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void box::update()
{

}
