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

#include "gp-window-commands.h"
#include "gp-window.h"

static gchar*
request_new_filename (GPWindow *window, GPDocumentInfo *document_info)
{
    gchar *suggested_filename = gp_document_info_get_filename (document_info);
    gchar *new_filename = NULL;

    gp_dialog_utils_show_image_save_dialog (GTK_WINDOW (window), suggested_filename, &new_filename);

    g_free (suggested_filename);

    return new_filename;
}


void _gp_cmd_save_as (GSimpleAction *action,
                      GVariant      *parameter,
                      gpointer       user_data)
{
    GPWindow *window = GP_WINDOW (user_data);
    GPImageEditor *editor = gp_window_get_active_image_editor (window);
    GPDocumentInfo *document_info = gp_image_editor_get_document_info (editor);
    gchar* filename = NULL;
    GError *error = NULL;

    filename = request_new_filename (window, document_info);

    if (filename == NULL)
    {
        return FALSE;
    }

    gp_image_editor_save_file (editor, filename, &error);

    if (error == NULL)
    {
        gp_document_info_set_filename (document_info, filename);
    }
    else
    {
        gp_dialog_utils_show_error (GTK_WINDOW (window),
                                    "Cannot save file \"%s\": %s",
                                    filename,
                                    error->message);
        g_error_free (error);
    }

    g_free (filename);


}
