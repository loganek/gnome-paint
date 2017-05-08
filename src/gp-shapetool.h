/* gp-shapetool.h
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

#ifndef GP_SHAPE_TOOL_H_
#define GP_SHAPE_TOOL_H_

#include "gp-tool.h"

G_BEGIN_DECLS

#define GP_TYPE_SHAPE_TOOL (gp_shape_tool_get_type ())
G_DECLARE_DERIVABLE_TYPE (GPShapeTool, gp_shape_tool, GP, SHAPE_TOOL, GPTool)

struct _GPShapeToolClass
{
    GPToolClass parent_class;

    GdkRectangle (*draw_shape) (GPShapeTool *tool,
                                cairo_t     *cairo_context);
};

G_END_DECLS

#endif /* GP_SHAPE_TOOL_H_ */
