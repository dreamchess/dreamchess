/*  DreamChess
**
**  DreamChess is the legal property of its developers, whose names are too
**  numerous to list here. Please refer to the AUTHORS.txt file distributed
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

#include "debug.h"
#include <stdio.h>
#include <stdlib.h>

/* Define our booleans */
#define TRUE 1
#define FALSE 0

#include "audio.h"
#include "board.h"
#include "theme.h"
#include "ui_sdlgl.h"
#include "ui_sdlgl_3d.h"

typedef struct coord3d {
	GLdouble x;
	GLdouble y;
	GLdouble z;
} coord3d_t;

typedef enum primitive_type { PRIM_TRIANGLES, PRIM_STRIP } primitive_type_t;

typedef struct group {
	primitive_type_t type;
	int len;
	GLuint *data;
} group_t;

typedef struct bone {
	char *name;
	float offset[3];
	int children;
	int *child;
} bone_t;

typedef struct mesh {
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
} mesh_t;

typedef struct model {
	mesh_t *mesh;
	texture_t *texture;
} model_t;

typedef struct data_col_item {
	char *name;
	void *data;
} data_col_item_t;

typedef struct {
	data_col_item_t *item;
	int size;
} data_col_t;

static void data_col_init(data_col_t *data_col) {
	data_col->item = NULL;
	data_col->size = 0;
}

static void data_col_free(data_col_t *data_col, void (*free_data)(void *)) {
	int i;
	for (i = 0; i < data_col->size; i++) {
		free(data_col->item[i].name);
		free_data(data_col->item[i].data);
	}

	free(data_col->item);
}

static void *data_col_find(data_col_t *data_col, char *name) {
	int i = 0;

	for (i = 0; i < data_col->size; i++)
		if (!strcmp(data_col->item[i].name, name))
			return data_col->item[i].data;

	return NULL;
}

static void data_col_add(data_col_t *data_col, char *name, void *data) {
	data_col->item = realloc(data_col->item, (data_col->size + 1) * sizeof(data_col_item_t));

	data_col->item[data_col->size].name = strdup(name);
	data_col->item[data_col->size++].data = data;
}

static data_col_t textures;
static data_col_t meshes;

#define SEL_BOUNCE_AMP 0.25f
#define SEL_HEIGHT 0.003f
static theme_selector_t sel;
static texture_t sel_tex;
static GLuint fb, colourpicking_tex, depth_rb;

#define BUF_SIZE 256
#define FN_LEN 256

static inline float in_product_self(float x, float y, float z) {
	return x * x + y * y + z * z;
}

static inline float in_product(float x, float y, float z, float xx, float yy, float zz) {
	return x * xx + y * yy + z * zz;
}

static model_t model[13];
static model_t board;
static model_t lettering;

static int is_2d;

#define SELECTOR_SHOW_TICKS 5000
static int selector, selected;
static float x_rotation, z_rotation;
static int selector_hide_time;

static int piece_moving_done = 1;
static int piece_moving_start;
static int piece_moving_dest;
static int piece_moving_source;
static int piece_moving_x_done;
static int piece_moving_y_done;
static float piece_moving_source_xpos;
static float piece_moving_source_ypos;
static float piece_moving_dest_xpos;
static float piece_moving_dest_ypos;
static float piece_moving_xpos;
static float piece_moving_ypos;

void init_fbo(void) {
	const int width = 1920;
	const int height = 1080;

	glGenTextures(1, &colourpicking_tex);
	glBindTexture(GL_TEXTURE_2D, colourpicking_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);

	glGenFramebuffers(1, &fb);
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colourpicking_tex, 0);

	glGenRenderbuffers(1, &depth_rb);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_rb);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rb);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		DBG_ERROR("Failed to set up FBO");
		exit(1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void deinit_fbo(void) {
	glDeleteTextures(1, &colourpicking_tex);
	glDeleteRenderbuffers(1, &depth_rb);
	glDeleteFramebuffers(1, &fb);
}

static void setup_view(void) {
	glLoadIdentity();

	/* Transition */
	glTranslatef(0, 0, -get_ui_trans_pos() / 20);

	glTranslatef(0, -0.5f, -12.0f);
	glRotatef(x_rotation, 1, 0, 0);
	glRotatef(z_rotation, 0, 0, 1);
}

int get_piece_moving_done(void) {
	return piece_moving_done;
}

void start_piece_move(int source, int dest) {
	//audio_play_sound(AUDIO_MOVE); 
	piece_moving_start = SDL_GetTicks();

	piece_moving_done = 0;

	piece_moving_dest = dest;
	piece_moving_source = source;

	piece_moving_source_xpos = (float)(source % 8);
	piece_moving_source_ypos = (float)(source / 8);

	piece_moving_dest_xpos = (float)(dest % 8);
	piece_moving_dest_ypos = (float)(dest / 8);

	piece_moving_xpos = piece_moving_source_xpos;
	piece_moving_ypos = piece_moving_source_ypos;

	piece_moving_x_done = 0;
	piece_moving_y_done = 0;
}

static texture_t *load_piece_texture(char *filename) {
	texture_t *tex = data_col_find(&textures, filename);

	if (tex) {
		return tex;
	}

	tex = malloc(sizeof(texture_t));

	DBG_LOG("Loading texture: %s", filename);
	load_texture_png(tex, filename, 1, 0);
	data_col_add(&textures, filename, tex);
	return tex;
}

static mesh_t *dcm_load(char *filename) {
	FILE *f;
	mesh_t *mesh;
	int version;
	char id[3];
	int vertices;
	int i;

	f = fopen(filename, "r");

	if (!f) {
		DBG_ERROR("Couldn't open %s", filename);
		return NULL;
	}

	if ((fscanf(f, "%c%c%c %d\n", &id[0], &id[1], &id[2], &version) != 4) ||
		((id[0] != 'D') || (id[1] != 'C') || (id[2] != 'M'))) {
		DBG_ERROR("Invalid DCM file header");
		fclose(f);
		return NULL;
	}

	if (version != 100) {
		DBG_ERROR("DCM version %i not supported", version);
		fclose(f);
		return NULL;
	}

	if (fscanf(f, "%d\n", &vertices) != 1) {
		DBG_ERROR("Error reading DCM file");
		fclose(f);
		return NULL;
	}

	mesh = malloc(sizeof(mesh_t));

	mesh->has_bones = 0;
	mesh->vertices = vertices;
	mesh->vertex = malloc(sizeof(GLfloat) * vertices * 3);

	for (i = 0; i < vertices * 3; i++) {
		if (fscanf(f, "%f\n", &mesh->vertex[i]) != 1) {
			DBG_ERROR("Error reading DCM file");
			exit(1);
		}
	}

	mesh->normal = malloc(sizeof(GLfloat) * vertices * 3);

	for (i = 0; i < vertices * 3; i++) {
		if (fscanf(f, "%f\n", &mesh->normal[i]) != 1) {
			DBG_ERROR("Error reading DCM file");
			exit(1);
		}
	}

	mesh->tex_coord = malloc(sizeof(GLfloat) * vertices * 2);

	for (i = 0; i < vertices * 2; i++) {
		if (fscanf(f, "%f\n", &mesh->tex_coord[i]) != 1) {
			DBG_ERROR("Error reading DCM file");
			exit(1);
		}
	}

	/* As we don't flip our images we flip our u coordinates instead. */
	for (i = 1; i < vertices * 2; i += 2)
		mesh->tex_coord[i] = 1.0f - mesh->tex_coord[i];

	if (fscanf(f, "%d\n", &mesh->groups) != 1) {
		DBG_ERROR("Error reading DCM file");
		exit(1);
	}

	mesh->group = malloc(sizeof(group_t) * mesh->groups);

	for (i = 0; i < mesh->groups; i++) {
		char line[11];
		int group_len;
		int j;

		fgets(line, 11, f);

		if (!strcmp(line, "STRIP\n"))
			mesh->group[i].type = PRIM_STRIP;
		else if (!strcmp(line, "TRIANGLES\n"))
			mesh->group[i].type = PRIM_TRIANGLES;
		else {
			DBG_ERROR("Error reading DCM file");
			exit(1);
		}

		if (fscanf(f, "%d\n", &group_len) != 1) {
			DBG_ERROR("Error reading DCM file");
			exit(1);
		}

		mesh->group[i].len = group_len;

		mesh->group[i].data = malloc(sizeof(GLuint) * group_len);

		for (j = 0; j < group_len; j++) {
			if (fscanf(f, "%u\n", &mesh->group[i].data[j]) != 1) {
				DBG_ERROR("Error reading DCM file");
				exit(1);
			}
		}
	}

	fclose(f);

	return mesh;
}

#if 0
mesh_t *dcm_load_new(char *filename) {
	FILE *f;
	mesh_t *mesh;
	int version;
	char id[3];
	int vertices;
	int bones;
	int i;

	f = fopen(filename, "r");

	if (!f) {
		DBG_ERROR("Couldn't open %s", filename);
		return NULL;
	}

	if ((fscanf(f, "%c%c%c %d\n", &id[0], &id[1], &id[2], &version) != 4) ||
		((id[0] != 'D') || (id[1] != 'C') || (id[2] != 'M'))) {
		DBG_ERROR("Invalid DCM file header");
		return NULL;
	}

	if (version != 1) {
		DBG_ERROR("DCM version %i not supported", version);
		return NULL;
	}

	if (fscanf(f, "VERTICES %d\n", &vertices) != 1) {
		DBG_ERROR("Error reading DCM file");
		return NULL;
	}

	mesh = malloc(sizeof(mesh_t));
	mesh->has_bones = 1;

	mesh->vertex = malloc(sizeof(float) * vertices * 3);
	mesh->normal = malloc(sizeof(float) * vertices * 3);

	for (i = 0; i < vertices; i++) {
		if (fscanf(f, "%f %f %f %f %f %f\n", &mesh->vertex[i * 3], &mesh->vertex[i * 3 + 1], &mesh->vertex[i * 3 + 2],
				   &mesh->normal[i * 3], &mesh->normal[i * 3 + 1], &mesh->normal[i * 3 + 2]) != 6) {
			DBG_ERROR("Error reading DCM file: %i of %i", i, vertices);
			exit(1);
		}
	}

	mesh->bone_w = malloc(sizeof(int) * vertices);
	memset(mesh->bone_w, -1, sizeof(int) * vertices);

	mesh->tex_coord = malloc(sizeof(float) * vertices * 2);

	for (i = 0; i < vertices * 2; i++)
		mesh->tex_coord[i] = 0;

	/* As we don't flip our images we flip our u coordinates instead. */
	for (i = 1; i < vertices * 2; i += 2)
		mesh->tex_coord[i] = 1.0f - mesh->tex_coord[i];

	mesh->groups = 1;
	mesh->group = malloc(sizeof(group_t) * mesh->groups);

	for (i = 0; i < mesh->groups; i++) {
		int group_len;
		float fl;
		int j;

		mesh->group[i].type = PRIM_TRIANGLES;

		if (fscanf(f, "TRIANGLES %d\n", &group_len) != 1) {
			DBG_ERROR("Error reading DCM file");
			exit(1);
		}

		mesh->group[i].len = group_len * 3;

		mesh->group[i].data = malloc(sizeof(unsigned int) * group_len * 3);

		for (j = 0; j < group_len; j++) {
			if (fscanf(f, "%u %u %u %f %f %f %f %f %f\n", &mesh->group[i].data[j * 3], &mesh->group[i].data[j * 3 + 1],
					   &mesh->group[i].data[j * 3 + 2], &fl, &fl, &fl, &fl, &fl, &fl) != 9) {
				DBG_ERROR("Error reading DCM file");
				exit(1);
			}
		}
	}

	if (fscanf(f, "BONES %d\n", &bones) != 1) {
		DBG_ERROR("Error reading DCM file");
		return NULL;
	}

	mesh->bone = malloc(sizeof(bone_t) * bones);

	for (i = 0; i < bones; i++) {
		int j;
		int vw;

		mesh->bone[i].name = malloc(256);
		if (fscanf(f, "BONE %255s\n", mesh->bone[i].name) != 1) {
			DBG_ERROR("Error reading DCM file");
			exit(1);
		}

		if (fscanf(f, "%f %f %f\n", &mesh->bone[i].offset[0], &mesh->bone[i].offset[1], &mesh->bone[i].offset[2]) !=
			3) {
			DBG_ERROR("Error reading DCM file");
			exit(1);
		}

		if (fscanf(f, "CHILDREN %d\n", &mesh->bone[i].children) != 1) {
			DBG_ERROR("Error reading DCM file");
			exit(1);
		}

		if (mesh->bone[i].children)
			mesh->bone[i].child = malloc(sizeof(int) * mesh->bone[i].children);

		for (j = 0; j < mesh->bone[i].children; j++) {
			if (fscanf(f, "%d\n", &mesh->bone[i].child[j]) != 1) {
				DBG_ERROR("Error reading DCM file");
				exit(1);
			}
		}

		if (fscanf(f, "VERTEXWEIGHTS %d\n", &vw) != 1) {
			DBG_ERROR("Error reading DCM file");
			exit(1);
		}

		for (j = 0; j < vw; j++) {
			float weight;
			int vert_idx;
			if (fscanf(f, "%i %f\n", &vert_idx, &weight) != 2) {
				DBG_ERROR("Error reading DCM file");
				exit(1);
			}
			mesh->bone_w[vert_idx] = i;
		}
	}

	fclose(f);

	return mesh;
}
#endif

static mesh_t *load_mesh(char *filename) {
	mesh_t *mesh = data_col_find(&meshes, filename);

	if (mesh) {
		return mesh;
	}

	DBG_LOG("Loading mesh: %s", filename);
	mesh = dcm_load(filename);
	data_col_add(&meshes, filename, mesh);
	return mesh;
}

#if 0
static mesh_t *load_mesh_new(char *filename) {
	mesh_t *mesh = data_col_find(&meshes, filename);

	if (mesh) {
		return mesh;
	}

	DBG_LOG("Loading mesh: %s", filename);
	mesh = dcm_load_new(filename);
	data_col_add(&meshes, filename, mesh);
	return mesh;
}
#endif

static void model_render(model_t *model, int specular, float alpha) {
	float specReflection[] = {1.0f, 1.0f, 1.0f, 1.0f};
	float nospecReflection[] = {0.0f, 0.0f, 0.0f, 1.0f};
	float mcolor[4] = {1.0f, 1.0f, 1.0f};
	texture_t *texture = model->texture;

	mcolor[3] = alpha;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mcolor);

	if (specular)
		glMaterialfv(GL_FRONT, GL_SPECULAR, specReflection);
	else
		glMaterialfv(GL_FRONT, GL_SPECULAR, nospecReflection);
	glMateriali(GL_FRONT, GL_SHININESS, 128);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture->id);

	glCallList(model->mesh->list);

	glDisable(GL_TEXTURE_2D);
}

static void model_render_cp(model_t *model, GLubyte colour) {
	glColor3ub(colour, 0.0f, 0.0f);
	glCallList(model->mesh->list);
}

static void model_make_list(model_t *model) {
	int g;
	mesh_t *mesh = model->mesh;

	mesh->list = glGenLists(1);
	glNewList(mesh->list, GL_COMPILE);

	for (g = 0; g < mesh->groups; g++) {
		int i;

		switch (mesh->group[g].type) {
		case PRIM_TRIANGLES:
			glBegin(GL_TRIANGLES);
			break;
		case PRIM_STRIP:
			glBegin(GL_TRIANGLE_STRIP);
		}

		for (i = 0; i < mesh->group[g].len; i++) {
			unsigned int *data = mesh->group[g].data;

			glTexCoord2fv(mesh->tex_coord + data[i] * 2);
			glNormal3fv(mesh->normal + data[i] * 3);
			glVertex3fv(mesh->vertex + data[i] * 3);
		}

		glEnd();
	}

	glEndList();
}

void set_theme(struct theme_struct *theme, texture_t texture) {
	sel = theme->selector;
	sel_tex = texture;
}

void loadmodels(char *filename) {
	int i;
	FILE *f;
	char mesh[256];
	char texture[256];

	data_col_init(&meshes);
	data_col_init(&textures);

	f = fopen(filename, "r");

	if (!f) {
		DBG_ERROR("Couldn't open %s\n", filename);
		exit(-1);
	}

	for (i = 0; i < 12; i++) {
		if (!fgets(mesh, 256, f) || (mesh[strlen(mesh) - 1] != '\n') || !fgets(texture, 256, f) ||
			(texture[strlen(texture) - 1] != '\n')) {
			DBG_ERROR("Error reading set file\n");
			exit(1);
		}

		mesh[strlen(mesh) - 1] = '\0';
		texture[strlen(texture) - 1] = '\0';

		model[i].mesh = load_mesh(mesh);
		model[i].texture = load_piece_texture(texture);
		model_make_list(model + i);
	}
	/*
		model[12].mesh = load_mesh_new("/home/walter/devel/ginger/ginger.dcm");
		model[12].texture = load_piece_texture(texture);
		model[11].mesh = model[12].mesh;
	*/
	if ((model[0].mesh->groups) == 1 && (model[0].mesh->group[0].len == 4))
		is_2d = 1;
	else
		is_2d = 0;

	fclose(f);
}

void load_board(char *dcm_name, char *texture_name) {
	board.mesh = load_mesh(dcm_name);
	board.texture = load_piece_texture(texture_name);
	lettering.mesh = load_mesh(dcm_name);
	lettering.texture = load_piece_texture("lettering.png");
	model_make_list(&board);
	model_make_list(&lettering);
}

static void free_mesh(void *data) {
	mesh_t *mesh = data;
	int i;

	free(mesh->vertex);
	free(mesh->normal);
	free(mesh->tex_coord);

	for (i = 0; i < mesh->groups; i++)
		free(mesh->group[i].data);

	free(mesh->group);
	free(mesh);
}

static void free_texture(void *data) {
	texture_t *tex = data;

	glDeleteTextures(1, &tex->id);
	free(tex);
}

void freemodels(void) {
	data_col_free(&meshes, free_mesh);
	data_col_free(&textures, free_texture);
}

/* How many squares per second? */
#define PIECE_MOVE_SPEED 3.0

/* Draw a selected piece after the rest.. */
static int selected_piece_render;
static int selected_piece_model;
static int selected_piece_grab;

/* Draw any moving piece.. */
static int moving_piece_render;
static int moving_piece_model;
static int moving_piece_grab;

static inline unsigned char square_to_colour(unsigned char square) {
	/* Add one to square to leave room for empty space */
	return square + 1;
}

static inline unsigned char colour_to_square(unsigned char colour) {
	return colour - 1;
}

static void draw_pieces(board_t *board, int flip) {
	int i, j, k;
	float moved = 0;

	moved = (float)((SDL_GetTicks() - piece_moving_start) / (1000 / PIECE_MOVE_SPEED));
	selected_piece_grab = FALSE;
	moving_piece_grab = FALSE;
	selected_piece_render = FALSE;
	moving_piece_render = FALSE;

	/* Draw the pieces.. */
	for (i = 7; i >= 0; i--)
		for (j = 0; j < 8; j++) {
			if ((k = board->square[(0 /*flip_board*/ ? 63 - (i * 8 + j) : i * 8 + j)]) != NONE) {
				setup_view();

				if (!is_2d && flip)
					glScalef(1.0f, 1.0f, -1.0f);

				if ((i * 8 + j) == piece_moving_dest) {
					if (piece_moving_dest_xpos > piece_moving_source_xpos) {
						piece_moving_xpos = piece_moving_source_xpos + moved;
						if (piece_moving_xpos >= piece_moving_dest_xpos)
							piece_moving_xpos = piece_moving_dest_xpos;
					} else if (piece_moving_dest_xpos < piece_moving_source_xpos) {
						piece_moving_xpos = piece_moving_source_xpos - moved;
						if (piece_moving_xpos <= piece_moving_dest_xpos)
							piece_moving_xpos = piece_moving_dest_xpos;
					}

					if (piece_moving_dest_ypos > piece_moving_source_ypos) {
						piece_moving_ypos = piece_moving_source_ypos + moved;
						if (piece_moving_ypos >= piece_moving_dest_ypos)
							piece_moving_ypos = piece_moving_dest_ypos;
					} else if (piece_moving_dest_ypos < piece_moving_source_ypos) {
						piece_moving_ypos = piece_moving_source_ypos - moved;
						if (piece_moving_ypos <= piece_moving_dest_ypos)
							piece_moving_ypos = piece_moving_dest_ypos;
					}

					if (piece_moving_xpos == piece_moving_dest_xpos && piece_moving_ypos == piece_moving_dest_ypos) {
						piece_moving_done = 1;
					}
					if (is_2d)
						glTranslatef(-3.5f + piece_moving_xpos, -3.5f + piece_moving_ypos, 0.002);
					else
						glTranslatef(-3.5f + piece_moving_xpos, -3.5f + piece_moving_ypos, 0.001);

				} else
					glTranslatef(-3.5f + j, -3.5f + i, 0.001);

				if (is_2d) {
					int steps = (z_rotation + 45.0f) / 90.0f;

					glRotatef(-steps * 90.0f, 0, 0, 1);
				}
				/*                else if (k < 12 && IS_BLACK(k))
								{
									glRotatef(180, 0, 0, 1);
								}*/

				if (i * 8 + j == selected) {
					selected_piece_render = TRUE;
					selected_piece_model = k;
					selected_piece_grab = TRUE;
					glPushMatrix();
				} else
					selected_piece_grab = FALSE;

				if (piece_moving_done == FALSE && piece_moving_dest == (i * 8 + j)) {
					moving_piece_render = TRUE;
					moving_piece_model = k;
					moving_piece_grab = TRUE;
					glPushMatrix();
				} else
					moving_piece_grab = FALSE;

				if (!selected_piece_grab && !moving_piece_grab)
					model_render(&model[k], !is_2d, (i * 8 + j == selected ? 0.5f : 1.0f));
			}
		}

	/* Do we need to draw a selected piece? */
	if (selected_piece_render) {
		glPopMatrix();
		model_render(&model[selected_piece_model], !is_2d, 0.5f);
	}

	if (moving_piece_render) {
		glPopMatrix();
		model_render(&model[moving_piece_model], !is_2d, 1.0f);
	}
}

static void draw_pieces_cp(board_t *board) {
	int i, j, k;

	/* Draw the pieces.. */
	for (i = 7; i >= 0; i--)
		for (j = 0; j < 8; j++) {
			if ((k = board->square[i * 8 + j]) != NONE) {
				setup_view();
				glTranslatef(-3.5f + j, -3.5f + i, 0.001);
				model_render_cp(&model[k], square_to_colour(i * 8 + j));
			}
		}
}

static void draw_board(int blend) {
	setup_view();
	model_render(&board, 0, 1.0f);
}

static void draw_lettering(int blend) {
	setup_view();
	model_render(&lettering, 0, 1.0f);
}

static void draw_selector(float alpha) {
	float bounce_offset = 0.0;
	float spin_offset = 0.0;
	Uint32 ticks = SDL_GetTicks();

	if (sel.bouncespeed) {
		float phase = ((ticks % (int)(1000 / sel.bouncespeed)) / (float)(1000 / sel.bouncespeed));

		if (phase < 0.5)
			bounce_offset = phase * 2 * SEL_BOUNCE_AMP;
		else
			bounce_offset = (1.0 - phase) * 2 * SEL_BOUNCE_AMP;
	}

	if (sel.spinspeed) {
		float phase = ((ticks % (int)(1000 / sel.spinspeed)) / (float)(1000 / sel.spinspeed));

		spin_offset = phase * 360;
	}

	setup_view();

	glTranslatef(-3.5 + selector % 8, -3.5 + selector / 8, 0);
	glRotatef(spin_offset, 0, 0, 1);

	glColor4f(0.0f, 0.0f, 0.0f, 1.0f * alpha);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, sel_tex.id);

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-sel.size / 2, sel.size / 2, SEL_HEIGHT);
	glTexCoord2f(1, 0);
	glVertex3f(sel.size / 2, sel.size / 2, SEL_HEIGHT);
	glTexCoord2f(1, 1);
	glVertex3f(sel.size / 2, -sel.size / 2, SEL_HEIGHT);
	glTexCoord2f(0, 1);
	glVertex3f(-sel.size / 2, -sel.size / 2, SEL_HEIGHT);
	glEnd();

	glTranslatef(0, 0, bounce_offset);

	glColor4f(sel.colour[0], sel.colour[1], sel.colour[2], sel.colour[3] * alpha);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-sel.size / 2, sel.size / 2, SEL_HEIGHT);
	glTexCoord2f(1, 0);
	glVertex3f(sel.size / 2, sel.size / 2, SEL_HEIGHT);
	glTexCoord2f(1, 1);
	glVertex3f(sel.size / 2, -sel.size / 2, SEL_HEIGHT);
	glTexCoord2f(0, 1);
	glVertex3f(-sel.size / 2, -sel.size / 2, SEL_HEIGHT);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}

int find_square(int x, int y) {
	GLubyte col;
	GLint viewport[4];

	setup_view();
	glGetIntegerv(GL_VIEWPORT, viewport);

	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	glReadPixels(x, viewport[3] - y, 1, 1, GL_RED, GL_UNSIGNED_BYTE, &col);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return col - 1;
}

static void draw_board_center(float r, float g, float b, float a) {
	float tc = 46 / 512.0f;
	float mcolor[4] = {1.0f, 1.0f, 1.0f};

	mcolor[3] = a;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mcolor);

	setup_view();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, board.texture->id);

	glBegin(GL_QUADS);
	glColor4f(r, g, b, a);
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

static void draw_board_center_cp() {
	setup_view();
	int row, col;

	for (row = 0; row < 8; ++row) {
		for (col = 0; col < 8; ++col) {
			glBegin(GL_QUADS);
			glColor3ub(square_to_colour(row * 8 + col), 0, 0);
			glVertex3f(-4.0f + col, -4.0f + row, 0.0f);
			glVertex3f(-3.0f + col, -4.0f + row, 0.0f);
			glVertex3f(-3.0f + col, -3.0f + row, 0.0f);
			glVertex3f(-4.0f + col, -3.0f + row, 0.0f);
			glEnd();
		}
	}
}

static void setup_stencil(void) {
	float tc = 46 / 512.0f;

	glDisable(GL_LIGHTING);
	glClearStencil(0);
	glClear(GL_STENCIL_BUFFER_BIT);
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	glStencilFunc(GL_ALWAYS, 1, 1);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
	glEnable(GL_STENCIL_TEST);

	setup_view();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, board.texture->id);

	glBegin(GL_QUADS);
	glColor4f(0, 0, 0, 1);
	glTexCoord2f(tc, tc);
	glVertex3f(-4, -4, 0);
	glTexCoord2f(1 - tc, tc);
	glVertex3f(4, -4, 0);
	glTexCoord2f(1 - tc, 1 - tc);
	glVertex3f(4, 4, 0);
	glTexCoord2f(tc, 1 - tc);
	glVertex3f(-4, 4, 0);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glStencilFunc(GL_EQUAL, 1, 1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glDepthMask(GL_TRUE);
	glEnable(GL_LIGHTING);
}

void render_scene_3d(board_t *board, GLuint target_fb, int reflections) {
	int ticks = SDL_GetTicks();

	//

	glBindFramebuffer(GL_FRAMEBUFFER, target_fb);
	glEnable(GL_LIGHTING);

	glEnable(GL_CULL_FACE);
	if (!is_2d && reflections) {

		setup_stencil();
		glCullFace(GL_FRONT);
		draw_pieces(board, 1);
		glDisable(GL_STENCIL_TEST);
		glCullFace(GL_BACK);
		draw_board_center(1.0f, 1.0f, 1.0f, 0.8f);
		draw_board(0);
		if(get_is_lettering())
			draw_lettering(0);
		draw_pieces(board, 0);

	} else {
		glPushMatrix();
		glTranslatef(5.0f, 5.0, 5.0f);
		draw_board_center(0.75f, 0.75f, 0.75f, 1.0f);
		draw_board(0);
		if(get_is_lettering())
			draw_lettering(0);
		draw_pieces(board, 0);
		glPopMatrix();
	}
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);

	if (ticks <= selector_hide_time)
		draw_selector(1.0f);
	else if (ticks < selector_hide_time + 1000)
		draw_selector(1.0f - ((ticks - selector_hide_time) / 1000.0f));

	/* Render FBO for colour picking */
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	draw_board_center_cp();
	if (!is_2d)
		draw_pieces_cp(board);

	glBindFramebuffer(GL_FRAMEBUFFER, target_fb);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
}

void move_camera(float x, float z) {
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
}

void move_selector(int direction) {
	int steps = (z_rotation + 45.0f) / 90.0f;

	selector_hide_time = SDL_GetTicks() + SELECTOR_SHOW_TICKS;

	while (steps--) {
		switch (direction) {
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

	switch (direction) {
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

int get_selector(void) {
	return selector;
}

void select_piece(int square) {
	selected = square;
}

void reset_3d(int flip) {
	float mcolor[] = {1.0f, 1.0f, 1.0f};
	float position[] = {50.0f, 10.0f, 50.0f, 1.0f};

	selected = -1;
	selector = 0;
	selector_hide_time = 0;

	glLightfv(GL_LIGHT0, GL_POSITION, position);

	if (is_2d) {
		GLfloat ambientLight[] = {1.0f, 1.0f, 1.0f, 1.0f};

		x_rotation = 0.0f;

		/* Create light components */
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	} else {
		/* Create light components */
		GLfloat ambientLight[] = {0.15f, 0.15f, 0.15f, 1.0f};
		GLfloat diffuseLight[] = {0.45f, 0.45f, 0.45f, 1.0f};
		GLfloat specularLight[] = {1.0f, 1.0f, 1.0f, 1.0f};

		x_rotation = -30.0f;

		/* Assign created components to GL_LIGHT0 */
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
		glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	}

	if (flip)
		z_rotation = 180.0f;
	else
		z_rotation = 0.0f;

	/* White specular highlights */
	glLightModelf(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mcolor);

	glEnable(GL_LIGHT0);
}
