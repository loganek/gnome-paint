/* gp-shapetool.c
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

#include "gp-shapetool.h"
#include "gp-documentmanager.h"

#include "gp-cairoutils.h"

typedef struct
{
    GdkPoint start_point;
    GdkPoint current_point;
    gboolean grabbed;
    GdkRectangle prev_bounding_rect;
} GPShapeToolPrivate;

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (GPShapeTool, gp_shape_tool, GP_TYPE_TOOL)

#define GP_SHAPE_TOOL_PRIV(shape_tool) ((GPShapeToolPrivate *) gp_shape_tool_get_instance_private (GP_SHAPE_TOOL (shape_tool)))

static void
get_min_max (gint v1, gint v2, gint *min, gint *max)
{
    if (v1 > v2)
    {
        *min = v2;
        *max = v1;
    }
    else
    {
        *min = v1;
        *max = v2;
    }
}

static GdkRectangle
gp_shape_tool_draw (GPShapeTool *tool, cairo_t *cairo_context)
{
    GPShapeToolClass *klass = GP_SHAPE_TOOL_GET_CLASS (tool);

    g_return_if_fail (klass->draw_shape != NULL);

    klass->draw_shape (tool, cairo_context);
}

static void
gp_shape_tool_button_press (GPTool *self, GdkEventButton *event, GdkPoint pos)
{
    GPShapeToolPrivate *priv = GP_SHAPE_TOOL_PRIV (self);

    priv->start_point = priv->current_point = pos;
    priv->grabbed = TRUE;
}

static void
gp_shape_tool_button_release (GPTool *self, GdkEventButton *event, GdkPoint pos)
{
    GPShapeToolPrivate *priv = GP_SHAPE_TOOL_PRIV (self);
    GPDocument *document = gp_document_manager_get_active_document (gp_document_manager_get_default ());
    cairo_t *cr = cairo_create (gp_document_get_surface (document));

    priv->grabbed = FALSE;

    gp_cairo_surface_clear (gp_document_get_tool_surface (document));

    priv->current_point = pos;

    gp_shape_tool_draw (GP_SHAPE_TOOL (self), cr);
    cairo_destroy (cr);
    gp_document_request_update_view (document);

    // doc.Workspace.Invalidate (last_dirty.ToGdkRectangle ());
}

static void
gp_shape_tool_move (GPTool *self, GdkEventMotion *event, GdkPoint pos)
{
    GPShapeToolPrivate *priv = GP_SHAPE_TOOL_PRIV (self);
    GPDocument *document = gp_document_manager_get_active_document (gp_document_manager_get_default ());
    cairo_surface_t *surface = gp_document_get_tool_surface (document);
    cairo_t *cr = cairo_create (surface);

    if (!priv->grabbed)
    {
        return;
    }

    gp_cairo_surface_clear (surface);

    priv->current_point = pos;

    GdkRectangle bounding_rect = gp_shape_tool_draw (GP_SHAPE_TOOL (self), cr);
    cairo_destroy (cr);
    // TODO bounding rect clamp()
    // doc.Workspace.Invalidate (last_dirty.ToGdkRectangle ());
    // doc.Workspace.Invalidate (dirty.ToGdkRectangle ());

    gp_document_request_update_view (document);

    priv->prev_bounding_rect = bounding_rect;
}

static void
gp_shape_tool_init (GPShapeTool *self)
{
    GPShapeToolPrivate *priv = GP_SHAPE_TOOL_PRIV (self);
    GdkPoint init_point = { -1, -1 };
    GdkRectangle init_rect = { 0, 0, 0, 0 };

    priv->grabbed = FALSE;
    priv->start_point = priv->current_point = init_point;
    priv->prev_bounding_rect = init_rect;
}

static void
gp_shape_tool_class_init (GPShapeToolClass *klass)
{
    GPToolClass *tool_class = GP_TOOL_CLASS (klass);

    tool_class->button_press = gp_shape_tool_button_press;
    tool_class->button_release = gp_shape_tool_button_release;
    tool_class->move = gp_shape_tool_move;

    klass->draw_shape = NULL;
}

GdkPoint
gp_shape_tool_get_start_point (GPShapeTool *tool)
{
    return GP_SHAPE_TOOL_PRIV (tool)->start_point;
}

GdkPoint
gp_shape_tool_get_current_point (GPShapeTool *tool)
{
    return GP_SHAPE_TOOL_PRIV (tool)->current_point;
}

GdkRectangle
gp_shape_tool_get_bbox (GPShapeTool *tool)
{
    GdkRectangle bbox;
    GPShapeToolPrivate *priv = GP_SHAPE_TOOL_PRIV (tool);

    // TODO unclear code
    get_min_max (priv->start_point.x, priv->current_point.x, &bbox.x, &bbox.width);
    bbox.width -= bbox.x;

    get_min_max (priv->start_point.y, priv->current_point.y, &bbox.y, &bbox.height);
    bbox.height -= bbox.y;

    return bbox;
}
