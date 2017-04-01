/* gp-rectangleselectiontool.c
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

#include "gp-rectangleselectiontool.h"

#include "gp-selectiontool.h"

struct _GPRectangleSelectionTool
{
    GPTool parent_instance;

    GdkPoint start_point;
    GdkPoint current_point;
    gboolean grabbed;
    GdkPixbuf *selection;
};

static void
gp_rectangle_selection_tool_interface_init (GPSelectionToolInterface *iface);

G_DEFINE_TYPE_WITH_CODE (GPRectangleSelectionTool, gp_rectangle_selection_tool, GP_TYPE_TOOL,
                         G_IMPLEMENT_INTERFACE (GP_TYPE_SELECTION_TOOL,
                                                gp_rectangle_selection_tool_interface_init))

static void
gp_rectangle_selection_tool_draw (GPTool *tool,
                                  cairo_t *cairo_context)
{
    GPRectangleSelectionTool *selection_tool = GP_RECTANGLE_SELECTION_TOOL (tool);
    GdkPoint start_point = selection_tool->start_point;
    GdkPoint current_point = selection_tool->current_point;
    static const double dashed3[] = {14.0, 6.0};

    if (start_point.x == current_point.x && start_point.y == current_point.y)
    {
        return;
    }

    cairo_set_dash(cairo_context, dashed3, 1, 0);
    cairo_set_source_rgb(cairo_context, 1, 0, 0);

    cairo_rectangle (cairo_context,
                     start_point.x, start_point.y,
                     current_point.x - start_point.x,
                     current_point.y - start_point.y);
    cairo_stroke (cairo_context);
}

static GtkWidget*
gp_line_tool_create_icon (GPTool *tool)
{
    return gtk_image_new_from_resource ("/org/gnome/Paint/toolicons/line.png");
}

static void
gp_rectangle_selection_tool_button_press (GPTool *tool, GdkEventButton *event)
{
    GdkPoint point = { event->x, event->y };
    GPRectangleSelectionTool *selection_tool = GP_RECTANGLE_SELECTION_TOOL (tool);

    selection_tool->current_point = selection_tool->start_point = point;
    selection_tool->grabbed = TRUE;
    gtk_widget_queue_draw (gp_tool_get_canvas_widget (tool));
}

static void
gp_rectangle_selection_tool_button_release (GPTool *tool, GdkEventButton *event, cairo_t *cairo_context)
{
    cairo_surface_t *surface = cairo_get_target (cairo_context);
    GPRectangleSelectionTool *selection_tool = GP_RECTANGLE_SELECTION_TOOL (tool);

    selection_tool->selection = gdk_pixbuf_get_from_surface (surface,
                                                             selection_tool->start_point.x,
                                                             selection_tool->start_point.y,
                                                             selection_tool->current_point.x,
                                                             selection_tool->current_point.y);

    selection_tool->grabbed = FALSE;
}

static void
gp_rectangle_selection_tool_move (GPTool *tool, GdkEventMotion *event)
{
    GdkPoint point = { event->x, event->y };

    if (GP_RECTANGLE_SELECTION_TOOL (tool)->grabbed == FALSE)
    {
        return;
    }

    GP_RECTANGLE_SELECTION_TOOL (tool)->current_point = point;
    gtk_widget_queue_draw (gp_tool_get_canvas_widget (tool));
}

static void
gp_rectangle_selection_tool_deactivate (GPTool *tool)
{
    GPRectangleSelectionTool *selection_tool = GP_RECTANGLE_SELECTION_TOOL (tool);

    selection_tool->grabbed = FALSE;

    if (selection_tool->selection != NULL)
    {
        g_object_unref (selection_tool->selection);
    }

    gtk_widget_queue_draw (gp_tool_get_canvas_widget (tool));
}

static GdkPixbuf*
gp_rectangle_selection_tool_get_selection(GPSelectionTool *self)
{
    return g_object_ref (GP_RECTANGLE_SELECTION_TOOL (self)->selection);
}

static void
gp_rectangle_selection_tool_interface_init (GPSelectionToolInterface *iface)
{
    iface->get_selection = gp_rectangle_selection_tool_get_selection;
}

static void
gp_rectangle_selection_tool_init (GPRectangleSelectionTool *self)
{
    self->grabbed = FALSE;
    self->selection = NULL;
}

static void
gp_rectangle_selection_tool_class_init (GPRectangleSelectionToolClass *klass)
{
    GPToolClass *tool_class = GP_TOOL_CLASS (klass);

    // todo finalize

    tool_class->create_icon = gp_line_tool_create_icon;
    tool_class->button_press = gp_rectangle_selection_tool_button_press;
    tool_class->button_release = gp_rectangle_selection_tool_button_release;
    tool_class->draw = gp_rectangle_selection_tool_draw;
    tool_class->move = gp_rectangle_selection_tool_move;
    tool_class->deactivate = gp_rectangle_selection_tool_deactivate;
}

GPTool*
gp_rectangle_selection_tool_create ()
{
    return  GP_TOOL (g_object_new (GP_TYPE_RECTANGLE_SELECTION_TOOL, NULL));
}

