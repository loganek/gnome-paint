/* gp-tool-priv.h
 *
 * Copyright (C) 2017 Marcin Kolny
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GP_TOOL_PRIV_H_
#define GP_TOOL_PRIV_H_

#include <gdk/gdk.h>

G_BEGIN_DECLS

void _gp_tool_load_drawing_color (gint button, cairo_t *cr);

void _gp_tool_clear_tool_layer (const GdkRectangle *bounding_rect);

G_END_DECLS

#endif // GP_TOOL_PRIV_H_
