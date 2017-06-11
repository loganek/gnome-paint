/* gp-help-commands.c
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

#include "gp-help-commands.h"

#include <glib/gi18n.h>

void
gp_help_commands_keyboard_shortcuts (GSimpleAction *action,
                                     GVariant *parameter,
                                     gpointer user_data)
{
    GtkApplication *app;
    GtkWindow *window;

    app = GTK_APPLICATION (user_data);
    window = gtk_application_get_active_window (app);

    static GtkWidget *shortcuts_window;

    if (shortcuts_window == NULL)
    {
        GtkBuilder *builder;

        builder = gtk_builder_new_from_resource ("/org/gnome/Paint/gp-shortcuts.ui");
        shortcuts_window = GTK_WIDGET (gtk_builder_get_object (builder, "shortcuts"));

        g_signal_connect (shortcuts_window,
                          "destroy",
                          G_CALLBACK (gtk_widget_destroyed),
                          &shortcuts_window);

        g_object_unref (builder);
    }

    if (GTK_WINDOW (window) != gtk_window_get_transient_for (GTK_WINDOW (shortcuts_window)))
    {
        gtk_window_set_transient_for (GTK_WINDOW (shortcuts_window), GTK_WINDOW (window));
    }

    gtk_widget_show_all (shortcuts_window);
    gtk_window_present (GTK_WINDOW (shortcuts_window));

}

void
gp_help_commands_about (GSimpleAction *action,
                        GVariant *parameter,
                        gpointer user_data)
{
    GtkApplication *application;
    GtkWindow *parent;
    static const gchar* artists[] = {
        NULL
    };
    static const gchar* authors[] = {
        "Marcin Kolny <marcin.kolny@gmail.com>",
        NULL
    };

    application = GTK_APPLICATION (user_data);
    parent = gtk_application_get_active_window (GTK_APPLICATION (application));
    gtk_show_about_dialog (parent,
                           "authors", authors,
                           "artists", artists,
                           "translator-credits", _("translator-credits"),
                           "comments", _("Simple Paint for GNOME"),
                           "copyright", "Copyright © 2017 Marcin Kolny <marcin.kolny@gmail.com>",
                           "license-type", GTK_LICENSE_GPL_3_0,
                           "logo-icon-name", PACKAGE_NAME,
                           "version", PACKAGE_VERSION,
                           "website", PACKAGE_URL, NULL);
}

void
gp_help_commands_help (GSimpleAction *action,
                       GVariant *parameter,
                       gpointer user_data)
{
    GtkApplication *application;
    GtkWindow *parent;
    GError *error = NULL;

    application = GTK_APPLICATION (user_data);
    parent = gtk_application_get_active_window (application);

    gtk_show_uri_on_window (parent, "help:gnome-paint",
                            GDK_CURRENT_TIME, &error);

    if (error)
    {
        g_debug (_("Error while opening help: %s"), error->message);
        g_error_free (error);
    }
}
