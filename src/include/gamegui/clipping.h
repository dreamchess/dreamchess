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
