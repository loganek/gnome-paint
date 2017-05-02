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
#include "gp-sizetoolproperty.h"
#include "gp-cairoutils.h"

#include "gp-shapetool-priv.h"

struct _GPRectangleTool
{
    GPShapeTool parent_instance;
    GPtrArray *properties;
};

G_DEFINE_TYPE (GPRectangleTool, gp_rectangle_tool, GP_TYPE_SHAPE_TOOL)

static GdkRectangle
gp_rectangle_tool_draw (GPShapeTool  *tool,
                        cairo_t      *cairo_context)
{
    GdkRectangle bbox_rect;
    GPShapeToolPrivate *priv = gp_shape_tool_get_priv (tool);
    GdkPointD start_point = priv->start_point;
    GdkPointD current_point = priv->current_point;

    cairo_rectangle (cairo_context,
                     start_point.x, start_point.y,
                     current_point.x - start_point.x,
                     current_point.y - start_point.y);


    bbox_rect = gp_cairo_stroke_get_bbox (cairo_context);

    cairo_stroke (cairo_context);

    return bbox_rect;
}

static GtkWidget*
gp_rectangle_tool_create_icon (GPTool *tool)
{
    return gtk_image_new_from_resource ("/org/gnome/Paint/toolicons/rectangle.png");
}

static const GPtrArray*
gp_rectangle_tool_get_properties (GPTool *tool)
{
    return GP_RECTANGLE_TOOL (tool)->properties;
}

static void
gp_rectangle_tool_free_property (gpointer data)
{
    g_object_unref (G_OBJECT (data));
}

static void
gp_rectangle_tool_finalize (GObject *gobj)
{
    GPRectangleTool *rectangle_tool = GP_RECTANGLE_TOOL (gobj);

    g_ptr_array_free (rectangle_tool->properties, TRUE);
    rectangle_tool->properties = NULL;
}

static void
gp_rectangle_tool_init (GPRectangleTool *self)
{
    self->properties = g_ptr_array_new_with_free_func (gp_rectangle_tool_free_property);

    g_ptr_array_add (self->properties, gp_size_tool_property_create ());
}

static void
gp_rectangle_tool_class_init (GPRectangleToolClass *klass)
{
    GPToolClass *tool_class = GP_TOOL_CLASS (klass);
    GPShapeToolClass *shape_tool_class = GP_SHAPE_TOOL_CLASS (klass);
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    tool_class->create_icon = gp_rectangle_tool_create_icon;
    tool_class->get_properties = gp_rectangle_tool_get_properties;

    shape_tool_class->draw_shape = gp_rectangle_tool_draw;

    gobject_class->finalize = gp_rectangle_tool_finalize;
}

GPTool*
gp_rectangle_tool_create ()
{
    return  GP_TOOL (g_object_new (GP_TYPE_RECTANGLE_TOOL, NULL));
}
