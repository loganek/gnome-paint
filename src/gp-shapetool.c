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
#include "gp-colormanager.h"
#include "gp-cairoutils.h"
#include "gp-drawhistoryitem.h"

#include "gp-shapetool-priv.h"

static GdkRectangle zero_rectangle = { 0, 0, 0, 0 };

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (GPShapeTool, gp_shape_tool, GP_TYPE_TOOL)

static void
gp_shape_tool_load_drawing_color (GPShapeToolPrivate *self_priv, cairo_t *cr)
{
    GPColorManager *manager = gp_color_manager_default ();
    GdkRGBA color;
    GdkRGBA bg_color;

    gp_color_manager_get_color (manager, &color, &bg_color);

    if (self_priv->trigger_button != GDK_BUTTON_PRIMARY)
    {
        color = bg_color;
    }

    cairo_set_source_rgba (cr, color.red, color.green, color.blue, color.alpha);
}

void
_gp_shape_tool_clear_tool_layer (GPShapeToolPrivate *self_priv)
{
    GPDocument *document = gp_document_manager_get_active_document (gp_document_manager_get_default ());

    gp_cairo_surface_clear (gp_document_get_tool_surface (document));
    gp_document_request_update_view (document, &self_priv->prev_bounding_rect);
}

static GdkRectangle
gp_shape_tool_draw (GPShapeTool *tool, cairo_t *cairo_context)
{
    GPShapeToolClass *klass = GP_SHAPE_TOOL_GET_CLASS (tool);

    g_return_val_if_fail (klass->draw_shape != NULL, zero_rectangle);

    return klass->draw_shape (tool, cairo_context);
}

void
_gp_shape_tool_draw_shape (GPShapeTool *self, cairo_surface_t *draw_surface)
{
    GPShapeToolPrivate *priv = GP_SHAPE_TOOL_PRIV (self);
    GPDocument *document;
    cairo_t *cr;
    GdkRectangle bounding_rect;

    document = gp_document_manager_get_active_document (gp_document_manager_get_default ());

    _gp_shape_tool_clear_tool_layer (priv);

    cr = cairo_create (draw_surface);
    gp_shape_tool_load_drawing_color (priv, cr);
    cairo_set_antialias (cr, CAIRO_ANTIALIAS_NONE);

    bounding_rect = gp_shape_tool_draw (GP_SHAPE_TOOL (self), cr);
    cairo_destroy (cr);

    gp_document_request_update_view (document, &bounding_rect);
    priv->prev_bounding_rect = bounding_rect;
}

static void
gp_shape_tool_button_press (GPTool *self, GdkEventButton *event, GdkPointD pos)
{
    GPShapeToolPrivate *priv = GP_SHAPE_TOOL_PRIV (self);
    GPDocument *document = gp_document_manager_get_active_document (gp_document_manager_get_default ());

    if (priv->grabbed && priv->trigger_button != event->button)
    {
        priv->grabbed = FALSE;
        _gp_shape_tool_clear_tool_layer (priv);
        return;
    }

    priv->start_point = pos;
    priv->width = priv->height = 0.0;
    priv->grabbed = TRUE;
    priv->trigger_button = event->button;

    _gp_shape_tool_draw_shape (GP_SHAPE_TOOL (self), gp_document_get_tool_surface (document));
}

static void
gp_shape_tool_button_release (GPTool *self, GdkEventButton *event, GdkPointD pos)
{
    GPShapeToolPrivate *priv = GP_SHAPE_TOOL_PRIV (self);
    GPDocument *document;
    GPHistory *history;
    cairo_surface_t *surface;

    if (!priv->grabbed)
    {
        return;
    }

    document = gp_document_manager_get_active_document (gp_document_manager_get_default ());
    surface = gp_document_get_surface (document);

    history = gp_document_get_history (document);
    gp_history_add_item (history, gp_draw_history_item_create (surface));

    _gp_shape_tool_draw_shape (GP_SHAPE_TOOL (self), surface);

    priv->grabbed = FALSE;
    priv->prev_bounding_rect = zero_rectangle;
}

static void
gp_shape_tool_move (GPTool *self, GdkEventMotion *event, GdkPointD pos)
{
    GPShapeToolPrivate *priv = GP_SHAPE_TOOL_PRIV (self);

    if (priv->grabbed)
    {
        GPDocument *document = gp_document_manager_get_active_document (gp_document_manager_get_default ());
        cairo_surface_t *tool_surface = gp_document_get_tool_surface (document);

        priv->width = pos.x - priv->start_point.x; // TODO + 0.5;
        priv->height = pos.y - priv->start_point.y;

        _gp_shape_tool_draw_shape (GP_SHAPE_TOOL (self), tool_surface);
    }
}

static void
gp_shape_tool_init (GPShapeTool *self)
{
    GPShapeToolPrivate *priv = GP_SHAPE_TOOL_PRIV (self);
    GdkPointD init_point = { -1, -1 };

    priv->grabbed = FALSE;
    priv->start_point = init_point;
    priv->width = priv->height = 0;
    priv->prev_bounding_rect = zero_rectangle;
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

GPShapeToolPrivate *
gp_shape_tool_get_priv (GPShapeTool *shape_tool)
{
    return (GPShapeToolPrivate *) gp_shape_tool_get_instance_private (shape_tool);
}
