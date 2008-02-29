
#include <GL/gl.h>
#include <GL/glu.h>
#include "SDL/SDL.h"
#include <string>

typedef enum primitive_type
{
    PRIM_TRIANGLES,
    PRIM_STRIP
} primitive_type_t;

typedef struct group
{
    primitive_type_t type;
    int len;
    GLuint *data;
}
group_t;

typedef struct bone
{
    char *name;
    float offset[3];
    int children;
    int *child;
}
bone_t;

typedef struct
{
    int has_bones;
    int vertices;
    GLfloat *vertex;
    GLfloat *normal;
    GLfloat *tex_coord;
    int *bone_w;
    int groups;
    group_t *group;
    bone_t *bone;
    GLuint list;
}mesh_t;

class mesh
{
    public:
        mesh(char *filename);
        std::string name;
        mesh_t *mesh_data;
};
