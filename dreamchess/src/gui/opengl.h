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

#ifndef GUI_OPENGL_H
#define GUI_OPENGL_H

#include "cglm/struct.h"

typedef enum {
	SHADER_2D_TEXTURED,
	SHADER_2D_SHADED,
	SHADER_2D_COUNT
} shader_2d_t;

typedef struct {
	shader_2d_t shader;
	GLuint vbo, ebo, vao;
	unsigned int count;
	GLuint tex;
	mat4s model;
	vec2s pos;
	vec2s scale;
	vec4s colour;
	GLenum mode;
} gl_2d_obj;

void gl_init(void);
void gl_set_gui(unsigned int width, unsigned int height);
void gl_2d_init(gl_2d_obj *obj, shader_2d_t shader);
void gl_2d_init_textured_rect(gl_2d_obj *obj, vec2s p1, vec2s p2, vec2s t1, vec2s t2);
void gl_2d_init_shaded_rect(gl_2d_obj *obj, vec2s p1, vec2s p2, vec4s c1, vec4s c2, vec4s c3, vec4s c4);
void gl_2d_init_rect(gl_2d_obj *obj, vec2s p1, vec2s p2);
void gl_2d_release(gl_2d_obj *obj);
void gl_2d_set_texture(gl_2d_obj *obj, GLuint tex);
void gl_2d_delete_texture(gl_2d_obj *obj);
void gl_2d_set_pos(gl_2d_obj *obj, vec2s pos);
void gl_2d_set_scale(gl_2d_obj *obj, vec2s pos);
void gl_2d_set_colour(gl_2d_obj *obj, vec4s colour);
void gl_2d_set_triangles(gl_2d_obj *obj, const float *data, size_t data_size, const uint16_t *idx, size_t idx_size);
void gl_2d_set_line_loop(gl_2d_obj *obj, const float *data, size_t data_size);
void gl_2d_render(gl_2d_obj *obj);

#endif
