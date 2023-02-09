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

#include "ui_sdlgl.h"
#include "opengl.h"
#include "cglm/cglm.h"

static GLuint program_gui;
static GLuint background_vbo, background_vao;
static texture_t background_tex;
static unsigned int screen_width, screen_height;

static const char *load_file(const char *filename) {
	FILE *f = fopen(filename, "rb");

	if (!f) {
		DBG_ERROR("Failed to open '%s'", filename);
		exit(1);
	}

	fseek(f, 0, SEEK_END);
	long size = ftell(f);

	if (size <= 0) {
		DBG_ERROR("Failed to get file size for '%s'", filename);
		exit(1);
	}

	fseek(f, 0, SEEK_SET);
	char *s = malloc(size + 1);

	size_t read_bytes = 0;
	if (s)
		read_bytes = fread(s, 1, size, f);

	if (read_bytes < size) {
		DBG_ERROR("Failed to read '%s'", filename);
		exit(1);
	}

	s[size] = 0;

	return s;
}

static GLuint load_shader(const char *filename, GLenum shader_type) {
	const char *shader_src = load_file(filename);
	GLuint shader = glCreateShader(shader_type);
	glShaderSource(shader, 1, &shader_src, NULL);
	glCompileShader(shader);

	GLsizei loglen;
	GLchar *log = malloc(1024);
	glGetShaderInfoLog(shader, 1024, &loglen, log);
 	DBG_LOG("Program log: %s", log);

	return shader;
}

void gl_init(void) {
	GLuint vertex_shader = load_shader("shaders/vertex.glsl", GL_VERTEX_SHADER);
	GLuint fragment_shader = load_shader("shaders/fragment.glsl", GL_FRAGMENT_SHADER);
	program_gui = glCreateProgram();
	glAttachShader(program_gui, vertex_shader);
	glAttachShader(program_gui, fragment_shader);
	glLinkProgram(program_gui);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	// Create VBO and VAO for background image
    glGenBuffers(1, &background_vbo);
	glGenVertexArrays(1, &background_vao);
}

void gl_set_gui(unsigned int width, unsigned int height) {
	GLenum glerr;
	while((glerr = glGetError()) != GL_NO_ERROR) {
		DBG_LOG("OpenGL error %d: %s", glerr, gluErrorString(glerr));
	}
	screen_width = width;
	screen_height = height;

	glUseProgram(program_gui);
	mat4s m = glms_ortho(0.0f, 480 * width / (float)height, 0.0f, 480.0f, -1.0f, 1.0f, m);
	GLint loc = glGetUniformLocation(program_gui, "projection");
	glUniformMatrix4fv(loc, 1, GL_FALSE, m.raw[0]);
	while((glerr = glGetError()) != GL_NO_ERROR) {
		DBG_LOG("OpenGL error %d: %s", glerr, gluErrorString(glerr));
	}
}

void gl_2d_init(gl_2d_obj *obj) {
	GLenum glerr;
	while((glerr = glGetError()) != GL_NO_ERROR) {
		DBG_LOG("OpenGL error %d: %s", glerr, gluErrorString(glerr));
	}
	obj->tex = 0;
	obj->count = 0;
	glGenBuffers(1, &obj->vbo);
	glGenBuffers(1, &obj->ebo);
	glGenVertexArrays(1, &obj->vao);
	obj->model = glms_mat4_identity();
	obj->colour = glms_vec4_one();
	obj->scale = glms_vec2_one();
	obj->pos = glms_vec2_zero();
}

void gl_2d_release(gl_2d_obj *obj) {
	glDeleteBuffers(1, &obj->vbo);
	glDeleteBuffers(1, &obj->ebo);
	glDeleteVertexArrays(1, &obj->vao);
}

void gl_2d_set_texture(gl_2d_obj *obj, GLuint tex) {
	obj->tex = tex;
}

void gl_2d_delete_texture(gl_2d_obj *obj) {
	glDeleteTextures(1, &obj->tex);
	obj->tex = 0;
}

void gl_2d_set_geometry(gl_2d_obj *obj, const float *data, size_t data_size, const uint16_t *idx, size_t idx_size) {
	GLenum glerr;
	while((glerr = glGetError()) != GL_NO_ERROR) {
		DBG_LOG("OpenGL error %d: %s", glerr, gluErrorString(glerr));
	}
	obj->count = idx_size / sizeof(uint16_t);
	glBindVertexArray(obj->vao);
	glBindBuffer(GL_ARRAY_BUFFER, obj->vbo);
	glBufferData(GL_ARRAY_BUFFER, data_size, data, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx_size, idx, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 4, NULL);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 4, (void *)(sizeof(GL_FLOAT) * 2));
	glBindVertexArray(0);
	while((glerr = glGetError()) != GL_NO_ERROR) {
		DBG_LOG("OpenGL error %d: %s", glerr, gluErrorString(glerr));
	}
}


static void update_model_matrix(gl_2d_obj *obj) {
	obj->model = glms_scale_make((vec3s){ obj->scale.x, obj->scale.y, 1.0f });
	obj->model = glms_translate(obj->model, (vec3s){ obj->pos.y, obj->pos.y, 0.0f });
}

void gl_2d_set_pos(gl_2d_obj *obj, vec2s pos) {
	obj->pos = pos;
	update_model_matrix(obj);
}

void gl_2d_set_scale(gl_2d_obj *obj, vec2s scale) {
	obj->scale = scale;
	update_model_matrix(obj);
	float *f = scale.raw;
}

void gl_2d_set_colour(gl_2d_obj *obj, vec4s colour) {
	obj->colour = colour;
}

void gl_2d_render(gl_2d_obj *obj) {
	GLint loc = glGetUniformLocation(program_gui, "model");
	glUniformMatrix4fv(loc, 1, GL_FALSE, obj->model.raw[0]);

	loc = glGetUniformLocation(program_gui, "colour");
	glUniform4fv(loc, 1, obj->colour.raw);

	GLenum glerr;
	while((glerr = glGetError()) != GL_NO_ERROR) {
		DBG_LOG("OpenGL error %d: %s", glerr, gluErrorString(glerr));
	}
	glBindVertexArray(obj->vao);

	glActiveTexture(GL_TEXTURE0); 
	glBindTexture(GL_TEXTURE_2D, obj->tex);
	loc = glGetUniformLocation(program_gui, "texture1");
	glUniform1i(loc, 0);

	glDrawElements(GL_TRIANGLES, obj->count, GL_UNSIGNED_SHORT, NULL);
	while((glerr = glGetError()) != GL_NO_ERROR) {
		DBG_LOG("OpenGL error %d: %s", glerr, gluErrorString(glerr));
	}
	glBindVertexArray(0);
}
