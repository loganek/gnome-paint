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

#include "gp-documentmanager.h"
#include "gp-toolbox.h"
#include "gp-colorselectorbox.h"
#include "gp-headerbar.h"
#include "gp-documentinfo.h"
#include "gp-dialogutils.h"
#include "gp-window-commands.h"

#include <glib/gi18n.h>

struct _GPWindow
{
    /*< private >*/
    GtkApplicationWindow parent_instance;
};

typedef struct
{
    GPImageEditor *image_editor;
    GPToolBox *tool_box;
    GPColorSelectorBox *color_selector_box;
    GPHeaderBar *header_bar;
} GPWindowPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GPWindow, gp_window, GTK_TYPE_APPLICATION_WINDOW)

static GActionEntry win_entries[] = {
    { "save-as", _gp_cmd_save_as, NULL, NULL, NULL, {} },
    { "save", _gp_cmd_save, NULL, NULL, NULL, {} },
    { "cut", _gp_cmd_cut, NULL, NULL, NULL, {} },
    { "copy", _gp_cmd_copy, NULL, NULL, NULL, {} },
    { "paste", _gp_cmd_paste, NULL, NULL, NULL, {} }
};

static void
gp_window_update_action_sensitivity (GPWindow *window)
{
    GPWindowPrivate *priv = gp_window_get_instance_private (window);
    GAction *action = NULL;
    gboolean selection_enabled = gp_image_editor_get_selection (priv->image_editor, NULL);

    action = g_action_map_lookup_action (G_ACTION_MAP (window), "cut");
    g_simple_action_set_enabled (G_SIMPLE_ACTION (action), selection_enabled);

    action = g_action_map_lookup_action (G_ACTION_MAP (window), "copy");
    g_simple_action_set_enabled (G_SIMPLE_ACTION (action), selection_enabled);
}

static void
on_color_changed (GtkWidget *widget, gpointer user_data)
{
    GdkRGBA color;
    gp_color_selector_box_get_color (GP_COLOR_SELECTOR_BOX (widget), &color);
    gp_image_editor_set_color (GP_IMAGE_EDITOR (user_data), &color);
}

static void
on_canvas_changed (GtkWidget *widget, gpointer user_data)
{
    gp_window_update_action_sensitivity (GP_WINDOW (user_data));
}

static void
on_active_document_status_changed (GPDocumentManager *manager, gpointer user_data)
{
    GPHeaderBar *header_bar = GP_HEADER_BAR (user_data);
    GPDocumentInfo *active_document = gp_document_manager_get_active_document (manager);
    gchar *filename  = gp_document_info_get_filename (active_document);

    gp_header_bar_set_filename (header_bar, filename, FALSE);

    g_free (filename);
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
    gtk_widget_class_bind_template_child_private (widget_class, GPWindow,
                                                  color_selector_box);
    gtk_widget_class_bind_template_child_private (widget_class, GPWindow,
                                                  header_bar);
}

static void
gp_window_init (GPWindow *window)
{
    GPWindowPrivate* priv;
    GPDocumentManager *document_manager;
    gtk_widget_init_template (GTK_WIDGET (window));

    priv = gp_window_get_instance_private (window);

    document_manager = gp_document_manager_get_default ();
    gp_document_manager_create_document (document_manager);

    g_signal_connect (priv->color_selector_box, "color-changed", G_CALLBACK (on_color_changed), priv->image_editor);
    g_signal_connect (document_manager, "active-document-status-changed", G_CALLBACK (on_active_document_status_changed), priv->header_bar);
    g_signal_connect (priv->image_editor, "canvas-changed", G_CALLBACK (on_canvas_changed), window);

    GdkRGBA color;
    gp_color_selector_box_get_color (priv->color_selector_box, &color);
    gp_image_editor_set_color (priv->image_editor, &color);

    g_action_map_add_action_entries (G_ACTION_MAP (window),
                                     win_entries,
                                     G_N_ELEMENTS (win_entries),
                                     window);

    gp_window_update_action_sensitivity (window);
}

GtkWidget *
gp_window_new (GtkApplication *application)
{
    g_return_val_if_fail (GTK_APPLICATION (application), NULL);

    return g_object_new (GP_TYPE_WINDOW, "application", application, NULL);
}

GPImageEditor *
gp_window_get_active_image_editor (GPWindow *window)
{
    GPWindowPrivate *priv = gp_window_get_instance_private (window);

    return priv->image_editor;
}
