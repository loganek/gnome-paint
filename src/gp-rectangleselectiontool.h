/* gp-rectangleselectiontool.h
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

#ifndef GP_RECTANGLE_SELECTION_TOOL_H_
#define GP_RECTANGLE_SELECTION_TOOL_H_

#include "gp-shapetool.h"

G_BEGIN_DECLS

#define GP_TYPE_RECTANGLE_SELECTION_TOOL (gp_rectangle_selection_tool_get_type ())
G_DECLARE_FINAL_TYPE (GPRectangleSelectionTool, gp_rectangle_selection_tool, GP, RECTANGLE_SELECTION_TOOL, GPShapeTool)

GPTool* gp_rectangle_selection_tool_create ();

G_END_DECLS

#endif /* GP_RECTANGLE_SELECTION_TOOL_H_ */
