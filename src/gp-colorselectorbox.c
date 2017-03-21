/* gp-colorselectorbox.c
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

#include "gp-colorselectorbox.h"
#include "gp-colorbutton.h"

#define BOX_COLOR_COUNT 8

static GdkRGBA default_colors[BOX_COLOR_COUNT] = {
    {1.0, 1.0, 1.0, 1.0}, // white
    {0.0, 0.0, 0.0, 1.0}, // black
    {1.0, 0.0, 0.0, 1.0}, // red
    {0.0, 1.0, 0.0, 1.0}, // green
    {0.0, 0.0, 1.0, 1.0}, // blue
    {1.0, 1.0, 0.0, 1.0}, // yellow
    {0.0, 1.0, 1.0, 1.0}, // cyan
    {1.0, 0.0, 1.0, 1.0}, // magenta
};

/* Signals */
enum
{
    COLOR_CHANGED,
    LAST_SIGNAL
};

static guint gp_color_selector_box_signals[LAST_SIGNAL] = { 0 };

struct _GPColorSelectorBox
{
    /*< private >*/
    GtkBox parent_instance;
};

typedef struct
{
    GdkRGBA active_color;
    GtkFlowBox *flow_box;
    GtkLabel *active_color_indicator;
} GPColorSelectorBoxPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GPColorSelectorBox, gp_color_selector_box, GTK_TYPE_BOX)

static gboolean
on_gb_color_selector_box_color_changed (GtkWidget *widget, gpointer user_data)
{
    GPColorSelectorBox *color_selector_box = GP_COLOR_SELECTOR_BOX (user_data);

    GdkRGBA color;
    gtk_color_chooser_get_rgba (GTK_COLOR_CHOOSER (widget), &color);
    gp_color_selector_box_set_color (color_selector_box, &color);

    return TRUE;
}

static void
gp_color_selector_box_populate_default_colors (GPColorSelectorBox *color_selector_box)
{
    GPColorSelectorBoxPrivate *priv = gp_color_selector_box_get_instance_private (color_selector_box);

    for (int i = 0; i < BOX_COLOR_COUNT; i++)
    {
        GtkWidget *btn = gp_color_button_new_with_rgba (&default_colors[i]);

        gtk_widget_show (btn);
        gtk_flow_box_insert (priv->flow_box, btn, i);

        g_signal_connect(G_OBJECT(btn), "singleclicked",
			 G_CALLBACK(on_gb_color_selector_box_color_changed),
                         color_selector_box);
        g_signal_connect(G_OBJECT(btn), "color-set",
			 G_CALLBACK(on_gb_color_selector_box_color_changed),
                         color_selector_box);
    }
}

static void
gp_color_selector_box_update_active_color_indicator (GPColorSelectorBox *color_selector_box)
{
    // TODO implement this method properly
    GPColorSelectorBoxPrivate *priv = gp_color_selector_box_get_instance_private (color_selector_box);

    gchar *text = g_strdup_printf("%f", priv->active_color.red);
    gtk_label_set_text (priv->active_color_indicator, text);
    g_free (text);
}

static void
gp_color_selector_box_init (GPColorSelectorBox *color_selector_box)
{
    GPColorSelectorBoxPrivate *priv = gp_color_selector_box_get_instance_private (color_selector_box);
    gtk_widget_init_template (GTK_WIDGET (color_selector_box));

    gp_color_selector_box_update_active_color_indicator (color_selector_box);
    gp_color_selector_box_populate_default_colors (color_selector_box);
}

static void
gp_color_selector_box_class_init (GPColorSelectorBoxClass *klass)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    gtk_widget_class_set_template_from_resource (widget_class,
                                                 "/org/gnome/Paint/gp-colorselectorbox.ui");
    gtk_widget_class_bind_template_child_private (widget_class, GPColorSelectorBox,
                                                  flow_box);
    gtk_widget_class_bind_template_child_private (widget_class, GPColorSelectorBox,
                                                  active_color_indicator);

    gp_color_selector_box_signals[COLOR_CHANGED] = g_signal_new ("color-changed",
				                                 G_TYPE_FROM_CLASS (G_OBJECT_CLASS (klass)),
				                                 G_SIGNAL_RUN_FIRST,
				                                 0,
				                                 NULL, NULL, NULL,
				                                 G_TYPE_NONE, 0);
}

void
gp_color_selector_box_set_color (GPColorSelectorBox *color_selector_box, const GdkRGBA *rgba)
{
    GPColorSelectorBoxPrivate *priv = gp_color_selector_box_get_instance_private (color_selector_box);

    priv->active_color = *rgba;

    gp_color_selector_box_update_active_color_indicator (color_selector_box);

    g_signal_emit (color_selector_box, gp_color_selector_box_signals[COLOR_CHANGED], 0);
}

void
gp_color_selector_box_get_color (GPColorSelectorBox *color_selector_box, GdkRGBA *rgba)
{
    GPColorSelectorBoxPrivate *priv = gp_color_selector_box_get_instance_private (color_selector_box);

    *rgba = priv->active_color;
}

GtkWidget *
gp_color_selector_box_new (void)
{
    return g_object_new (GP_TYPE_COLOR_SELECTOR_BOX, NULL);
}

