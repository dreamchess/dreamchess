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

/** @file
 *  @brief Gamegui clipping.
 */

#ifndef GG_CLIPPING_H
#define GG_CLIPPING_H

#include <gamegui/system.h>

/** @brief Returns the clipping rectangle.
 *
 *  @return The clipping rectangle, or NULL if no clipping rectangle set.
 */
gg_rect_t *gg_clipping_get();

/** @brief Adjusts the clipping rectangle. The clipping rectangle will be set
 *         to rect, clipped to the previous clipping rectangle (if any).
 *
 *  @param rect The clipping rectangle.
 */
void gg_clipping_adjust(gg_rect_t *rect);

/** @brief Undoes the last clipping adjustment. */
void gg_clipping_undo();

/** @brief Clips a rectangle to another rectangle.
 *
 *  @param rect1 The first rectangle.
 *  @param rect2 The second rectangle.
 *  @return The clipped rectangle.
 */
gg_rect_t gg_clipping_rect(gg_rect_t *rect1, gg_rect_t *rect2);

#endif /* GG_CLIPPING_H */
