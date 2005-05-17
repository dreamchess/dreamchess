/*  DreamChess
 *  Copyright (C) 2005  The DreamChess project
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#ifdef WITH_UI_SDLGL

#include <stdio.h>
#include <stdlib.h>

#ifdef _arch_dreamcast
#include <dc/fmath.h>
#define sin fsin
#define cos fcos
#define sqrt fsqrt
#define magnitude_sqr(X, Y, Z) fipr_magnitude_sqr(X, Y, Z, 0)
#define dot_product(X, Y, Z, XX, YY, ZZ) fipr(X, Y, Z, 0, XX, YY, ZZ, 0)
#else
#include <math.h>
#define magnitude_sqr in_product_self
#define dot_product in_product
#endif

#include <SDL_opengl.h>

#include "board.h"
#include "ui_sdlgl_3d.h"

typedef struct coord3
{
    float x;
    float y;
    float z;
}
coord3_t;

typedef struct texture
{
    /** OpenGL Texture ID. */
    GLuint id;

    /** Lower-left u-coordinate. Ranges from 0.0f to 1.0f. */
    float u1;

    /** Lower-left v-coordinate. Ranges from 0.0f to 1.0f. */
    float v1;

    /** Upper-right u-coordinate. Ranges from 0.0f to 1.0f. */
    float u2;

    /** Upper-right v-coordinate. Ranges from 0.0f to 1.0f. */
    float v2;

    /** Width of texture in pixels. */
    int width;

    /** Height of texture in pixels. */
    int height;
}
texture_t;

typedef enum primitive_type
{
    PRIM_TRIANGLES,
    PRIM_STRIP
} primitive_type_t;

typedef struct group
{
    primitive_type_t type;
    int len;
    unsigned int *data;
}
group_t;

typedef struct mesh
{
    float *vertex;
    float *normal;
    float *tex_coord;
    int groups;
    group_t *group;
}
mesh_t;

typedef struct model
{
    mesh_t *mesh;
    texture_t texture;
}
model_t;

void load_texture_png( texture_t *texture, char *filename, int alpha );

typedef struct data_col_item
{
    char *name;
    void *data;
}
data_col_item_t;

typedef struct
{
    data_col_item_t *item;
    int size;
}
data_col_t;

static void data_col_init(data_col_t *data_col)
{
    data_col->item = NULL;
    data_col->size = 0;
}

static void data_col_free(data_col_t *data_col, void (* free_data) (void *))
{
    int i;
    for (i = 0; i < data_col->size; i++)
    {
        free(data_col->item[i].name);
        free_data(data_col->item[i].data);
    }
}

static void *data_col_find(data_col_t *data_col, char *name)
{
    int i = 0;

    for (i = 0; i < data_col->size; i++)
        if (!strcmp(data_col->item[i].name, name))
            return data_col->item[i].data;

    return NULL;
}

static void data_col_add(data_col_t *data_col, char *name, void *data)
{
    data_col->item = realloc(data_col->item, (data_col->size + 1)
                             * sizeof(data_col_item_t));

    data_col->item[data_col->size].name = strdup(name);
    data_col->item[data_col->size++].data = data;
}

static data_col_t textures;
static data_col_t meshes;

texture_t ground;

#define BUF_SIZE 256
#define FN_LEN 256

static inline float in_product_self(float x, float y, float z)
{
    return x * x + y * y + z * z;
}

static inline float in_product(float x, float y, float z, float xx, float yy,
                               float zz)
{
    return x * xx + y * yy + z * zz;
}

static model_t model[12];
static model_t board;

static int is_2d;

static int selector, selected;

static float x_rotation, z_rotation;

static texture_t load_piece_texture(char *filename)
{
    texture_t *tex = data_col_find(&textures, filename);

    if (tex)
    {
        printf("Already loaded %s\n", filename);
        return *tex;
    }

    tex = malloc(sizeof(texture_t));

    printf("Loading %s\n", filename);
    load_texture_png(tex, filename, 1);
    data_col_add(&textures, filename, tex);
    return *tex;
}

mesh_t *dcm_load(char *filename)
{
    FILE *f;
    mesh_t *mesh;
    int version;
    char id[3];
    int vertices;
    int i;

    f = fopen(filename, "r");

    if (!f)
    {
        fprintf(stderr, "Couldn't open %s\n", filename);
        return NULL;
    }

    if ((fscanf(f, "%c%c%c %d\n", &id[0], &id[1], &id[2], &version) != 4)
            || ((id[0] != 'D') || (id[1] != 'C') || (id[2] != 'M')))
    {
        fprintf(stderr, "Invalid DCM file header\n");
        return NULL;
    }

    if (version != 100)
    {
        fprintf(stderr, "DCM version %i not supported.\n", version);
        return NULL;
    }

    if (fscanf(f, "%d\n", &vertices) != 1)
    {
        fprintf(stderr, "Error reading DCM file\n");
        return NULL;
    }

    mesh = malloc(sizeof(mesh_t));

    mesh->vertex = malloc(sizeof(float) * vertices * 3);

    for (i = 0; i < vertices * 3; i++)
    {
        if (fscanf(f, "%f\n", &mesh->vertex[i]) != 1)
        {
            fprintf(stderr, "Error reading DCM file\n");
            exit(1);
        }
    }

    mesh->normal = malloc(sizeof(float) * vertices * 3);

    for (i = 0; i < vertices * 3; i++)
    {
        if (fscanf(f, "%f\n", &mesh->normal[i]) != 1)
        {
            fprintf(stderr, "Error reading DCM file\n");
            exit(1);
        }
    }

    mesh->tex_coord = malloc(sizeof(float) * vertices * 2);

    for (i = 0; i < vertices * 2; i++)
    {
        if (fscanf(f, "%f\n", &mesh->tex_coord[i]) != 1)
        {
            fprintf(stderr, "Error reading DCM file\n");
            exit(1);
        }
    }

    /* As we don't flip our images we flip our u coordinates instead. */
    for (i = 1; i < vertices * 2; i += 2)
        mesh->tex_coord[i] = 1.0f - mesh->tex_coord[i];

    if (fscanf(f, "%d\n", &mesh->groups) != 1)
    {
        fprintf(stderr, "Error reading DCM file\n");
        exit(1);
    }

    mesh->group = malloc(sizeof(group_t) * mesh->groups);

    for (i = 0; i < mesh->groups; i++)
    {
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
            fprintf(stderr, "Error reading DCM file\n");
            exit(1);
        }

        if (fscanf(f, "%d\n", &group_len) != 1)
        {
            fprintf(stderr, "Error reading DCM file\n");
            exit(1);
        }

        mesh->group[i].len = group_len;

        mesh->group[i].data = malloc(sizeof(unsigned int) * group_len);

        for (j = 0; j < group_len; j++)
        {
            if (fscanf(f, "%u\n", &mesh->group[i].data[j]) != 1)
            {
                fprintf(stderr, "Error reading DCM file\n");
                exit(1);
            }
        }
    }

    fclose(f);

    return mesh;
}

static mesh_t *load_mesh(char *filename)
{
    mesh_t *mesh = data_col_find(&meshes, filename);

    if (mesh)
    {
        printf("Already loaded %s\n", filename);
        return mesh;
    }

    printf("Loading %s\n", filename);
    mesh = dcm_load(filename);
    data_col_add(&meshes, filename, mesh);
    return mesh;
}

coord3_t light = {0.0f, 0.0f, -1.0f};

#define DC_PI 3.14159265358979323846

inline float arccos(float f)
{
    return (2.193376378 + (-2.987042783 + (.5314426631 + .2990387380 * f)
                           * f) * f) / (1.396346817 + (-1.012703522 + (-.3056194995
                                                       + .1383216735 * f) * f) * f);
}

void model_render(model_t *model, float alpha, coord3_t *light)
{
    mesh_t *mesh = model->mesh;
    int g;
    texture_t *texture = &model->texture;
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture->id);

    for (g = 0; g < mesh->groups; g++)
    {
        int i;

        switch (mesh->group[g].type)
        {
        case PRIM_TRIANGLES:
            glBegin(GL_TRIANGLES);
            break;
        case PRIM_STRIP:
            glBegin(GL_TRIANGLE_STRIP);
        }

        for (i = 0; i < mesh->group[g].len; i++)
        {
            unsigned int *data = mesh->group[g].data;
            float angle = 1.0f;

            if (light)
            {
                angle = arccos(dot_product(mesh->normal[data[i] * 3],
                               mesh->normal[data[i] * 3 + 1],
                               mesh->normal[data[i] * 3 + 2],
                               light->x, light->y, light->z));

                angle /= 2.8;

                if (angle < 0.5f)
                    angle = 0.25f;

                else
                {
                    angle -= 0.5f;
                    angle *= 1.5;
                    angle += 0.25f;
                }
            }

            glColor4f(angle, angle, angle, alpha);

            glTexCoord2f(mesh->tex_coord[data[i] * 2] * texture->u2,
                         mesh->tex_coord[data[i] * 2 + 1] * texture->v2);

            glVertex3f(mesh->vertex[data[i] * 3],
                       mesh->vertex[data[i] * 3 + 1],
                       mesh->vertex[data[i] * 3 + 2]);
        }

        glEnd();
    }

    glDisable(GL_TEXTURE_2D);
}

void loadmodels(char *filename)
{
    int i;
    FILE *f;
    char mesh[256];
    char texture[256];

    data_col_init(&meshes);
    data_col_init(&textures);

    f = fopen(filename, "r");

    if (!f)
    {
        fprintf(stderr, "Couldn't open %s\n", filename);
        exit(-1);
    }

    for (i = 0; i < 12; i++)
    {
        if (!fgets(mesh, 256, f) || (mesh[strlen(mesh) - 1] != '\n')
                || !fgets(texture, 256, f)
                || (texture[strlen(texture) - 1] != '\n'))
        {
            fprintf(stderr, "Error reading set file\n");
            exit(1);
        }

        mesh[strlen(mesh) - 1] = '\0';
        texture[strlen(texture) - 1] = '\0';

        model[i].mesh = load_mesh(mesh);
        model[i].texture = load_piece_texture(texture);
    }

    if ((model[0].mesh->groups) == 1 && (model[0].mesh->group[0].len == 4))
        is_2d = 1;
    else
        is_2d = 0;
}

void load_board(char *dcm_name, char *texture_name)
{
    board.mesh = dcm_load(dcm_name);
    load_texture_png(&board.texture, texture_name, 0);
}

void free_mesh(void *data)
{
    mesh_t *mesh = data;

    free(mesh->vertex);
#if 0

    free(mesh->face);
#endif

    free(mesh);
}

void free_texture(void *data)
{
    texture_t *tex = data;

    glDeleteTextures(1, &tex->id);
    free(tex);
}

void freemodels()
{
    data_col_free(&meshes, free_mesh);
    data_col_free(&textures, free_texture);
}

static void draw_pieces(board_t *board, float rot_x, float rot_z)
{
    int i,j,k;
    coord3_t light_inv;

    light_inv.x = -light.x;
    light_inv.y = -light.y;
    light_inv.z = light.z;

    /* Draw the pieces.. */
    for (i = 7; i >= 0; i--)
        for (j = 0; j < 8; j++)
        {
            if ((k = board->square[(0/*flip_board*/ ? 63 - (i*8+j) : i*8+j)]) != NONE)
            {
                coord3_t *l = &light;
                glLoadIdentity();
                glTranslatef(0, -0.5f, -12.0f );
                glRotatef(rot_x, 1, 0, 0);
                glRotatef(rot_z, 0, 0, 1);
                glTranslatef(-3.5f + j, -3.5f + i, 0.02);
                if (is_2d)
                {
                    int steps = (z_rotation + 45.0f) / 90.0f;

                    glRotatef(-steps * 90.0f, 0, 0, 1);
                }
                else if (k < 12 && IS_BLACK(k))
                {
                    glRotatef(180, 0, 0, 1);
                    l = &light_inv;
                }
                model_render(&model[k], (i * 8 + j == selected ? 0.5f : 1.0f), l);
            }
        }
}

static void draw_board(float rot_x, float rot_z)
{
    coord3_t fixed = {0, 0, -1};
    glLoadIdentity();
    glTranslatef(0, -0.5f, -12.0f );
    glRotatef(rot_x, 1, 0, 0);
    glRotatef(rot_z, 0, 0, 1);
    model_render(&board, 1.0f, &fixed);
}

void draw_selector()
{
    glLoadIdentity();
    glTranslatef(0, -0.5, -12.0);
    glRotatef(x_rotation, 1, 0, 0);
    glRotatef(z_rotation, 0, 0, 1);
    glTranslatef(-3.5 + selector % 8, -3.5 + selector / 8, 0.01f);
    glColor4f(1, 1, 0, 0.2);

    glBegin(GL_QUADS);
    glVertex3f(-0.5, -0.5, 0);
    glVertex3f(0.5, -0.5, 0);
    glVertex3f(0.5, 0.5, 0);
    glVertex3f(-0.5, 0.5, 0);
    glEnd();
}

void render_scene_3d(board_t *board)
{
    glEnable(GL_CULL_FACE);
    draw_board(x_rotation, z_rotation);
    draw_selector();
    draw_pieces(board, x_rotation, z_rotation);
    glDisable(GL_CULL_FACE);
}

static void update_light()
{
    float len;

    light.x = sin(z_rotation * DC_PI / 180.0f) * (1.0f - cos(x_rotation * DC_PI / 180.0f));
    light.y = -sin(x_rotation * DC_PI / 180.0f) * cos(z_rotation * DC_PI / 180.0f);
    light.z = -cos(x_rotation * DC_PI / 180.0f);

    len = sqrt(magnitude_sqr(light.x, light.y, light.z));

    light.x /= len;
    light.y /= len;
    light.z /= len;
}

void move_camera(float x, float z)
{
    x_rotation -= x;
    if (x_rotation > 0.0f)
        x_rotation = 0.0f;

    if (x_rotation < -90.0f)
        x_rotation = -90.0f;

    z_rotation -= z;
    if (z_rotation >= 360.0f)
        z_rotation -= 360.0f;

    if (z_rotation <= 0.0f)
        z_rotation = 360.0f;

    update_light();
}

void move_selector(int direction)
{
    int steps = (z_rotation + 45.0f) / 90.0f;

    while (steps--)
    {
        switch (direction)
        {
        case SELECTOR_UP:
            direction = SELECTOR_RIGHT;
            break;
        case SELECTOR_RIGHT:
            direction = SELECTOR_DOWN;
            break;
        case SELECTOR_DOWN:
            direction = SELECTOR_LEFT;
            break;
        case SELECTOR_LEFT:
            direction = SELECTOR_UP;
        }
    }

    switch(direction)
    {
    case SELECTOR_UP:
        if (selector < 56)
            selector += 8;
        break;
    case SELECTOR_RIGHT:
        if (selector % 8 < 7)
            selector++;
        break;
    case SELECTOR_DOWN:
        if (selector > 7)
            selector -= 8;
        break;
    case SELECTOR_LEFT:
        if (selector % 8 > 0)
            selector--;
    }
}

int get_selector()
{
    return selector;
}

void select_piece(int square)
{
    selected = square;
}

void reset_3d()
{
    selected = -1;
    selector = 0;
    if (is_2d)
    {
        x_rotation = 0.0f;
        z_rotation = 0.0f;
    }
    else
    {
        x_rotation = -45.0f;
        z_rotation = 0.0f;
    }
    update_light();
}

#endif /* WITH_UI_SDLGL */
