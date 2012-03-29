/*  DreamChess
**
**  DreamChess is the legal property of its developers, whose names are too
**  numerous to list here. Please refer to the COPYRIGHT file distributed
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

typedef struct vector3
{
    float e[3];
} vector3_t;

typedef struct matrix4x4
{
    float e[4][4];
} matrix4x4_t;

void mat_print(matrix4x4_t *mat);
matrix4x4_t mat_identity();
matrix4x4_t mat_multiply(matrix4x4_t *mat1, matrix4x4_t *mat2);
