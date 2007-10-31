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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#ifdef WITH_UI_SDLGL

#include <stdio.h>

#include "matrix.h"

void mat_print(matrix4x4_t *mat)
{
   int i;

    for (i = 0; i < 3; i++)
        printf("| %f %f %f %f |\n", mat->e[0][i], mat->e[1][i], mat->e[2][i], mat->e[3][i]);
}

void mat_print_v(vector3_t *vect)
{
    printf("| %f %f %f |\n", vect->e[0], vect->e[1], vect->e[2]);
}

matrix4x4_t mat_identity()
{
    matrix4x4_t mat;
    int i, j;

    for (i = 0; i < 3; i++)
        for(j = 0; j < 3; j++)
            if (i == j)
                mat.e[i][j] = 1;
            else
                mat.e[i][j] = 0;

    return mat;
}

matrix4x4_t mat_multiply(matrix4x4_t *mat1, matrix4x4_t *mat2)
{
    int i, j, k;
    matrix4x4_t mat;

    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
        {
            mat.e[i][j] = 0;
            for (k = 0; k < 4; k++)
                mat.e[i][j] += mat1->e[k][j] * mat2->e[i][k];
        }

    return mat;
}


vector3_t mat_apply(matrix4x4_t *mat, vector3_t *vect)
{
    int i, j;
    vector3_t v;

    for (j = 0; j < 3; j++)
    {
        v.e[j] = 0;
        for (i = 0; i < 3; i++)
        {
            v.e[j] += mat->e[i][j] * vect->e[i];
        }
        v.e[j] += mat->e[3][j];
    }

    return v;
}

#endif /* WITH_UI_SDLGL */
