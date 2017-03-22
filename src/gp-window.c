/* gp-window.c
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

#include "gp-window.h"

#include "gp-drawingarea.h"
#include "gp-toolbox.h"
#include "gp-linetool.h" // TODO remove

#include <glib/gi18n.h>

struct _GPWindow
{
    /*< private >*/
    GtkApplicationWindow parent_instance;
};

#define RESIZE_MARGIN 5

typedef enum
{
    GP_RESIZE_MODE_NONE = 0,
    GP_RESIZE_MODE_X = 1,
    GP_RESIZE_MODE_Y = 2,
    GP_RESIZE_MODE_XY = 3
} GPResizeMode;

typedef struct
{
    GtkFrame* main_frame;
    GPDrawingArea* drawing_area;
    GPToolBox* tool_box;
    GtkEventBox *resizer;
    GPResizeMode resize_mode;
} GPWindowPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GPWindow, gp_window, GTK_TYPE_APPLICATION_WINDOW)

static GPResizeMode
calculate_resize_mode (GtkWidget *widget, gint pos_x, gint pos_y)
{
    GPResizeMode resize_mode = GP_RESIZE_MODE_NONE;

    if (gtk_widget_get_allocated_width (widget) - RESIZE_MARGIN < pos_x)
    {
        resize_mode |= GP_RESIZE_MODE_X;
    }
    if (gtk_widget_get_allocated_height (widget) - RESIZE_MARGIN < pos_y)
    {
        resize_mode |= GP_RESIZE_MODE_Y;
    }

    return resize_mode;
}

static void
on_tool_changed (GtkWidget *widget, gpointer user_data)
{
    GPWindowPrivate *priv = gp_window_get_instance_private (GP_WINDOW (user_data));

    gp_drawing_area_set_tool (priv->drawing_area, gp_tool_box_get_active_tool (priv->tool_box));
}

static gboolean
on_resizer_button_press_event (GtkWidget      *widget,
                               GdkEventButton *event,
                               gpointer        user_data)
{
    GPWindowPrivate *priv = gp_window_get_instance_private (GP_WINDOW (user_data));

    priv->resize_mode = calculate_resize_mode (GTK_WIDGET (priv->drawing_area), event->x, event->y);

    return TRUE;
}

static gboolean
on_resizer_button_release_event (GtkWidget      *widget,
                                 GdkEventButton *event,
                                 gpointer        user_data)
{
    GPWindowPrivate *priv = gp_window_get_instance_private (GP_WINDOW (user_data));

    if (priv->resize_mode)
    {
        gint new_x = priv->resize_mode & GP_RESIZE_MODE_X ? MAX(1, event->x) : gtk_widget_get_allocated_width (widget);
        gint new_y = priv->resize_mode & GP_RESIZE_MODE_Y ? MAX(1, event->y) : gtk_widget_get_allocated_height (widget);

        gtk_widget_set_size_request (GTK_WIDGET (widget), new_x, new_y);
        priv->resize_mode = GP_RESIZE_MODE_NONE;
    }

    return TRUE;
}

static gboolean
on_resizer_motion_notify_event (GtkWidget      *widget,
                                GdkEventMotion *event,
                                gpointer        data)
{
    GPWindowPrivate *priv = gp_window_get_instance_private (GP_WINDOW (data));
    GdkCursorType cursor_type = GDK_LAST_CURSOR;

    if (priv->resize_mode == GP_RESIZE_MODE_NONE)
    {
        GPResizeMode resize_mode = calculate_resize_mode (GTK_WIDGET (priv->drawing_area), event->x, event->y);

        switch (resize_mode)
        {
        case GP_RESIZE_MODE_X:
            cursor_type = GDK_RIGHT_SIDE;
            break;
        case GP_RESIZE_MODE_Y:
            cursor_type = GDK_BOTTOM_SIDE;
            break;
        case GP_RESIZE_MODE_XY:
            cursor_type = GDK_BOTTOM_RIGHT_CORNER;
            break;
        default:
            g_warning ("Invalid resize mode: %d", resize_mode);
        }

        gdk_window_set_cursor (gtk_widget_get_window (GTK_WIDGET (priv->resizer)),
	                       gdk_cursor_new_for_display (gdk_display_get_default(), cursor_type));
        gdk_window_set_cursor (gtk_widget_get_window (GTK_WIDGET (priv->drawing_area)),
	                       gdk_cursor_new_for_display (gdk_display_get_default(), cursor_type));
    }

    return FALSE;
}

static void
gp_window_class_init (GPWindowClass *klass)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    gtk_widget_class_set_template_from_resource (widget_class,
                                                 "/org/gnome/Paint/gp-window.ui");
    gtk_widget_class_bind_template_child_private (widget_class, GPWindow,
                                                  drawing_area);
    gtk_widget_class_bind_template_child_private (widget_class, GPWindow,
                                                  resizer);
    gtk_widget_class_bind_template_child_private (widget_class, GPWindow,
                                                  tool_box);
	/*
    gtk_widget_class_bind_template_child_private (widget_class, GlWindow,
                                                  event);
    gtk_widget_class_bind_template_child_private (widget_class, GlWindow,
                                                  info_bar);
*/
    gtk_widget_class_bind_template_callback (widget_class,
                                             on_resizer_button_press_event);
    gtk_widget_class_bind_template_callback (widget_class,
                                             on_resizer_button_release_event);
    gtk_widget_class_bind_template_callback (widget_class,
                                             on_resizer_motion_notify_event);
}

static void
gp_window_init (GPWindow *window)
{
    GPWindowPrivate* priv;
    gtk_widget_init_template (GTK_WIDGET (window));

    priv = gp_window_get_instance_private (window);

    gtk_widget_set_size_request (GTK_WIDGET (priv->drawing_area), 100, 100); // TODO

    gtk_widget_add_events (GTK_WIDGET (priv->resizer),
			   GDK_BUTTON_PRESS_MASK
			   | GDK_BUTTON_RELEASE_MASK
			   | GDK_POINTER_MOTION_MASK);

    g_signal_connect (priv->tool_box, "tool-changed", G_CALLBACK (on_tool_changed), window);

    gp_drawing_area_set_tool (priv->drawing_area, gp_tool_box_get_active_tool (priv->tool_box));
}

GtkWidget *
gp_window_new (GtkApplication *application)
{
    g_return_val_if_fail (GTK_APPLICATION (application), NULL);

    return g_object_new (GP_TYPE_WINDOW, "application", application, NULL);
}

