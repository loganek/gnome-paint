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
} GPWindowPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GPWindow, gp_window, GTK_TYPE_APPLICATION_WINDOW)

static void
on_tool_changed (GtkWidget *widget, gpointer user_data)
{
    GPWindowPrivate *priv = gp_window_get_instance_private (GP_WINDOW (user_data));

    gp_image_editor_set_tool (priv->image_editor, gp_tool_box_get_active_tool (priv->tool_box));
}

static void
gp_window_class_init (GPWindowClass *klass)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    gtk_widget_class_set_template_from_resource (widget_class,
                                                 "/org/gnome/Paint/gp-window.ui");
    gtk_widget_class_bind_template_child_private (widget_class, GPWindow,
                                                  image_editor);
    gtk_widget_class_bind_template_child_private (widget_class, GPWindow,
                                                  tool_box);
}

static void
gp_window_init (GPWindow *window)
{
    GPWindowPrivate* priv;
    gtk_widget_init_template (GTK_WIDGET (window));

    priv = gp_window_get_instance_private (window);

    gtk_widget_set_size_request (GTK_WIDGET (priv->image_editor), 100, 100); // TODO

    g_signal_connect (priv->tool_box, "tool-changed", G_CALLBACK (on_tool_changed), window);

    gp_image_editor_set_tool (priv->image_editor, gp_tool_box_get_active_tool (priv->tool_box));
}

GtkWidget *
gp_window_new (GtkApplication *application)
{
    g_return_val_if_fail (GTK_APPLICATION (application), NULL);

    return g_object_new (GP_TYPE_WINDOW, "application", application, NULL);
}

