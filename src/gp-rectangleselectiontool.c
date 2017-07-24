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
#include "gp-shapetool-priv.h"
#include "gp-tool-priv.h"

#include "gp-documentmanager.h"
#include "gp-cairoutils.h"
#include "gp-colormanager.h"

#define RESIZER_SIZE 10

// TODO It can't be constant, but hast to be width/height/x/y dependent
// We do two 'double to int" casts (for (x, y) and (width, height)
#define SELECTION_MARGIN 2

struct _GPRectangleSelectionTool
{
    GPShapeTool parent_instance;

    gdouble offset_x;
    gdouble offset_y;
    GdkPointD root_point;
    GdkPixbuf *selection;
    gboolean is_selected;
};

G_DEFINE_TYPE (GPRectangleSelectionTool, gp_rectangle_selection_tool, GP_TYPE_SHAPE_TOOL)

static void
_fix_dimension_if_less_than_zero (gdouble *coordinate, gdouble *value)
{
    if (*value < 0)
    {
        *value = -*value;
        *coordinate -= *value;
    }
}

static void
gp_rectangle_selection_tool_draw_selection_to_location (GPRectangleSelectionTool *selection_tool, cairo_t *cairo_context)
{
    GPShapeToolPrivate *priv = GP_SHAPE_TOOL_PRIV (selection_tool);

    g_return_if_fail (selection_tool->selection != NULL);

    gdk_cairo_set_source_pixbuf (cairo_context, selection_tool->selection,
                                 (gint)priv->start_point.x,
                                 (gint)priv->start_point.y);

    cairo_rectangle(cairo_context,
                    (gint)priv->start_point.x,
                    (gint)priv->start_point.y,
                    (gint)priv->width + SELECTION_MARGIN,
                    (gint)priv->height + SELECTION_MARGIN);
    cairo_clip(cairo_context);
    cairo_paint(cairo_context);
}

static void
gp_rectangle_selection_tool_draw_bg_placeholder (GPRectangleSelectionTool *selection_tool, cairo_t *cairo_context)
{
    GPShapeToolPrivate *priv = GP_SHAPE_TOOL_PRIV (selection_tool);
    GPColorManager *color_manager = gp_color_manager_default ();
    GdkRGBA bg_color;

    gp_color_manager_get_color (color_manager, NULL, &bg_color);

    cairo_save (cairo_context);
    cairo_set_source_rgba (cairo_context, bg_color.red, bg_color.green, bg_color.blue, bg_color.alpha);
    cairo_rectangle (cairo_context, selection_tool->root_point.x, selection_tool->root_point.y,
                     priv->width, priv->height);
    cairo_fill (cairo_context);
    cairo_restore (cairo_context);
}

static GdkRectangle
gp_rectangle_selection_tool_draw_bounding_box (GPRectangleSelectionTool *self, cairo_t *cairo_context)
{
    static double pattern[] = {5.0, 5.0};
    GdkRectangle bbox_rect;
    GPShapeToolPrivate *priv = GP_SHAPE_TOOL_PRIV (self);

    cairo_save (cairo_context);
    cairo_set_source_rgba (cairo_context, 1.0, 1.0, 0.0, 1.0);
    cairo_set_dash (cairo_context, pattern, 2, 0);
    cairo_rectangle (cairo_context,
                     priv->start_point.x, priv->start_point.y,
                     priv->width, priv->height);

    bbox_rect = gp_cairo_stroke_get_bbox (cairo_context);

    cairo_stroke (cairo_context);
    cairo_restore (cairo_context);

    return bbox_rect;
}

static GdkRectangle
gp_rectangle_selection_tool_draw (GPShapeTool *tool,
                                  cairo_t     *cairo_context)
{
    GPRectangleSelectionTool *selection_tool = GP_RECTANGLE_SELECTION_TOOL (tool);

    if (selection_tool->is_selected)
    {
        gp_rectangle_selection_tool_draw_bg_placeholder (selection_tool, cairo_context);
        gp_rectangle_selection_tool_draw_selection_to_location (selection_tool, cairo_context);
    }

    return gp_rectangle_selection_tool_draw_bounding_box (selection_tool, cairo_context);
}

static void
gp_rectangle_selection_tool_update_selection (GPRectangleSelectionTool *selection_tool,
                                              GdkPixbuf                *selection)
{
    GPDocument *document = gp_document_manager_get_active_document (gp_document_manager_get_default ());

    g_clear_object (&selection_tool->selection);
    selection_tool->selection = selection;
    gp_document_set_selection (document, selection == NULL ? NULL : g_object_ref (selection));
}

static void
gp_rectangle_selection_tool_apply_motion (GPRectangleSelectionTool *self)
{
    GPDocument *document = gp_document_manager_get_active_document (gp_document_manager_get_default ());

    cairo_t *cairo_context = cairo_create (gp_document_get_surface (document));

    gp_rectangle_selection_tool_draw_bg_placeholder (self, cairo_context);
    gp_rectangle_selection_tool_draw_selection_to_location (self, cairo_context);

    cairo_destroy (cairo_context);
}


static GtkWidget*
gp_line_tool_create_icon (GPTool *tool)
{
    return gtk_image_new_from_resource ("/org/gnome/Paint/toolicons/rectangleselection.png");
}

static gboolean
gp_rectangle_selection_tool_region_in_selection (GPRectangleSelectionTool *self, gdouble x, gdouble y)
{
    GPShapeToolPrivate *priv = GP_SHAPE_TOOL_PRIV (self);
    cairo_rectangle_t rect = { priv->start_point.x, priv->start_point.y,
                               priv->width, priv->height };

    return gp_cairo_point_is_in_rectangle (&rect, x, y);
}

static void
gp_rectangle_selection_tool_button_press (GPTool *tool, GdkEventButton *event, GdkPointD pos)
{
    GPRectangleSelectionTool *selection_tool = GP_RECTANGLE_SELECTION_TOOL (tool);

    if (selection_tool->is_selected
            && gp_rectangle_selection_tool_region_in_selection (selection_tool, pos.x, pos.y))
    {
        GP_SHAPE_TOOL_PRIV (tool)->grabbed = TRUE;
        selection_tool->offset_x = pos.x - GP_SHAPE_TOOL_PRIV (tool)->start_point.x;
        selection_tool->offset_y = pos.y - GP_SHAPE_TOOL_PRIV (tool)->start_point.y;
    }
    else
    {
        if (selection_tool->is_selected)
        {
            gp_rectangle_selection_tool_apply_motion (selection_tool);
            selection_tool->is_selected = FALSE;
        }

        GP_TOOL_CLASS (gp_rectangle_selection_tool_parent_class)->button_press (tool, event, pos);
    }
}

static void
gp_rectangle_selection_tool_button_release (GPTool *tool, GdkEventButton *event, GdkPointD pos)
{
    GPShapeToolPrivate *priv = GP_SHAPE_TOOL_PRIV (tool);
    GPRectangleSelectionTool *selection_tool = GP_RECTANGLE_SELECTION_TOOL (tool);
    GPDocument *document;

    if (!priv->grabbed)
    {
        return;
    }

    document = gp_document_manager_get_active_document (gp_document_manager_get_default ());

    _gp_shape_tool_draw_shape (GP_SHAPE_TOOL (tool), gp_document_get_tool_surface (document));

    _fix_dimension_if_less_than_zero (&priv->start_point.x, &priv->width);
    _fix_dimension_if_less_than_zero (&priv->start_point.y, &priv->height);

    if (!selection_tool->is_selected)
    {
        if (priv->width > 0 && priv->height > 0)
        {
            GdkPixbuf *selection = gdk_pixbuf_get_from_surface (gp_document_get_surface (document),
                                                                priv->start_point.x, priv->start_point.y,
                                                                (gint)priv->width + SELECTION_MARGIN, (gint)priv->height + SELECTION_MARGIN);
            gp_rectangle_selection_tool_update_selection (selection_tool, selection);
            selection_tool->is_selected = TRUE;
        }
        else
        {
            gp_rectangle_selection_tool_update_selection (selection_tool, NULL);
        }

        selection_tool->root_point = priv->start_point;
    }

    priv->grabbed = FALSE;
}

static void
gp_rectangle_selection_tool_move (GPTool *tool, GdkEventMotion *event, GdkPointD pos)
{
    GPShapeToolPrivate *priv = GP_SHAPE_TOOL_PRIV (tool);
    GPRectangleSelectionTool *selection_tool = GP_RECTANGLE_SELECTION_TOOL (tool);
    GPDocument *document = gp_document_manager_get_active_document (gp_document_manager_get_default ());
    cairo_surface_t *tool_surface = gp_document_get_tool_surface (document);

    if (!priv->grabbed)
    {
        return;
    }

    if (selection_tool->is_selected)
    {
        priv->start_point.x = pos.x - selection_tool->offset_x;
        priv->start_point.y = pos.y - selection_tool->offset_y;

        _gp_shape_tool_draw_shape (GP_SHAPE_TOOL (tool), tool_surface);
    }
    else
    {
        GP_TOOL_CLASS (gp_rectangle_selection_tool_parent_class)->move (tool, event, pos);
    }
}

static void
gp_rectangle_selection_tool_deactivate (GPTool *tool)
{
    GPRectangleSelectionTool *selection_tool = GP_RECTANGLE_SELECTION_TOOL (tool);
    if (selection_tool->is_selected)
    {
        gp_rectangle_selection_tool_apply_motion (selection_tool);
        selection_tool->is_selected = FALSE;
        gp_rectangle_selection_tool_update_selection (selection_tool, NULL);
        _gp_tool_clear_tool_layer (&(GP_SHAPE_TOOL_PRIV (tool)->prev_bounding_rect));
    }
}

static void
gp_rectangle_selection_tool_init (GPRectangleSelectionTool *self)
{
    self->is_selected = FALSE;
    self->selection = NULL;
}

static void
gp_rectangle_selection_tool_finalize (GObject *self)
{
    GPRectangleSelectionTool *tool = GP_RECTANGLE_SELECTION_TOOL (self);

    g_clear_object (&tool->selection);
}

static void
gp_rectangle_selection_tool_class_init (GPRectangleSelectionToolClass *klass)
{
    GObjectClass * gobject_class = G_OBJECT_CLASS (klass);
    GPToolClass *tool_class = GP_TOOL_CLASS (klass);
    GPShapeToolClass *shape_tool_class = GP_SHAPE_TOOL_CLASS (klass);

    gobject_class->finalize = gp_rectangle_selection_tool_finalize;

    tool_class->create_icon = gp_line_tool_create_icon;
    tool_class->button_press = gp_rectangle_selection_tool_button_press;
    tool_class->button_release = gp_rectangle_selection_tool_button_release;
    tool_class->move = gp_rectangle_selection_tool_move;
    tool_class->deactivate = gp_rectangle_selection_tool_deactivate;

    shape_tool_class->draw_shape = gp_rectangle_selection_tool_draw;
}

GPTool*
gp_rectangle_selection_tool_create ()
{
    return  GP_TOOL (g_object_new (GP_TYPE_RECTANGLE_SELECTION_TOOL, NULL));
}
