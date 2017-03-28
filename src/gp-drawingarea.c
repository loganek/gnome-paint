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

static guint gp_drawing_area_signals[LAST_SIGNAL] = { 0 };

struct _GPDrawingArea
{
    /*< private >*/
    GtkDrawingArea parent_instance;
};

typedef struct
{
    cairo_surface_t *active_surface;
    cairo_surface_t *base_surface;
} GPDrawingAreaPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GPDrawingArea, gp_drawing_area, GTK_TYPE_DRAWING_AREA)

static void
clear_surface (cairo_surface_t *surface)
{
    cairo_t *cr = cairo_create (surface);

    cairo_set_source_rgb (cr, 1, 1, 1);
    cairo_paint (cr);

    cairo_destroy (cr);
}

static void
repaint_surface (cairo_surface_t *dest_surface, cairo_surface_t *src_surface)
{
    cairo_t *cr = cairo_create (dest_surface);

    cairo_set_source_surface (cr, src_surface, 0, 0);
    cairo_paint (cr);

    cairo_destroy (cr);
}

static void
init_surface (cairo_surface_t **surface, GtkWidget *widget)
{
    cairo_surface_t *tmp_surface = gdk_window_create_similar_surface (gtk_widget_get_window (widget),
                                                                      CAIRO_CONTENT_COLOR,
                                                                      gtk_widget_get_allocated_width (widget),
                                                                      gtk_widget_get_allocated_height (widget));

    clear_surface (tmp_surface);

    if (*surface)
    {
        repaint_surface (tmp_surface, *surface);
        cairo_surface_destroy (*surface);
    }

    *surface = tmp_surface;
}

static gboolean
on_gp_drawing_area_draw (GtkWidget *widget,
                         cairo_t   *cr)
{
    GPDrawingAreaPrivate *priv = gp_drawing_area_get_instance_private (GP_DRAWING_AREA (widget));

    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_source_surface (cr, priv->base_surface, 0, 0);
    cairo_paint (cr);

    g_signal_emit (widget, gp_drawing_area_signals[SIGNAL_DRAW_OVERLAY], 0, cr, NULL);

    return FALSE;
}

static gboolean
on_gp_drawing_area_configure_event (GtkWidget         *widget,
                                    GdkEventConfigure *event,
                                    gpointer           data)
{
    GPDrawingAreaPrivate *priv = gp_drawing_area_get_instance_private (GP_DRAWING_AREA (widget));

    init_surface (&priv->base_surface, widget);
    init_surface (&priv->active_surface, widget);

    return TRUE;
}

static void
gp_drawing_area_init (GPDrawingArea *drawing_area)
{
    gtk_widget_init_template (GTK_WIDGET (drawing_area));

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

    widget_class->draw = on_gp_drawing_area_draw;

    gtk_widget_class_set_template_from_resource (widget_class,
                                                 "/org/gnome/Paint/gp-drawingarea.ui");
    gtk_widget_class_bind_template_callback (widget_class,
                                             on_gp_drawing_area_configure_event);

    gp_drawing_area_signals[SIGNAL_DRAW_OVERLAY] =
            g_signal_new ("draw-overlay",
                          G_TYPE_FROM_CLASS (klass),
                          0,
                          0,
                          NULL,
                          NULL,
                          gp_VOID__BOXED,
                          G_TYPE_NONE, 1, CAIRO_GOBJECT_TYPE_CONTEXT);
}

GtkWidget *
gp_drawing_area_new (void)
{
    return g_object_new (GP_TYPE_DRAWING_AREA, NULL);
}

cairo_surface_t*
gp_drawing_area_get_surface (GPDrawingArea *drawing_area)
{
    GPDrawingAreaPrivate *priv = gp_drawing_area_get_instance_private (drawing_area);

    return priv->base_surface;
}

void
gp_drawing_area_load_from_pixbuf (GPDrawingArea *drawing_area, GdkPixbuf *pixbuf)
{
    GPDrawingAreaPrivate *priv = gp_drawing_area_get_instance_private (drawing_area);

    priv->base_surface = gdk_cairo_surface_create_from_pixbuf (pixbuf, 0,
                                                               gtk_widget_get_window (GTK_WIDGET (drawing_area)));
}
