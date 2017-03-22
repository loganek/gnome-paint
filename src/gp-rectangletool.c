/* gp-rectangletool.c
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

#include "gp-rectangletool.h"

typedef struct {
    GPTool parent_instance;
} GPRectangleTool;

static void
gp_rectangle_tool_draw (GPTool *tool,
                        GdkPoint start_point,
                        GdkPoint current_point,
		        cairo_t *cairo_context)
{
    cairo_rectangle (cairo_context,
		     start_point.x, start_point.y,
		     current_point.x - start_point.x,
		     current_point.y - start_point.y);
    cairo_stroke (cairo_context);
}

static GtkWidget*
gp_rectangle_tool_create_icon (GPTool *tool)
{
    return gtk_image_new_from_resource ("/org/gnome/Paint/toolicons/rectangle.png");
}

GPTool*
gp_rectangle_tool_create ()
{
    GPRectangleTool *rectangle_tool = g_new (GPRectangleTool, 1);

    rectangle_tool->parent_instance.draw = gp_rectangle_tool_draw;
    rectangle_tool->parent_instance.create_icon = gp_rectangle_tool_create_icon;

    return (GPTool*) rectangle_tool;
}

