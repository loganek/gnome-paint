/* gp-dialogutils.c
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

#include "gp-dialogutils.h"

#include <glib/gi18n.h>

void
gp_dialog_utils_show_error (GtkWindow *window, const gchar *format, ...)
{
    GtkWidget *dialog = NULL;
    gchar *msg = NULL;
    va_list args;

    va_start (args, format);
    msg = g_strdup_vprintf (format, args);
    va_end (args);

    gtk_message_dialog_new (GTK_WINDOW (window),
                            GTK_DIALOG_DESTROY_WITH_PARENT,
                            GTK_MESSAGE_ERROR,
                            GTK_BUTTONS_CLOSE,
                            msg);
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
    g_free (msg);
}

gint
gp_dialog_utils_show_image_open_dialog (GtkWindow *window, gchar **filename)
{
    GtkFileChooser *chooser = NULL;
    GtkWidget *dialog = NULL;
    gint res;

    g_assert (filename != NULL);

    dialog = gtk_file_chooser_dialog_new (_("Open File"),
                                          window,
                                          GTK_FILE_CHOOSER_ACTION_OPEN,
                                          _("_Cancel"),
                                          GTK_RESPONSE_CANCEL,
                                          _("_Open"),
                                          GTK_RESPONSE_ACCEPT,
                                          NULL);

    res = gtk_dialog_run (GTK_DIALOG (dialog));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        chooser = GTK_FILE_CHOOSER (dialog);
        *filename = gtk_file_chooser_get_filename (chooser);
    }

    gtk_widget_destroy (dialog);

    return res;
}

gint
gp_dialog_utils_show_image_save_dialog (GtkWindow   *window,
                                        const gchar *suggested_filename,
                                        gchar      **filename)
{
    GtkWidget *dialog;
    GtkFileChooser *chooser;
    gint res;

    g_assert (filename != NULL);

    dialog = gtk_file_chooser_dialog_new ("Save File",
                                          window,
                                          GTK_FILE_CHOOSER_ACTION_SAVE,
                                          _("_Cancel"),
                                          GTK_RESPONSE_CANCEL,
                                          _("_Save"),
                                          GTK_RESPONSE_ACCEPT,
                                          NULL);
    chooser = GTK_FILE_CHOOSER (dialog);

    gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);

    gtk_file_chooser_set_filename (chooser, suggested_filename);

    res = gtk_dialog_run (GTK_DIALOG (dialog));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        *filename = gtk_file_chooser_get_filename (chooser);
    }

    gtk_widget_destroy (dialog);

    return res;
}
