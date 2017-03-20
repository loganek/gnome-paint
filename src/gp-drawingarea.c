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

struct _GPDrawingArea
{
    /*< private >*/
    GtkDrawingArea parent_instance;
};

typedef struct
{
    GPTool *tool;
    gboolean is_drawing;
    cairo_surface_t *active_surface;
    cairo_surface_t *base_surface;
    GdkPoint start_point;
    GdkPoint current_point;
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

    if (priv->is_drawing == FALSE)
    {
        return;
    }

    if (priv->tool == NULL)
    {
        g_error ("Tool expected");
        return;
    }

    cr = cairo_create (priv->active_surface);

    priv->tool->draw (priv->tool, priv->start_point, priv->current_point, cr);

    cairo_destroy (cr);

    gp_drawing_area_queue_draw (drawing_area);
}

static gboolean
on_gp_drawing_area_draw (GtkWidget *widget,
                         cairo_t   *cr,
                         gpointer   data)
{
    GPDrawingAreaPrivate *priv = gp_drawing_area_get_instance_private (GP_DRAWING_AREA (widget));

    cairo_set_source_surface (cr, priv->active_surface, 0, 0);
    cairo_paint (cr);

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

static gboolean
on_gp_drawing_area_button_press_event (GtkWidget      *widget,
                                       GdkEventButton *event,
                                       gpointer        user_data)
{
    GPDrawingAreaPrivate *priv = gp_drawing_area_get_instance_private (GP_DRAWING_AREA (widget));

    if (event->button == GDK_BUTTON_PRIMARY)
    {
        priv->is_drawing = TRUE;

        priv->current_point.x = (gint) event->x;
        priv->current_point.y = (gint) event->y;
        priv->start_point = priv->current_point;

        repaint_surface (priv->active_surface, priv->base_surface);

	gp_drawing_area_draw (GP_DRAWING_AREA (widget));
    }
    else if (event->button == GDK_BUTTON_SECONDARY && priv->is_drawing == TRUE)
    {
        repaint_surface (priv->active_surface, priv->base_surface);
        gp_drawing_area_queue_draw (GP_DRAWING_AREA (widget));
        priv->is_drawing = FALSE;
    }
    return TRUE;
}

static gboolean
on_gp_drawing_area_button_release_event (GtkWidget      *widget,
                                       GdkEventButton *event,
                                       gpointer        user_data)
{
    GPDrawingAreaPrivate *priv = gp_drawing_area_get_instance_private (GP_DRAWING_AREA (widget));

    if (event->button == GDK_BUTTON_PRIMARY && priv->is_drawing == TRUE)
    {
        repaint_surface (priv->base_surface, priv->active_surface);
        priv->is_drawing = FALSE;
    }
}

static gboolean
on_gp_drawing_area_motion_notify_event (GtkWidget      *widget,
                                        GdkEventMotion *event,
                                        gpointer        data)
{
    GPDrawingAreaPrivate *priv = gp_drawing_area_get_instance_private (GP_DRAWING_AREA (widget));

    if (event->state & GDK_BUTTON1_MASK && priv->is_drawing == TRUE)
    {
        priv->current_point.x = (gint) event->x;
        priv->current_point.y = (gint) event->y;

        repaint_surface (priv->active_surface, priv->base_surface);

        gp_drawing_area_draw (GP_DRAWING_AREA (widget));
    }

    return TRUE;
}

gboolean
on_gp_drawing_area_enter_notify_event (GtkWidget *widget,
                                       GdkEvent  *event,
                                       gpointer   user_data)
{
    GdkWindow *window = gtk_widget_get_window (widget);

    gdk_window_set_cursor (window, gdk_cursor_new_for_display (gdk_display_get_default(), GDK_CROSSHAIR));

    return TRUE;
}

static void
gp_drawing_area_init (GPDrawingArea *drawing_area)
{
    GPDrawingAreaPrivate *priv = gp_drawing_area_get_instance_private (drawing_area);
    priv->tool = NULL;
    priv->is_drawing = FALSE;

    gtk_widget_add_events (GTK_WIDGET (drawing_area),
			   GDK_BUTTON_PRESS_MASK
			   | GDK_BUTTON_RELEASE_MASK
			   | GDK_POINTER_MOTION_MASK
			   | GDK_ENTER_NOTIFY_MASK);

    g_signal_connect (drawing_area, "button-press-event",
		      G_CALLBACK(on_gp_drawing_area_button_press_event), NULL);
    g_signal_connect (drawing_area, "button-release-event",
		      G_CALLBACK(on_gp_drawing_area_button_release_event), NULL);
    g_signal_connect (drawing_area, "motion-notify-event",
                      G_CALLBACK (on_gp_drawing_area_motion_notify_event), NULL);
    g_signal_connect (drawing_area, "enter-notify-event",
                      G_CALLBACK (on_gp_drawing_area_enter_notify_event), NULL);
    g_signal_connect (drawing_area,"configure-event",
                      G_CALLBACK (on_gp_drawing_area_configure_event), NULL);
    g_signal_connect (drawing_area, "draw",
                      G_CALLBACK (on_gp_drawing_area_draw), NULL);
}

static void
gp_drawing_area_class_init (GPDrawingAreaClass *klass)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    gtk_widget_class_set_template_from_resource (widget_class,
                                                 "/org/gnome/Paint/gp-drawingarea.ui");
}

void
gp_drawing_area_set_tool (GPDrawingArea *drawing_area, GPTool *tool)
{
    GPDrawingAreaPrivate *priv = gp_drawing_area_get_instance_private (drawing_area);

    priv->tool = tool;
}

GtkWidget *
gp_drawing_area_new (void)
{
    return g_object_new (GP_TYPE_DRAWING_AREA, NULL);
}

