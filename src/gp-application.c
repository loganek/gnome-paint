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

#include "gp-widget-init.h"
#include "gp-help-commands.h"
#include "gp-toolmanager.h"

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

    gp_tool_manager_create_default_tool_set (gp_tool_manager_default ());

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

static GActionEntry actions[] = {
    { "shortcuts", gp_help_commands_keyboard_shortcuts, NULL, NULL, NULL, {} },
    { "help", gp_help_commands_help, NULL, NULL, NULL, {} },
    { "about", gp_help_commands_about, NULL, NULL, NULL, {} },
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
    gp_widget_gtype_init ();

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
