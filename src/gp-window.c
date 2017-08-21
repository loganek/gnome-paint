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

#include "gp-window-priv.h"

#include "gp-documentmanager.h"
#include "gp-dialogutils.h"
#include "gp-window-commands.h"

#include <glib/gi18n.h>

struct _GPWindow
{
    /*< private >*/
    GtkApplicationWindow parent_instance;
};

G_DEFINE_TYPE_WITH_PRIVATE (GPWindow, gp_window, GTK_TYPE_APPLICATION_WINDOW)

static GActionEntry win_entries[] = {
    { "open", _gp_cmd_open, NULL, NULL, NULL, {0} },
    { "save-as", _gp_cmd_save_as, NULL, NULL, NULL, {0} },
    { "save", _gp_cmd_save, NULL, NULL, NULL, {0} },
    { "cut", _gp_cmd_cut, NULL, NULL, NULL, {0} },
    { "copy", _gp_cmd_copy, NULL, NULL, NULL, {0} },
    { "paste", _gp_cmd_paste, NULL, NULL, NULL, {0} },
    { "undo", _gp_cmd_undo, NULL, NULL, NULL, {0} },
    { "redo", _gp_cmd_redo, NULL, NULL, NULL, {0} }
};

GPWindowPrivate *
gp_window_get_priv (GPWindow *window)
{
    return gp_window_get_instance_private (window);
}

static void
gp_window_update_action_sensitivity (GPWindow *window)
{
    GPDocumentManager *document_manager = gp_document_manager_get_default ();
    GPDocument *active_document = gp_document_manager_get_active_document (document_manager);
    gboolean selection_enabled = active_document != NULL && gp_document_get_selection (active_document) != NULL;
    GAction *action;
    gboolean can_undo = FALSE;
    gboolean can_redo = FALSE;

    action = g_action_map_lookup_action (G_ACTION_MAP (window), "cut");
    g_simple_action_set_enabled (G_SIMPLE_ACTION (action), selection_enabled);

    action = g_action_map_lookup_action (G_ACTION_MAP (window), "copy");
    g_simple_action_set_enabled (G_SIMPLE_ACTION (action), selection_enabled);

    if (active_document != NULL)
    {
        GPHistory *history = gp_document_get_history (active_document);
        can_undo = gp_history_can_undo (history);
        can_redo = gp_history_can_redo (history);
    }

    action = g_action_map_lookup_action (G_ACTION_MAP (window), "undo");
    g_simple_action_set_enabled (G_SIMPLE_ACTION (action), can_undo);

    action = g_action_map_lookup_action (G_ACTION_MAP (window), "redo");
    g_simple_action_set_enabled (G_SIMPLE_ACTION (action), can_redo);
}

static void
on_canvas_changed (GtkWidget *widget, gpointer user_data)
{
    GPDocumentManager *document_manager = gp_document_manager_get_default ();
    GPDocument *active_document = gp_document_manager_get_active_document (document_manager);
    gp_document_update_is_dirty (active_document);

    gp_window_update_action_sensitivity (GP_WINDOW (user_data));
}

static void
on_document_status_changed (GPDocumentManager *manager, GPDocument *document, gpointer user_data)
{
    GPHeaderBar *header_bar = GP_HEADER_BAR (user_data);
    GPDocument *active_document = gp_document_manager_get_active_document (manager);
    gchar *filename = NULL;

    if (active_document != document)
    {
        return;
    }

    filename = gp_document_get_filename (document);
    gp_header_bar_set_filename (header_bar, filename, gp_document_get_is_dirty (document));

    g_free (filename);
}

static void
on_active_document_changed (GPDocumentManager *manager, gpointer user_data)
{
    GPWindowPrivate *priv = gp_window_get_instance_private (GP_WINDOW (user_data));
    GPDocument *active_document = gp_document_manager_get_active_document (manager);
    gchar *filename = gp_document_get_filename (active_document);

    gp_header_bar_set_filename (priv->header_bar, filename, gp_document_get_is_dirty (active_document));
    gp_image_editor_set_document (priv->image_editor, active_document);

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

    g_signal_connect (document_manager, "document-status-changed", G_CALLBACK (on_document_status_changed), priv->header_bar);
    g_signal_connect (document_manager, "active-document-changed", G_CALLBACK (on_active_document_changed), window);
    g_signal_connect (priv->image_editor, "canvas-changed", G_CALLBACK (on_canvas_changed), window);

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
