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

#include "gp-toolbox.h"
#include "gp-imageeditor.h"
#include "gp-colorselectorbox.h"
#include "gp-headerbar.h"
#include "gp-documentinfo.h"

#include <glib/gi18n.h>

struct _GPWindow
{
    /*< private >*/
    GtkApplicationWindow parent_instance;
};

typedef struct
{
    GtkFrame *main_frame;
    GPImageEditor *image_editor;
    GPToolBox *tool_box;
    GPColorSelectorBox *color_selector_box;
    GPHeaderBar *header_bar;
    GPDocumentInfo *document_info;
} GPWindowPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GPWindow, gp_window, GTK_TYPE_APPLICATION_WINDOW)

static void
on_tool_changed (GtkWidget *widget, gpointer user_data)
{
    GPWindowPrivate *priv = gp_window_get_instance_private (GP_WINDOW (user_data));

    gp_image_editor_set_tool (priv->image_editor, gp_tool_box_get_active_tool (priv->tool_box));
}

static void
on_color_changed (GtkWidget *widget, gpointer user_data)
{
    GPWindowPrivate *priv = gp_window_get_instance_private (GP_WINDOW (user_data));

    GdkRGBA color;
    gp_color_selector_box_get_color (GP_COLOR_SELECTOR_BOX (widget), &color);
    gp_image_editor_set_color (priv->image_editor, &color);
}

static gchar*
gp_window_request_new_filename (GPWindow *window)
{
    return g_strdup ("/home/loganek/todo.png"); // TODO
}

static gboolean
gp_window_close_document (GPWindow *window)
{
    GPWindowPrivate *priv = gp_window_get_instance_private (window);
    GError *error = NULL;
    gboolean ret = TRUE;

    if (gp_document_info_is_modified (priv->document_info) == FALSE)
    {
        return TRUE;
    }

    gchar *filename = gp_document_info_get_filename (priv->document_info);

    if (filename == NULL)
    {
        filename = gp_window_request_new_filename (window);

        if (filename == NULL)
        {
            return FALSE;
        }
    }

    gp_image_editor_save_file (priv->image_editor, filename, &error);

    if (error == NULL)
    {
        gp_document_info_set_filename (priv->document_info, filename);
        ret = TRUE;
    }
    else
    {
        // TODO print error
        ret = FALSE;
    }

    g_free (filename);

    return ret;
}

static void
gp_window_open_file (GPWindow *window, const gchar *filename)
{
    GPWindowPrivate *priv = gp_window_get_instance_private (window);
    GError *error = NULL;
    GtkWidget *dialog = NULL;

    if (gp_window_close_document (window) == FALSE)
    {
        return;
    }

    gp_image_editor_open_file (priv->image_editor, filename, &error);

    if (error != NULL)
    {
        dialog = gtk_message_dialog_new (GTK_WINDOW (window),
                                         GTK_DIALOG_DESTROY_WITH_PARENT,
                                         GTK_MESSAGE_ERROR,
                                         GTK_BUTTONS_CLOSE,
                                         _("Error on opening file \"%s\": %s"),
                                         filename,
                                         error->message);
        g_error_free (error);
        gtk_dialog_run (GTK_DIALOG (dialog));
        gtk_widget_destroy (dialog);
    }
    else
    {
        gp_document_info_set_filename (priv->document_info, filename);
        gp_header_bar_set_filename (priv->header_bar, filename, FALSE);
    }
}

static void
on_try_file_open (GtkWidget *widget, gpointer user_data)
{
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    GtkWidget *dialog = gtk_file_chooser_dialog_new (_("Open File"),
                                                     GTK_WINDOW (user_data),
                                                     action,
                                                     _("_Cancel"),
                                                     GTK_RESPONSE_CANCEL,
                                                     _("_Open"),
                                                     GTK_RESPONSE_ACCEPT,
                                                     NULL);

    res = gtk_dialog_run (GTK_DIALOG (dialog));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
        char *filename = gtk_file_chooser_get_filename (chooser);

        gp_window_open_file (GP_WINDOW (user_data), filename);

        g_free (filename);
    }

    gtk_widget_destroy (dialog);
}

static void
gp_window_finalize (GObject *gobj)
{
    GPWindowPrivate *priv = gp_window_get_instance_private (GP_WINDOW (gobj));

    g_object_unref (G_OBJECT (priv->document_info));
}

static void
gp_window_class_init (GPWindowClass *klass)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    gtk_widget_class_set_template_from_resource (widget_class,
                                                 "/org/gnome/Paint/gp-window.ui");
    gtk_widget_class_bind_template_child_private (widget_class, GPWindow,
                                                  image_editor);
    gtk_widget_class_bind_template_child_private (widget_class, GPWindow,
                                                  tool_box);
    gtk_widget_class_bind_template_child_private (widget_class, GPWindow,
                                                  color_selector_box);
    gtk_widget_class_bind_template_child_private (widget_class, GPWindow,
                                                  header_bar);

    gobject_class->finalize = gp_window_finalize;
}

static void
gp_window_init (GPWindow *window)
{
    GPWindowPrivate* priv;
    gtk_widget_init_template (GTK_WIDGET (window));

    priv = gp_window_get_instance_private (window);

    priv->document_info = gp_document_info_create ();

    g_signal_connect (priv->tool_box, "tool-changed", G_CALLBACK (on_tool_changed), window);
    g_signal_connect (priv->color_selector_box, "color-changed", G_CALLBACK (on_color_changed), window);
    g_signal_connect (priv->header_bar, "try-file-open", G_CALLBACK (on_try_file_open), window);

    gp_image_editor_set_tool (priv->image_editor, gp_tool_box_get_active_tool (priv->tool_box));

    GdkRGBA color;
    gp_color_selector_box_get_color (priv->color_selector_box, &color);
    gp_image_editor_set_color (priv->image_editor, &color);
}

GtkWidget *
gp_window_new (GtkApplication *application)
{
    g_return_val_if_fail (GTK_APPLICATION (application), NULL);

    return g_object_new (GP_TYPE_WINDOW, "application", application, NULL);
}

