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
#include "gp-documentinfo.h"
#include "gp-dialogutils.h"

static void
save_file_cmd (GPWindow *window, GPDocumentInfo *document, gboolean use_old_name)
{
    GPImageEditor *editor = gp_window_get_active_image_editor (window);
    gchar* filename = NULL;
    GError *error = NULL;

    gp_document_info_set_pixbuf (document, gp_image_editor_get_pixbuf (editor));

    if (use_old_name == FALSE)
    {
        gchar *suggested_filename = gp_document_info_get_filename (document);
        gp_dialog_utils_show_image_save_dialog (GTK_WINDOW (window), suggested_filename, &filename);
        g_free (suggested_filename);
    }
    else
    {
        filename = gp_document_info_get_filename (document);
        g_return_if_fail (filename != NULL);
    }

    if (filename == NULL)
    {
        return;
    }

    gp_document_info_save_file (document, filename, &error);

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
    GPDocumentInfo *active_document = gp_document_manager_get_active_document (document_manager);

    save_file_cmd (GP_WINDOW (user_data), active_document, FALSE);
}

void _gp_cmd_save (GSimpleAction *action,
                   GVariant      *parameter,
                   gpointer       user_data)
{
    GPDocumentManager *document_manager = gp_document_manager_get_default ();
    GPDocumentInfo *active_document = gp_document_manager_get_active_document (document_manager);

    save_file_cmd (GP_WINDOW (user_data), active_document, gp_document_info_has_custom_name (active_document));
}

void _gp_cmd_cut (GSimpleAction *action,
                   GVariant      *parameter,
                   gpointer       user_data)
{
    GPImageEditor *editor = gp_window_get_active_image_editor (GP_WINDOW (user_data));

    _gp_cmd_copy (action, parameter, user_data);
    gp_image_editor_clear_selection (editor);
}

void _gp_cmd_copy (GSimpleAction *action,
                   GVariant      *parameter,
                   gpointer       user_data)
{
    GPImageEditor *editor = gp_window_get_active_image_editor (GP_WINDOW (user_data));
    GdkPixbuf *selection = NULL;

    g_return_if_fail (gp_image_editor_get_selection (editor, &selection) == TRUE);

    gtk_clipboard_set_image (gtk_clipboard_get (GDK_SELECTION_CLIPBOARD), selection);
    gtk_clipboard_store (gtk_clipboard_get (GDK_SELECTION_CLIPBOARD));
}

void _gp_cmd_paste (GSimpleAction *action,
                   GVariant      *parameter,
                   gpointer       user_data)
{

}
