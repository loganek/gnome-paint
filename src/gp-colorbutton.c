/* gp-colorbutton.c
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

#include "gp-colorbutton.h"
#include "gp-marshal.h"

/* Signals */
enum
{
    SINGLE_CLICKED,
    LAST_SIGNAL
};

static guint gp_color_button_signals[LAST_SIGNAL] = { 0 };

struct _GPColorButton
{
    /*< private >*/
    GtkColorButton parent_instance;
};

G_DEFINE_TYPE (GPColorButton, gp_color_button, GTK_TYPE_COLOR_BUTTON)

static void
gp_color_button_button_event (GtkWidget      *widget,
                              GdkEventButton *event)
{
    if (event->type == GDK_2BUTTON_PRESS)
    {
        GTK_BUTTON_CLASS (gp_color_button_parent_class)->clicked (GTK_BUTTON (widget));
    }
    else if (event->type == GDK_BUTTON_PRESS)
    {
        g_signal_emit (widget, gp_color_button_signals[SINGLE_CLICKED], 0, event);
    }
}

static void
gp_color_button_init (GPColorButton *color_button)
{
    g_signal_connect (color_button,
                      "button-press-event",
                      G_CALLBACK (gp_color_button_button_event),
                      NULL);
}

static void
gp_color_button_class_init (GPColorButtonClass *klass)
{
    GtkButtonClass *button_class = GTK_BUTTON_CLASS (klass);

    button_class->clicked = NULL;

    gp_color_button_signals[SINGLE_CLICKED] = g_signal_new ("single-clicked",
                                                            G_TYPE_FROM_CLASS (G_OBJECT_CLASS (klass)),
                                                            G_SIGNAL_RUN_FIRST,
                                                            0,
                                                            NULL, NULL,
                                                            gp_VOID__BOXED,
                                                            G_TYPE_NONE, 1, GDK_TYPE_EVENT);

}

GtkWidget *
gp_color_button_new (void)
{
    return g_object_new (GP_TYPE_COLOR_BUTTON, NULL);
}

GtkWidget* gp_color_button_new_with_rgba (const GdkRGBA *rgba)
{
    return g_object_new (GP_TYPE_COLOR_BUTTON, "rgba", rgba, NULL);
}

