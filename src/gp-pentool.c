/* gp-pentool.c
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

#include "gp-pentool.h"
#include "gp-tool-priv.h"

#include "gp-documentmanager.h"
#include "gp-colormanager.h"
#include "gp-cairoutils.h"

struct _GPPenTool
{
    GPTool parent_instance;

    gboolean grabbed;
    guint trigger_button;
    GdkPointD prev_point;

    GPtrArray *properties;
};

G_DEFINE_TYPE (GPPenTool, gp_pen_tool, GP_TYPE_TOOL)

static void
_gp_pen_tool_draw (GPPenTool *pen_tool, cairo_surface_t *surface, GdkPointD pos)
{
    cairo_t *cairo_context = cairo_create (surface);

    _gp_tool_load_drawing_color (pen_tool->trigger_button, cairo_context);

    cairo_set_antialias (cairo_context, CAIRO_ANTIALIAS_NONE);
    cairo_set_line_width (cairo_context, 1.0);

    cairo_move_to (cairo_context, pen_tool->prev_point.x, pen_tool->prev_point.y);
    cairo_line_to (cairo_context, pos.x, pos.y);
    cairo_stroke (cairo_context);

    cairo_destroy (cairo_context);
}

static void
gp_pen_tool_button_press (GPTool         *tool,
                          GdkEventButton *event,
                          GdkPointD       pos)
{
    GPPenTool *pen_tool = GP_PEN_TOOL (tool);
    GPDocument *document;

    if (pen_tool->grabbed && pen_tool->trigger_button != event->button)
    {
        pen_tool->grabbed = FALSE;
        _gp_tool_clear_tool_layer (NULL); // TODO not null!
        return;
    }

    pen_tool->grabbed = TRUE;
    pen_tool->trigger_button = event->button;

    document = gp_document_manager_get_active_document (gp_document_manager_get_default ());
    pen_tool->prev_point = pos;
    pos.x += 0.5; pos.y += 0.5;
    _gp_pen_tool_draw (pen_tool, gp_document_get_tool_surface (document), pos);
    gp_document_request_update_view (document, NULL); // TODO not null, bounding rect!
}

static void
gp_pen_tool_button_release (GPTool         *tool,
                            GdkEventButton *event,
                            GdkPointD       pos)
{
    GPPenTool *pen_tool = GP_PEN_TOOL (tool);
    GPDocument *document;

    if (!pen_tool->grabbed)
    {
        return;
    }

    document = gp_document_manager_get_active_document (gp_document_manager_get_default ());

    // TODO not 0, 0!
    gp_cairo_repaint_surface (gp_document_get_tool_surface (document), gp_document_get_surface (document), 0, 0);

    pen_tool->grabbed = FALSE;
}

static void
gp_pen_tool_move (GPTool         *tool,
                  GdkEventMotion *event,
                  GdkPointD       pos)
{
    GPPenTool *pen_tool = GP_PEN_TOOL (tool);

    if (pen_tool->grabbed)
    {
        GPDocument *document = gp_document_manager_get_active_document (gp_document_manager_get_default ());
        cairo_surface_t *tool_surface = gp_document_get_tool_surface (document);

        _gp_pen_tool_draw (pen_tool, tool_surface, pos);
        gp_document_request_update_view (document, NULL); // TODO not null, bounding rect!
        pen_tool->prev_point = pos;
    }
}

static void
gp_pen_tool_activate (GPTool *tool)
{
    GP_PEN_TOOL (tool)->grabbed = FALSE;
}

static void
gp_pen_tool_deactivate (GPTool *tool)
{
    GP_PEN_TOOL (tool)->grabbed = FALSE;
}

static GtkWidget*
gp_pen_tool_create_icon (GPTool *tool)
{
    return gtk_image_new_from_resource ("/org/gnome/Paint/toolicons/pen.png");
}

static const GPtrArray*
gp_pen_tool_get_properties (GPTool *tool)
{
    return GP_PEN_TOOL (tool)->properties;
}

static void
gp_pen_tool_free_property (gpointer data)
{
    g_object_unref (G_OBJECT (data));
}

static void
gp_pen_tool_finalize (GObject *gobj)
{
    GPPenTool *pen_tool = GP_PEN_TOOL (gobj);

    g_ptr_array_free (pen_tool->properties, TRUE);
    pen_tool->properties = NULL;
}

static void
gp_pen_tool_init (GPPenTool *self)
{
    self->properties = g_ptr_array_new_with_free_func (gp_pen_tool_free_property);
    self->grabbed = TRUE;
}

static void
gp_pen_tool_class_init (GPPenToolClass *klass)
{
    GPToolClass *tool_class = GP_TOOL_CLASS (klass);
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    tool_class->create_icon = gp_pen_tool_create_icon;
    tool_class->get_properties = gp_pen_tool_get_properties;
    tool_class->button_press = gp_pen_tool_button_press;
    tool_class->button_release = gp_pen_tool_button_release;
    tool_class->activate = gp_pen_tool_activate;
    tool_class->deactivate = gp_pen_tool_deactivate;
    tool_class->move = gp_pen_tool_move;

    gobject_class->finalize = gp_pen_tool_finalize;
}

GPTool*
gp_pen_tool_create ()
{
    return  GP_TOOL (g_object_new (GP_TYPE_PEN_TOOL, NULL));
}
