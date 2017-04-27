/* gp-drawingarea.c
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

#include "config.h"

#include "gp-marshal.h"
#include "gp-drawingarea.h"

#include <cairo-gobject.h>

enum
{
    SIGNAL_DRAW_OVERLAY,
    LAST_SIGNAL
};

struct _GPDrawingArea
{
    /*< private >*/
    GtkDrawingArea parent_instance;

    GPDocument *document;
    cairo_surface_t *surface;
};

G_DEFINE_TYPE (GPDrawingArea, gp_drawing_area, GTK_TYPE_DRAWING_AREA)

static void
gp_drawing_area_render (cairo_surface_t *source, cairo_surface_t *destination)
{
    cairo_t *cr;

    if (source == NULL || destination == NULL)
    {
        return;
    }

    cr = cairo_create (destination);

    cairo_set_source_surface (cr, source, 0, 0);
    cairo_paint (cr);

    cairo_destroy (cr);

    return;
}

static void
gp_drawing_area_adjust_surface (GPDrawingArea *self)
{
    cairo_surface_t *doc_surface = gp_document_get_surface(self->document);
    gint doc_width = cairo_image_surface_get_width (doc_surface);
    gint doc_height = cairo_image_surface_get_height (doc_surface);
    gboolean create_surface = FALSE;

    if (self->surface == NULL)
    {
        create_surface = TRUE;
    }
    else
    {
        gint width = cairo_image_surface_get_width (self->surface);
        gint height = cairo_image_surface_get_height (self->surface);

        if (width != doc_width || height != doc_height)
        {
            create_surface = TRUE;
            cairo_surface_destroy (self->surface);
        }
    }

    if (create_surface)
    {
        self->surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, doc_width, doc_height);
    }
}

static gboolean
on_gp_drawing_area_draw (GtkWidget *widget,
                         cairo_t   *cr)
{
    GPDrawingArea *self = GP_DRAWING_AREA (widget);

    if (self->document == NULL)
    {
        return FALSE;
    }
    gp_drawing_area_adjust_surface (self);
    gp_drawing_area_render (gp_document_get_surface (self->document), self->surface);
    gp_drawing_area_render (gp_document_get_tool_surface (self->document), self->surface);

    cairo_surface_mark_dirty (self->surface);
    cairo_set_source_surface (cr, self->surface, 0, 0);
    cairo_paint(cr);

    return FALSE;
}

static void
gp_drawing_area_finalize (GObject *gobj)
{
    GPDrawingArea *self = GP_DRAWING_AREA (gobj);

    g_clear_object (&self->document);

    if (self->surface)
    {
        cairo_surface_destroy (self->surface);
    }
}

static void
gp_drawing_area_init (GPDrawingArea *drawing_area)
{
    gtk_widget_add_events (GTK_WIDGET (drawing_area),
                           GDK_BUTTON_PRESS_MASK
                           | GDK_BUTTON_RELEASE_MASK
                           | GDK_POINTER_MOTION_MASK
                           | GDK_ENTER_NOTIFY_MASK);
}

static void
gp_drawing_area_class_init (GPDrawingAreaClass *klass)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    widget_class->draw = on_gp_drawing_area_draw;

    gobject_class->finalize = gp_drawing_area_finalize;
}

GtkWidget *
gp_drawing_area_new (void)
{
    return g_object_new (GP_TYPE_DRAWING_AREA, NULL);
}

void
gp_drawing_area_set_document (GPDrawingArea *drawing_area, GPDocument *document)
{
    g_clear_object (&drawing_area->document);

    drawing_area->document = g_object_ref (document);
}
