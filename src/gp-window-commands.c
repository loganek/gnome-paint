/* gp-window-commands.c
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

#include "gp-documentmanager.h"
#include "gp-window-commands.h"
#include "gp-window.h"
#include "gp-window-priv.h"
#include "gp-dialogutils.h"

void _gp_cmd_open (GSimpleAction *action,
                   GVariant      *parameter,
                   gpointer       user_data)
{
    GPDocumentManager *document_manager = gp_document_manager_get_default ();
    GPDocument *document;
    gchar *filename = NULL;
    GFile *file;

    if (gp_dialog_utils_show_image_open_dialog (GTK_WINDOW (user_data), &filename) != GTK_RESPONSE_ACCEPT)
    {
        return;
    }

    file = g_file_new_for_path (filename);
    g_free (filename);

    document = gp_document_manager_create_document_from_file (document_manager, file);

    g_object_unref (file);

    gp_document_manager_set_active_document (document_manager, document);
}

static void
save_file_cmd (GPWindow *window, GPDocument *document, gboolean use_old_name)
{
    gchar* filename = NULL;
    GError *error = NULL;

    if (use_old_name == FALSE)
    {
        gchar *suggested_filename = gp_document_get_filename (document);
        gp_dialog_utils_show_image_save_dialog (GTK_WINDOW (window), suggested_filename, &filename);
        g_free (suggested_filename);
    }
    else
    {
        filename = gp_document_get_filename (document);
        g_return_if_fail (filename != NULL);
    }

    if (filename == NULL)
    {
        return;
    }

    gp_document_save_file (document, filename, &error);

    if (error != NULL)
    {
        gp_dialog_utils_show_error (GTK_WINDOW (window),
                                    "Cannot save file \"%s\": %s",
                                    filename,
                                    error->message);
        g_error_free (error);
    }

    g_free (filename);
}

void _gp_cmd_save_as (GSimpleAction *action,
                      GVariant      *parameter,
                      gpointer       user_data)
{
    GPDocumentManager *document_manager = gp_document_manager_get_default ();
    GPDocument *active_document = gp_document_manager_get_active_document (document_manager);

    save_file_cmd (GP_WINDOW (user_data), active_document, FALSE);
}

void _gp_cmd_save (GSimpleAction *action,
                   GVariant      *parameter,
                   gpointer       user_data)
{
    GPDocumentManager *document_manager = gp_document_manager_get_default ();
    GPDocument *active_document = gp_document_manager_get_active_document (document_manager);

    save_file_cmd (GP_WINDOW (user_data), active_document, gp_document_has_custom_name (active_document));
}

void _gp_cmd_cut (GSimpleAction *action,
                  GVariant      *parameter,
                  gpointer       user_data)
{
    _gp_cmd_copy (action, parameter, user_data);
    // TODO gp_image_editor_clear_selection (editor);
}

void _gp_cmd_copy (GSimpleAction *action,
                   GVariant      *parameter,
                   gpointer       user_data)
{
    GPDocumentManager *document_manager = gp_document_manager_get_default ();
    GPDocument *active_document = gp_document_manager_get_active_document (document_manager);
    GdkPixbuf *selection = gp_document_get_selection (active_document);

    g_return_if_fail (selection != NULL);

    gtk_clipboard_set_image (gtk_clipboard_get (GDK_SELECTION_CLIPBOARD), selection);
    gtk_clipboard_store (gtk_clipboard_get (GDK_SELECTION_CLIPBOARD));
}

void _gp_cmd_paste (GSimpleAction *action,
                    GVariant      *parameter,
                    gpointer       user_data)
{
    GPDocumentManager *document_manager = gp_document_manager_get_default ();
    GPDocument *active_document = gp_document_manager_get_active_document (document_manager);
    GPHistory *history = gp_document_get_history (active_document);

    g_return_if_fail (gp_history_can_redo (history));

    gp_history_redo (history);
    // TODO request only updated region
    gp_document_request_update_view (active_document, NULL);
}

void _gp_cmd_undo (GSimpleAction *action,
                   GVariant      *parameter,
                   gpointer       user_data)
{
    GPDocumentManager *document_manager = gp_document_manager_get_default ();
    GPDocument *active_document = gp_document_manager_get_active_document (document_manager);
    GPHistory *history = gp_document_get_history (active_document);
    GPWindowPrivate *priv = GP_WINDOW_PRIV (user_data);

    g_return_if_fail (gp_history_can_undo (history));

    gp_history_undo (history);

    gp_image_editor_external_modification (priv->image_editor);

    // TODO request only updated region
    gp_document_request_update_view (active_document, NULL);
}

void _gp_cmd_redo (GSimpleAction *action,
                   GVariant      *parameter,
                   gpointer       user_data)
{
    GPDocumentManager *document_manager = gp_document_manager_get_default ();
    GPDocument *active_document = gp_document_manager_get_active_document (document_manager);
    GPHistory *history = gp_document_get_history (active_document);
    GPWindowPrivate *priv = GP_WINDOW_PRIV (user_data);

    g_return_if_fail (gp_history_can_redo (history));

    gp_history_redo (history);

    gp_image_editor_external_modification (priv->image_editor);

    // TODO request only updated region
    gp_document_request_update_view (active_document, NULL);
}
