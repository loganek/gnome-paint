/* gp-linetool.c
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

#include "gp-linetool.h"

typedef struct {
    GPTool parent_instance;
} GPLineTool;

static void
gp_line_tool_draw (GPTool *tool,
                  GdkPoint start_point,
                  GdkPoint current_point,
		  cairo_t *cairo_context)
{
    cairo_move_to (cairo_context,
		   start_point.x,
		   start_point.y);
    cairo_line_to (cairo_context,
		   current_point.x,
		   current_point.y);
    cairo_stroke (cairo_context);
}

GPTool* gp_line_tool_create ()
{
    GPLineTool *line_tool = g_new (GPLineTool, 1);

    line_tool->parent_instance.draw = gp_line_tool_draw;

    return line_tool;
}

