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

static const struct {
	const char *vertex_shader;
	const char *fragment_shader;
} shader_2d_sources[SHADER_2D_COUNT] = {
	{ "shaders/vert_textured.glsl", "shaders/frag_textured.glsl" }, // SHADER_2D_TEXTURED
	{ "shaders/vert_shaded.glsl", "shaders/frag_shaded.glsl" }      // SHADER_2D_SHADED
};

static GLuint shader_2d_programs[SHADER_2D_COUNT];

static char *load_file(const char *filename) {
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
	char *shader_src = load_file(filename);
	GLuint shader = glCreateShader(shader_type);
	glShaderSource(shader, 1, &shader_src, NULL);
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE) {
		GLint loglen;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &loglen);
		GLchar *log = malloc(loglen);
		glGetShaderInfoLog(shader, loglen, NULL, log);
		DBG_ERROR("Shader '%s' failed to compile: %s", filename, log);
		exit(1);
	}

	free(shader_src);
	return shader;
}

void gl_init(void) {
	for (unsigned int i = 0; i < SHADER_2D_COUNT; ++i) {
		GLuint vertex_shader = load_shader(shader_2d_sources[i].vertex_shader, GL_VERTEX_SHADER);
		GLuint fragment_shader = load_shader(shader_2d_sources[i].fragment_shader, GL_FRAGMENT_SHADER);
		GLuint program = glCreateProgram();
		glAttachShader(program, vertex_shader);
		glAttachShader(program, fragment_shader);
		glLinkProgram(program);

		GLint success;
		glGetProgramiv(program, GL_LINK_STATUS, &success);

		if (success == GL_FALSE) {
			GLint loglen;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &loglen);
			GLchar *log = malloc(loglen);
			glGetProgramInfoLog(program, loglen, NULL, log);
			DBG_ERROR("Program with '%s' and '%s' failed to link: %s",
				shader_2d_sources[i].vertex_shader, shader_2d_sources[i].fragment_shader, log);
			exit(1);
		}

		shader_2d_programs[i] = program;
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
	}
}

void gl_set_gui(unsigned int width, unsigned int height) {
	mat4s m = glms_ortho(0.0f, 480 * width / (float)height, 0.0f, 480.0f, -1.0f, 1.0f);

	for (unsigned int i = 0; i < SHADER_2D_COUNT; ++i) {
		glUseProgram(shader_2d_programs[i]);
		GLint loc = glGetUniformLocation(shader_2d_programs[i], "projection");
		glUniformMatrix4fv(loc, 1, GL_FALSE, m.raw[0]);
	}
}

void gl_2d_init(gl_2d_obj *obj, shader_2d_t shader) {
	obj->shader = shader;
	obj->tex = 0;
	obj->count = 0;
	glGenBuffers(1, &obj->vbo);
	glGenBuffers(1, &obj->ebo);
	glGenVertexArrays(1, &obj->vao);
	obj->model = glms_mat4_identity();
	obj->colour = glms_vec4_one();
	obj->scale = glms_vec2_one();
	obj->pos = glms_vec2_zero();
	obj->mode = 0;
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

static void set_vertex_attrib_pointers(GLsizei stride, GLint attrib_1_size) {
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * stride, NULL);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, attrib_1_size, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * stride, (void *)(sizeof(GL_FLOAT) * 2));
}

void gl_2d_set_triangles(gl_2d_obj *obj, const float *data, size_t data_size, const uint16_t *idx, size_t idx_size) {
	const GLint attrib_1_size = (obj->shader == SHADER_2D_TEXTURED ? 2 : 4);
	const GLsizei stride = 2 + attrib_1_size;

	obj->count = idx_size / sizeof(uint16_t);
	glBindVertexArray(obj->vao);
	glBindBuffer(GL_ARRAY_BUFFER, obj->vbo);
	glBufferData(GL_ARRAY_BUFFER, data_size, data, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx_size, idx, GL_STATIC_DRAW);
	set_vertex_attrib_pointers(stride, attrib_1_size);
	glBindVertexArray(0);

	obj->mode = GL_TRIANGLES;
}

void gl_2d_init_textured_rect(gl_2d_obj *obj, vec2s p1, vec2s p2, vec2s t1, vec2s t2) {
	gl_2d_init(obj, SHADER_2D_TEXTURED);

	const float data[] = {
		p1.x, p1.y, t1.u, t2.v, 
		p2.x, p1.y, t2.u, t2.v,
		p2.x, p2.y, t2.u, t1.v,
		p1.x, p2.y, t1.u, t1.v
	};

	const uint16_t indices[] = { 0, 1, 2, 0, 2, 3 };

	gl_2d_set_triangles(obj, data, sizeof(data), indices, sizeof(indices));
}

void gl_2d_init_shaded_rect(gl_2d_obj *obj, vec2s p1, vec2s p2, vec4s c1, vec4s c2, vec4s c3, vec4s c4) {
	gl_2d_init(obj, SHADER_2D_SHADED);

	const float data[] = {
		p1.x, p1.y, c1.r, c1.g, c1.b, c1.a, 
		p2.x, p1.y, c2.r, c2.g, c2.b, c2.a,
		p2.x, p2.y, c3.r, c3.g, c3.b, c3.a,
		p1.x, p2.y, c4.r, c4.g, c4.b, c4.a
	};

	const uint16_t indices[] = { 0, 1, 2, 0, 2, 3 };

	gl_2d_set_triangles(obj, data, sizeof(data), indices, sizeof(indices));
}

void gl_2d_set_line_loop(gl_2d_obj *obj, const float *data, size_t data_size) {
	const GLint attrib_1_size = (obj->shader == SHADER_2D_TEXTURED ? 2 : 4);
	const GLsizei stride = 2 + attrib_1_size;

	obj->count = data_size / stride / sizeof(GL_FLOAT);
	glBindVertexArray(obj->vao);
	glBindBuffer(GL_ARRAY_BUFFER, obj->vbo);
	glBufferData(GL_ARRAY_BUFFER, data_size, data, GL_STATIC_DRAW);
	set_vertex_attrib_pointers(stride, attrib_1_size);
	glBindVertexArray(0);

	obj->mode = GL_LINE_LOOP;
}

void gl_2d_init_rect(gl_2d_obj *obj, vec2s p1, vec2s p2) {
	gl_2d_init(obj, SHADER_2D_SHADED);

	const float data[] = {
		p1.x, p1.y, 1.0f, 1.0f, 1.0f, 1.0f, 
		p2.x, p1.y, 1.0f, 1.0f, 1.0f, 1.0f,
		p2.x, p2.y, 1.0f, 1.0f, 1.0f, 1.0f,
		p1.x, p2.y, 1.0f, 1.0f, 1.0f, 1.0f
	};

	gl_2d_set_line_loop(obj, data, sizeof(data));
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
	GLuint program = shader_2d_programs[obj->shader];
	glUseProgram(program);

	GLint loc = glGetUniformLocation(program, "model");
	glUniformMatrix4fv(loc, 1, GL_FALSE, obj->model.raw[0]);

	loc = glGetUniformLocation(program, "colour");
	glUniform4fv(loc, 1, obj->colour.raw);
	glBindVertexArray(obj->vao);

	glActiveTexture(GL_TEXTURE0); 
	glBindTexture(GL_TEXTURE_2D, obj->tex);
	loc = glGetUniformLocation(program, "texture1");
	glUniform1i(loc, 0);

	if (obj->mode == GL_TRIANGLES)
		glDrawElements(GL_TRIANGLES, obj->count, GL_UNSIGNED_SHORT, NULL);
	else if (obj->mode == GL_LINE_LOOP)
		glDrawArrays(GL_LINE_LOOP, 0, obj->count);

	glBindVertexArray(0);
}
