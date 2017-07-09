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
#include "gp-marshal.h"
#include "gp-toolmanager.h"

#include <glib/gi18n.h>

#define RESIZE_MARGIN 15

/* Signals */
enum
{
    CANVAS_CHANGED,
    LAST_SIGNAL
};

static guint gp_image_editor_signals[LAST_SIGNAL] = { 0 };

struct _GPImageEditor
{
    /*< private >*/
    GtkFixed parent_instance;
};

typedef struct
{
    GPTool *tool;
    GdkRGBA fg_color;
    GdkRGBA bg_color;
    GtkEventBox *resizer;
    GPDrawingArea *canvas;
    GPDocument *document;
    gulong update_view_id;
} GPImageEditorPrivate;

#define GP_IMAGE_EDITOR_PRIV(image_editor) ((GPImageEditorPrivate *) gp_image_editor_get_instance_private (image_editor))

G_DEFINE_TYPE_WITH_PRIVATE (GPImageEditor, gp_image_editor, GTK_TYPE_FIXED)

static void
gp_image_editor_document_view_updated (GPDocument           *document,
                                       const GdkRectangle   *bounding_box,
                                       GPImageEditorPrivate *priv)
{
    if (bounding_box == NULL)
    {
        gtk_widget_queue_draw (GTK_WIDGET (priv->canvas));
    }
    else
    {
        gtk_widget_queue_draw_area (GTK_WIDGET (priv->canvas),
                                    bounding_box->x,
                                    bounding_box->y,
                                    bounding_box->x + bounding_box->width,
                                    bounding_box->y + bounding_box->height);
    }
}

static gboolean
on_canvas_button_press_event (GtkWidget      *widget,
                              GdkEventButton *event,
                              gpointer        user_data)
{
    GPImageEditorPrivate *priv = GP_IMAGE_EDITOR_PRIV (GP_IMAGE_EDITOR (user_data));
    GdkPointD pt = { event->x, event->y };
    gp_tool_button_press (priv->tool, event, pt);

    return TRUE;
}

static gboolean
on_canvas_button_release_event (GtkWidget      *widget,
                                GdkEventButton *event,
                                gpointer        user_data)
{
    GPImageEditorPrivate *priv = GP_IMAGE_EDITOR_PRIV (GP_IMAGE_EDITOR (user_data));
    GdkPointD pt = { event->x, event->y };

    gp_tool_button_release (priv->tool, event, pt);

     g_signal_emit (user_data, gp_image_editor_signals[CANVAS_CHANGED], 0);

    return TRUE;
}

static gboolean
on_canvas_motion_notify_event (GtkWidget      *widget,
                               GdkEventMotion *event,
                               gpointer        user_data)
{
    GPImageEditorPrivate *priv = GP_IMAGE_EDITOR_PRIV (GP_IMAGE_EDITOR (user_data));
    GdkPointD pt = { event->x, event->y };

    g_return_val_if_fail (priv->tool != NULL, TRUE);

    gp_tool_move (priv->tool, event, pt);

    return TRUE;
}

static void
on_tool_changed (GPToolManager *manager, gpointer user_data)
{
    gp_image_editor_set_tool (GP_IMAGE_EDITOR (user_data), gp_tool_manager_get_active_tool (manager));
}

static void
gp_image_editor_init (GPImageEditor *self)
{
    GPImageEditorPrivate *priv = GP_IMAGE_EDITOR_PRIV (self);
    GdkRGBA bg_color = { 1.0, 1.0, 1.0, 1.0 };
    gtk_widget_init_template (GTK_WIDGET (self));

    priv->tool = NULL;
    g_signal_connect (gp_tool_manager_default (), "active-tool-changed", G_CALLBACK (on_tool_changed), self);

    priv->bg_color = bg_color; // todo set from application

    priv->update_view_id = 0;

    gtk_widget_set_margin_bottom (GTK_WIDGET (priv->canvas), RESIZE_MARGIN);
    gtk_widget_set_margin_end (GTK_WIDGET (priv->canvas), RESIZE_MARGIN);

    priv->document = NULL;

    gtk_widget_add_events (GTK_WIDGET (priv->resizer),
                           GDK_BUTTON_PRESS_MASK
                           | GDK_BUTTON_RELEASE_MASK
                           | GDK_POINTER_MOTION_MASK);

    gtk_widget_set_size_request (GTK_WIDGET (priv->resizer), 300, 300);
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
                                             on_canvas_button_press_event);
    gtk_widget_class_bind_template_callback (widget_class,
                                             on_canvas_button_release_event);
    gtk_widget_class_bind_template_callback (widget_class,
                                             on_canvas_motion_notify_event);

    gp_image_editor_signals[CANVAS_CHANGED] = g_signal_new ("canvas-changed",
                                                            G_TYPE_FROM_CLASS (G_OBJECT_CLASS (klass)),
                                                            G_SIGNAL_RUN_FIRST,
                                                            0,
                                                            NULL, NULL, NULL,
                                                            G_TYPE_NONE, 0);
}

void
gp_image_editor_set_tool (GPImageEditor *image_editor, GPTool *tool)
{
    GPImageEditorPrivate *priv = GP_IMAGE_EDITOR_PRIV (image_editor);

    if (priv->tool != NULL)
    {
        gp_tool_deactivate (priv->tool);
        g_signal_emit (image_editor, gp_image_editor_signals[CANVAS_CHANGED], 0);

        g_clear_object (&priv->tool);
    }

    priv->tool = g_object_ref (tool);
    gp_tool_set_canvas_widget (tool, GTK_WIDGET (priv->canvas));
    gp_tool_activate (tool);
}

void
gp_image_editor_set_color (GPImageEditor *image_editor, const GdkRGBA *color)
{
    GPImageEditorPrivate *priv = GP_IMAGE_EDITOR_PRIV (image_editor);

    priv->fg_color = *color;
}

GtkWidget *
gp_image_editor_new (void)
{
    return g_object_new (GP_TYPE_IMAGE_EDITOR, NULL);
}

void
gp_image_editor_set_document (GPImageEditor *image_editor, GPDocument *document)
{
    GPImageEditorPrivate *priv = GP_IMAGE_EDITOR_PRIV (image_editor);
    GPSize doc_size;

    if (priv->document && priv->update_view_id)
    {
        g_signal_handler_disconnect (priv->document, priv->update_view_id);
    }

    g_signal_connect (document, "view-updated", G_CALLBACK (gp_image_editor_document_view_updated), priv);
    g_clear_object (&priv->document);
    priv->document = g_object_ref (document);

    doc_size = gp_document_get_size (document);

    gp_drawing_area_set_document (priv->canvas, document);

    gtk_widget_set_size_request (GTK_WIDGET (priv->resizer), doc_size.width + RESIZE_MARGIN, doc_size.height + RESIZE_MARGIN);
}

void
gp_image_editor_external_modification (GPImageEditor *image_editor)
{
    g_signal_emit (image_editor, gp_image_editor_signals[CANVAS_CHANGED], 0);
}
