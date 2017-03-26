/* gp-imageeditor.c
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

#include "gp-imageeditor.h"
#include "gp-drawingarea.h"

#define RESIZE_MARGIN 15

typedef enum
{
    GP_RESIZE_MODE_NONE = 0,
    GP_RESIZE_MODE_X = 1,
    GP_RESIZE_MODE_Y = 2,
    GP_RESIZE_MODE_XY = 3,
    GP_RESIZE_MODE_ENABLED = 8
} GPResizeMode;

struct _GPImageEditor
{
    /*< private >*/
    GtkFixed parent_instance;
};

typedef struct
{
    GPTool *tool;
    GPResizeMode resize_mode;
    GtkEventBox *resizer;
    GtkDrawingArea *canvas;
} GPImageEditorPrivate;

#define GP_IMAGE_EDITOR_PRIV(image_editor) ((GPImageEditorPrivate *) gp_image_editor_get_instance_private (image_editor))

G_DEFINE_TYPE_WITH_PRIVATE (GPImageEditor, gp_image_editor, GTK_TYPE_FIXED)

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
static gboolean
on_resizer_button_press_event (GtkWidget      *widget,
                               GdkEventButton *event,
                               gpointer        user_data)
{
    GPImageEditorPrivate *priv = GP_IMAGE_EDITOR_PRIV (GP_IMAGE_EDITOR (user_data));

    priv->resize_mode = calculate_resize_mode (GTK_WIDGET (priv->resizer), event->x, event->y);

    return TRUE;
}

static gboolean
on_resizer_button_release_event (GtkWidget      *widget,
                                 GdkEventButton *event,
                                 gpointer        user_data)
{
    GPImageEditor *self = GP_IMAGE_EDITOR (user_data);
    GPImageEditorPrivate *priv = GP_IMAGE_EDITOR_PRIV (self);

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
                                gpointer        user_data)
{
    GPImageEditor *self = GP_IMAGE_EDITOR (user_data);
    GPImageEditorPrivate *priv = GP_IMAGE_EDITOR_PRIV (self);

    GdkCursorType cursor_type = GDK_LAST_CURSOR;

    if (priv->resize_mode & GP_RESIZE_MODE_ENABLED)
    {
        GPResizeMode resize_mode = calculate_resize_mode (GTK_WIDGET (priv->resizer), event->x, event->y);

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
            g_assert_not_reached ();
        }

        gdk_window_set_cursor (gtk_widget_get_window (GTK_WIDGET (priv->resizer)),
                               gdk_cursor_new_for_display (gdk_display_get_default(), cursor_type));
    }

    return FALSE;
}

static gboolean
on_resizer_enter_notify_event (GtkWidget *widget,
                               GdkEvent  *event,
                               gpointer   user_data)
{
    GP_IMAGE_EDITOR_PRIV (GP_IMAGE_EDITOR (user_data))->resize_mode |= GP_RESIZE_MODE_ENABLED;

    return FALSE;
}

static gboolean
on_resizer_leave_notify_event (GtkWidget *widget,
                               GdkEvent  *event,
                               gpointer   user_data)
{
    GP_IMAGE_EDITOR_PRIV (GP_IMAGE_EDITOR (user_data))->resize_mode &= ~GP_RESIZE_MODE_ENABLED;

    return TRUE;
}


static gboolean
on_canvas_button_press_event (GtkWidget      *widget,
                              GdkEventButton *event,
                              gpointer        user_data)
{
    GPImageEditorPrivate *priv = GP_IMAGE_EDITOR_PRIV (GP_IMAGE_EDITOR (user_data));

    gp_tool_button_press (priv->tool, event);

    return TRUE;
}

static gboolean
on_canvas_button_release_event (GtkWidget      *widget,
                                GdkEventButton *event,
                                gpointer        user_data)
{
    GPImageEditorPrivate *priv = GP_IMAGE_EDITOR_PRIV (GP_IMAGE_EDITOR (user_data));
    cairo_t *cr = cairo_create (gp_drawing_area_get_surface (GP_DRAWING_AREA (widget)));

    gp_tool_button_release (priv->tool, event, cr);

    cairo_destroy (cr);

    return TRUE;
}

static void
on_canvas_draw_overlay (GtkWidget *widget,
                        cairo_t   *cr,
                        gpointer   user_data)
{
    GPImageEditorPrivate *priv = GP_IMAGE_EDITOR_PRIV (GP_IMAGE_EDITOR (user_data));

    cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 1.0);
    gp_tool_draw (priv->tool, cr);
}

static gboolean
on_canvas_motion_notify_event (GtkWidget      *widget,
                               GdkEventMotion *event,
                               gpointer        user_data)
{
    GPImageEditorPrivate *priv = GP_IMAGE_EDITOR_PRIV (GP_IMAGE_EDITOR (user_data));

    gp_tool_move (priv->tool, event);

    return TRUE;
}

static void
gp_image_editor_init (GPImageEditor *self)
{
    GPImageEditorPrivate *priv = GP_IMAGE_EDITOR_PRIV (self);

    gtk_widget_init_template (GTK_WIDGET (self));

    priv->tool = NULL;

    g_signal_connect (priv->canvas,
                      "draw-overlay",
                      G_CALLBACK (on_canvas_draw_overlay),
                      self);

    gtk_widget_add_events (GTK_WIDGET (priv->resizer),
                           GDK_BUTTON_PRESS_MASK
                           | GDK_BUTTON_RELEASE_MASK
                           | GDK_POINTER_MOTION_MASK);
    gtk_widget_add_events (GTK_WIDGET (priv->canvas),
                           GDK_BUTTON_PRESS_MASK
                           | GDK_BUTTON_RELEASE_MASK
                           | GDK_POINTER_MOTION_MASK);
}

static void
gp_image_editor_class_init (GPImageEditorClass *klass)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    gtk_widget_class_set_template_from_resource (widget_class,
                                                 "/org/gnome/Paint/gp-imageeditor.ui");
    gtk_widget_class_bind_template_child_private (widget_class, GPImageEditor,
                                                  resizer);
    gtk_widget_class_bind_template_child_private (widget_class, GPImageEditor,
                                                  canvas);
    gtk_widget_class_bind_template_callback (widget_class,
                                             on_resizer_button_press_event);
    gtk_widget_class_bind_template_callback (widget_class,
                                             on_resizer_button_release_event);
    gtk_widget_class_bind_template_callback (widget_class,
                                             on_resizer_motion_notify_event);
    gtk_widget_class_bind_template_callback (widget_class,
                                             on_resizer_enter_notify_event);
    gtk_widget_class_bind_template_callback (widget_class,
                                             on_resizer_leave_notify_event);
    gtk_widget_class_bind_template_callback (widget_class,
                                             on_canvas_button_press_event);
    gtk_widget_class_bind_template_callback (widget_class,
                                             on_canvas_button_release_event);
    gtk_widget_class_bind_template_callback (widget_class,
                                             on_canvas_motion_notify_event);

}

void
gp_image_editor_set_tool (GPImageEditor *image_editor, GPTool *tool)
{
    GPImageEditorPrivate *priv = GP_IMAGE_EDITOR_PRIV (image_editor);

    gp_tool_set_canvas_widget (tool, GTK_WIDGET (priv->canvas));
    priv->tool = tool;

    // TODO set cursor on show widget
    gdk_window_set_cursor (gtk_widget_get_window (GTK_WIDGET (priv->canvas)), gdk_cursor_new_for_display (gdk_display_get_default(), GDK_CROSSHAIR));
}

GtkWidget *
gp_image_editor_new (void)
{
    return g_object_new (GP_TYPE_IMAGE_EDITOR, NULL);
}

