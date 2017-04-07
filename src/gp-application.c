/* gp-application.c
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

#include "gp-application.h"
#include "gp-window.h"
#include "gp-colorselectorbox.h"
#include "gp-drawingarea.h"
#include "gp-toolbox.h"
#include "gp-imageeditor.h"
#include "gp-headerbar.h"

#include <glib/gi18n.h>

struct _GPApplication
{
    /*< private >*/
    GtkApplication parent_instance;

    GMenuModel *hamburger_menu;
};

G_DEFINE_TYPE (GPApplication, gp_application, GTK_TYPE_APPLICATION)

static void
on_quit (GSimpleAction *action,
         GVariant *parameter,
         gpointer user_data)
{
    GApplication *application;

    application = G_APPLICATION (user_data);
    g_application_quit (application);
}

static void
add_accelerator (GtkApplication *app,
                 const gchar    *action_name,
                 const gchar    *accel)
{
    const gchar *vaccels[] = {
        accel,
        NULL
    };

    gtk_application_set_accels_for_action (app, action_name, vaccels);
}


static void
gp_application_activate (GApplication *application)
{
    GtkWidget *window;

    window = gp_window_new (GTK_APPLICATION (application));
    gtk_widget_show (window);
}

static const GOptionEntry options[] =
{
    { "version", 'v', 0, G_OPTION_ARG_NONE, NULL,
      N_("Print version information and exit"), NULL },
    { NULL }
};

static gint
gp_application_handle_local_options (GApplication *application,
                                     GVariantDict *options)
{
    if (g_variant_dict_contains (options, "version"))
    {
        g_print (_("%s - Version %s\n"), g_get_application_name (), PACKAGE_VERSION);
        return 0;
    }

    return -1;
}

static void
on_keyboard_shortcuts (GSimpleAction *action,
                       GVariant *parameter,
                       gpointer user_data)
{
    GtkApplication *app;
    GPWindow *window;

    app = GTK_APPLICATION (user_data);
    window = GP_WINDOW (gtk_application_get_active_window (app));

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

static void
on_about (GSimpleAction *action,
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
                           "logo-icon-name", PACKAGE_TARNAME,
                           "version", PACKAGE_VERSION,
                           "website", PACKAGE_URL, NULL);
}

static void
on_help (GSimpleAction *action,
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

static GActionEntry actions[] = {
    { "shortcuts", on_keyboard_shortcuts, NULL, NULL, NULL, {} },
    { "help", on_help, NULL, NULL, NULL, {} },
    { "about", on_about, NULL, NULL, NULL, {} },
    { "quit", on_quit, NULL, NULL, NULL, {} }
};

static void
gp_application_startup (GApplication *application)
{
    g_action_map_add_action_entries (G_ACTION_MAP (application), actions,
                                     G_N_ELEMENTS (actions), application);

    /* Calls gtk_init() with no arguments. */
    G_APPLICATION_CLASS (gp_application_parent_class)->startup (application);

    /* gtk_init() calls setlocale(), so gettext must be called after that. */
    g_set_application_name (_(PACKAGE_NAME));
    gtk_window_set_default_icon_name (PACKAGE_TARNAME);

    GP_APPLICATION (application)->hamburger_menu =
            G_MENU_MODEL (g_object_ref_sink (gtk_application_get_menu_by_id (GTK_APPLICATION (application), "hamburger-menu")));

    /* Must register custom types before using them from GtkBuilder. */
    gp_window_get_type ();
    gp_drawing_area_get_type ();
    gp_color_selector_box_get_type ();
    gp_tool_box_get_type ();
    gp_image_editor_get_type ();
    gp_header_bar_get_type ();

    add_accelerator (GTK_APPLICATION (application), "win.close", "<Primary>w");
    add_accelerator (GTK_APPLICATION (application), "win.copy",  "<Primary>c");
    add_accelerator (GTK_APPLICATION (application), "win.cut",   "<Primary>x");
    add_accelerator (GTK_APPLICATION (application), "win.paste", "<Primary>v");
}


static void
gp_application_finalize (GObject *object)
{
}

static void
gp_application_init (GPApplication *application)
{
    g_application_add_main_option_entries (G_APPLICATION (application), options);
}

static void
gp_application_class_init (GPApplicationClass *klass)
{
    GObjectClass *gobject_class;
    GApplicationClass *app_class;

    gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->finalize = gp_application_finalize;

    app_class = G_APPLICATION_CLASS (klass);
    app_class->activate = gp_application_activate;
    app_class->startup = gp_application_startup;
    app_class->handle_local_options = gp_application_handle_local_options;
}

GtkApplication *
gp_application_new (void)
{
    return g_object_new (GP_TYPE_APPLICATION, "application-id",
                         "org.gnome.Paint", NULL);
}

GMenuModel*
gp_application_get_hamburger_menu_model (GPApplication *application)
{
    return application->hamburger_menu;
}
