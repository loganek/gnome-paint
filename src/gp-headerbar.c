/* gp-headerbar.c
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

#include "gp-headerbar.h"
#include "gp-marshal.h"

#include <string.h>

enum
{
    SIGNAL_FILE_OPEN,
    LAST_SIGNAL
};

static guint gp_header_bar_signals[LAST_SIGNAL] = { 0 };

struct _GPHeaderBar
{
    GtkHeaderBar parent_instance;
};

typedef struct
{
    GtkButton *open_button;
} GPHeaderBarPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GPHeaderBar, gp_header_bar, GTK_TYPE_HEADER_BAR)

static void
on_open_button_clicked (GtkWidget *widget, gpointer user_data)
{
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    GtkWidget *dialog = gtk_file_chooser_dialog_new ("Open File",
                                                     GTK_WINDOW (gtk_widget_get_toplevel (widget)),
                                                     action,
                                                     "_Cancel",
                                                     GTK_RESPONSE_CANCEL,
                                                     "_Open",
                                                     GTK_RESPONSE_ACCEPT,
                                                     NULL);

    res = gtk_dialog_run (GTK_DIALOG (dialog));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
        char *filename = gtk_file_chooser_get_filename (chooser);

        g_signal_emit (GP_HEADER_BAR (user_data), gp_header_bar_signals[SIGNAL_FILE_OPEN], 0, filename, NULL);

        g_free (filename);
    }

    gtk_widget_destroy (dialog);
}

static void
gp_header_bar_init (GPHeaderBar *header_bar)
{
    gtk_widget_init_template (GTK_WIDGET (header_bar));
}

static void
gp_header_bar_class_init (GPHeaderBarClass *klass)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    gtk_widget_class_set_template_from_resource (widget_class,
                                                 "/org/gnome/Paint/gp-headerbar.ui");
    gtk_widget_class_bind_template_child_private (widget_class, GPHeaderBar,
                                                  open_button);
    gtk_widget_class_bind_template_callback (widget_class,
                                             on_open_button_clicked);

    gp_header_bar_signals[SIGNAL_FILE_OPEN] =
            g_signal_new ("file-open",
                          G_TYPE_FROM_CLASS (klass),
                          0,
                          0,
                          NULL,
                          NULL,
                          gp_VOID__STRING,
                          G_TYPE_NONE, 1, G_TYPE_STRING);

}

GtkWidget *
gp_header_bar_new (void)
{
    return g_object_new (GP_TYPE_HEADER_BAR, NULL);
}

void
gp_header_bar_set_filename (GPHeaderBar *header_bar, const gchar *filename, gboolean modified)
{
    gchar *name = g_path_get_basename (filename);

    if (modified)
    {
        size_t len = strlen (name);
        name = g_realloc (name, len + 2);
        name [len] = '*'; name[len + 1] = 0;
    }

    gtk_header_bar_set_title (GTK_HEADER_BAR (header_bar), name);

    g_free (name);
}
