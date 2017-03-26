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

#include "gp-tool.h"
#include "gp-drawingarea.h"

#include <cairo-gobject.h>

enum
{
    SIGNAL_DRAW_OVERLAY,
    LAST_SIGNAL
};

#define g_marshal_value_peek_boxed(v)    (v)->data[0].v_pointer

void
gegl_gtk_marshal_VOID__BOXED (GClosure     *closure,
                                    GValue       *return_value G_GNUC_UNUSED,
                                    guint         n_param_values,
                                    const GValue *param_values,
                                    gpointer      invocation_hint G_GNUC_UNUSED,
                                    gpointer      marshal_data)
{
    typedef void (*GMarshalFunc_VOID__BOXED) (gpointer     data1,
                                                    gpointer     arg_1,
                                                    gpointer     data2);
    GMarshalFunc_VOID__BOXED callback;
    GCClosure *cc = (GCClosure*) closure;
    gpointer data1, data2;

    g_return_if_fail (n_param_values == 2);

    if (G_CCLOSURE_SWAP_DATA (closure))
    {
        data1 = closure->data;
        data2 = g_value_peek_pointer (param_values + 0);
    }
    else
    {
        data1 = g_value_peek_pointer (param_values + 0);
        data2 = closure->data;
    }
    callback = (GMarshalFunc_VOID__BOXED) (marshal_data ? marshal_data : cc->callback);

    callback (data1,
              g_marshal_value_peek_boxed (param_values + 1),
              data2);
}


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

static void
gp_drawing_area_queue_draw (GPDrawingArea *drawing_area)
{
    GtkWidget *widget = GTK_WIDGET (drawing_area);

    gtk_widget_queue_draw_area (
                widget,
                0, 0,
                gtk_widget_get_allocated_width (widget),
                gtk_widget_get_allocated_height (widget));
}

static void
gp_drawing_area_draw (GPDrawingArea *drawing_area)
{
    GPDrawingAreaPrivate *priv = gp_drawing_area_get_instance_private (drawing_area);
    cairo_t *cr;


    cr = cairo_create (priv->active_surface);


    cairo_destroy (cr);

    gp_drawing_area_queue_draw (drawing_area);
}

static gboolean
on_gp_drawing_area_draw (GtkWidget *widget,
                         cairo_t   *cr,
                         gpointer   data)
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
    GPDrawingAreaPrivate *priv = gp_drawing_area_get_instance_private (drawing_area);
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
                          gegl_gtk_marshal_VOID__BOXED,
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
