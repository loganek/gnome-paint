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

struct _GPLineTool
{
    GPBaseTool parent_instance;
};

G_DEFINE_TYPE (GPLineTool, gp_line_tool, GP_TYPE_BASE_TOOL)

static void
gp_line_tool_draw (GPBaseTool *tool,
                   cairo_t *cairo_context)
{
    GdkPoint start_point = gp_base_tool_get_start_point (tool);
    GdkPoint current_point = gp_base_tool_get_current_point (tool);

    cairo_move_to (cairo_context,
                   start_point.x,
                   start_point.y);
    cairo_line_to (cairo_context,
                   current_point.x,
                   current_point.y);
    cairo_stroke (cairo_context);
}

static GtkWidget*
gp_line_tool_create_icon (GPTool *tool)
{
    return gtk_image_new_from_resource ("/org/gnome/Paint/toolicons/line.png");
}

static void
gp_line_tool_button_release (GPBaseTool *tool, GdkEventButton *event, cairo_t *cairo_context)
{
    gp_line_tool_draw (tool, cairo_context);
}

static void
gp_line_tool_init (GPLineTool *self)
{
}

static void
gp_line_tool_class_init (GPLineToolClass *klass)
{
    GPToolClass *tool_class = GP_TOOL_CLASS (klass);
    GPBaseToolClass *base_tool_class = GP_BASE_TOOL_CLASS (klass);

    tool_class->create_icon = gp_line_tool_create_icon;

    base_tool_class->pre_button_release = gp_line_tool_button_release;
    base_tool_class->draw_bbox = gp_line_tool_draw;
}

GPTool*
gp_line_tool_create ()
{
    return  GP_TOOL (g_object_new (GP_TYPE_LINE_TOOL, NULL));
}

