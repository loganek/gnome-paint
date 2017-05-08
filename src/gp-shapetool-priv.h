/* gp-shapetool-priv.h
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

#ifndef GP_SHAPE_TOOL_PRIV_H_
#define GP_SHAPE_TOOL_PRIV_H_

#include "gp-shapetool.h"

G_BEGIN_DECLS

typedef struct
{
    GdkPointD start_point;
    gdouble width;
    gdouble height;
    gboolean grabbed;
    GdkRectangle prev_bounding_rect;
    guint trigger_button;
} GPShapeToolPrivate;

GPShapeToolPrivate * gp_shape_tool_get_priv (GPShapeTool *shape_tool);

#define GP_SHAPE_TOOL_PRIV(tool) gp_shape_tool_get_priv (GP_SHAPE_TOOL (tool))

void _gp_shape_tool_clear_tool_layer (GPShapeToolPrivate *self_priv);

void _gp_shape_tool_draw_shape (GPShapeTool *self, cairo_surface_t *draw_surface);

G_END_DECLS

#endif /* GP_SHAPE_TOOL_PRIV_H_ */
