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
#include "gp-documentmanager.h"

#include <glib/gi18n.h>

struct _GPApplication
{
    /*< private >*/
    GtkApplication parent_instance;

    GMenuModel *hamburger_menu;
    GSList *startup_files;
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
    GPDocumentManager *document_manager;
    GPApplication *self;

    window = gp_window_new (GTK_APPLICATION (application));
    document_manager = gp_document_manager_get_default ();
    self = GP_APPLICATION (application);

    gp_tool_manager_create_default_tool_set (gp_tool_manager_default ());

    if (self->startup_files)
    {
        gp_document_manager_create_document_from_file (document_manager, G_FILE (self->startup_files->data));
    }
    else
    {
        gp_document_manager_create_new_document (document_manager, 240, 120); // TODO 240 and 120 from parameters
    }

    gtk_widget_show (window);
}

static const GOptionEntry options[] =
{
    {
        "version", 'v', 0, G_OPTION_ARG_NONE, NULL,
        N_("Print version information and exit"), NULL
    },
    {
        G_OPTION_REMAINING, '\0', 0, G_OPTION_ARG_FILENAME_ARRAY, NULL, NULL,
        N_("[FILE...]")
    },

    { NULL }
};

static gint
gp_application_command_line (GApplication            *application,
                             GApplicationCommandLine *cl)
{
    GVariantDict *options;
    gchar **remaining_args = NULL;
    GPApplication *self = GP_APPLICATION (application);

    options = g_application_command_line_get_options_dict (cl);

    if (g_variant_dict_lookup (options, G_OPTION_REMAINING, "^a&ay", &remaining_args) && remaining_args != NULL)
    {
        // TODO currently handle only first file
        GFile *file = g_application_command_line_create_file_for_arg (cl, remaining_args[0]);

        self->startup_files = g_slist_append (self->startup_files, file);

        g_free (remaining_args);
    }

    g_application_activate (application);

    g_slist_free_full (self->startup_files, g_object_unref);
    self->startup_files = NULL;

    return 0;
}

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
    { "shortcuts", gp_help_commands_keyboard_shortcuts, NULL, NULL, NULL, {0} },
    { "help", gp_help_commands_help, NULL, NULL, NULL, {0} },
    { "about", gp_help_commands_about, NULL, NULL, NULL, {0} },
    { "quit", on_quit, NULL, NULL, NULL, {0} }
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
    gtk_window_set_default_icon_name (PACKAGE_NAME);

    GP_APPLICATION (application)->hamburger_menu =
            G_MENU_MODEL (g_object_ref_sink (gtk_application_get_menu_by_id (GTK_APPLICATION (application), "hamburger-menu")));

    /* Must register custom types before using them from GtkBuilder. */
    gp_widget_gtype_init ();

    add_accelerator (GTK_APPLICATION (application), "win.open",  "<Primary>o");
    add_accelerator (GTK_APPLICATION (application), "win.close", "<Primary>w");
    add_accelerator (GTK_APPLICATION (application), "win.copy",  "<Primary>c");
    add_accelerator (GTK_APPLICATION (application), "win.cut",   "<Primary>x");
    add_accelerator (GTK_APPLICATION (application), "win.paste", "<Primary>v");
    add_accelerator (GTK_APPLICATION (application), "win.undo",  "<Primary>z");
    add_accelerator (GTK_APPLICATION (application), "win.redo",  "<Primary>y");
}


static void
gp_application_finalize (GObject *object)
{
}

static void
gp_application_init (GPApplication *application)
{
    g_application_add_main_option_entries (G_APPLICATION (application), options);

    application->startup_files = NULL;
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
    app_class->command_line = gp_application_command_line;
}

GtkApplication *
gp_application_new (void)
{
    return g_object_new (GP_TYPE_APPLICATION, "application-id",
                         "org.gnome.Paint", "flags", G_APPLICATION_HANDLES_COMMAND_LINE, NULL);
}

GMenuModel*
gp_application_get_hamburger_menu_model (GPApplication *application)
{
    return application->hamburger_menu;
}
