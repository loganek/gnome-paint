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
#include "gp-colormanager.h"

struct _GPColorSelectorBox
{
    /*< private >*/
    GtkBox parent_instance;
};

typedef struct
{
    GtkFlowBox *flow_box;
    GtkFixed *active_colors_fixed;
    GPColorManager *color_manager;

    GtkWidget *fg_button;
    GtkWidget *bg_button;
} GPColorSelectorBoxPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GPColorSelectorBox, gp_color_selector_box, GTK_TYPE_BOX)

#define GP_COLOR_SELECTOR_BOX_PRIV(object) ((GPColorSelectorBoxPrivate *) gp_color_selector_box_get_instance_private (object))

static gboolean
gb_color_selector_box_button_clicked (GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
    GPColorSelectorBoxPrivate *priv = GP_COLOR_SELECTOR_BOX_PRIV (user_data);
    GdkRGBA color;

    gtk_color_chooser_get_rgba (GTK_COLOR_CHOOSER (widget), &color);

    if (event->button == GDK_BUTTON_PRIMARY)
    {
        gp_color_manager_set_color (priv->color_manager, &color, NULL);
    }
    else
    {
        gp_color_manager_set_color (priv->color_manager, NULL, &color);
    }

    return TRUE;
}


static void
gp_color_selector_box_active_color_changed (GPColorManager *manager, gpointer user_data)
{
    GPColorSelectorBoxPrivate *priv = GP_COLOR_SELECTOR_BOX_PRIV (user_data);
    GdkRGBA fg_color;
    GdkRGBA bg_color;

    gp_color_manager_get_color (manager, &fg_color, &bg_color);

    gtk_color_chooser_set_rgba (GTK_COLOR_CHOOSER (priv->fg_button), &fg_color);
    gtk_color_chooser_set_rgba (GTK_COLOR_CHOOSER (priv->bg_button), &bg_color);
}

static GtkWidget *
gp_color_selector_box_create_color_button (GPColorSelectorBox *self, const GdkRGBA *color)
{
    GPColorSelectorBoxPrivate *priv = GP_COLOR_SELECTOR_BOX_PRIV (self);
    GtkWidget *btn = gp_color_button_new_with_rgba (color);

    gtk_widget_show (btn);

    g_signal_connect (G_OBJECT (btn), "single-clicked",
                      G_CALLBACK (gb_color_selector_box_button_clicked),
                      self);

    return btn;
}

static void
gp_color_selector_box_populate_colors (GPColorSelectorBox *self)
{
    GPColorSelectorBoxPrivate *priv = GP_COLOR_SELECTOR_BOX_PRIV (self);
    gsize i;

    for (i = 0; i < gp_color_manager_get_palette_size (priv->color_manager); i++)
    {
        GdkRGBA color = gp_color_manager_get_palette_color (priv->color_manager, i);
        GtkWidget *btn = gp_color_selector_box_create_color_button (self, &color);

        gtk_flow_box_insert (priv->flow_box, btn, i);
    }
}

static void
gp_color_selector_box_create_special_buttons (GPColorSelectorBox *self)
{
    GPColorSelectorBoxPrivate *priv = GP_COLOR_SELECTOR_BOX_PRIV (self);
    GdkRGBA fg_color;
    GdkRGBA bg_color;

    gp_color_manager_get_color (priv->color_manager, &fg_color, &bg_color);

    priv->bg_button = gp_color_selector_box_create_color_button (self, &bg_color);
    gtk_fixed_put (priv->active_colors_fixed, priv->bg_button, 20, 20);

    priv->fg_button = gp_color_selector_box_create_color_button (self, &fg_color);
    gtk_fixed_put (priv->active_colors_fixed, priv->fg_button, 0, 0);
}

static void
gp_color_selector_box_init (GPColorSelectorBox *self)
{
    GPColorSelectorBoxPrivate *priv = GP_COLOR_SELECTOR_BOX_PRIV (self);

    gtk_widget_init_template (GTK_WIDGET (self));

    priv->color_manager = g_object_ref (gp_color_manager_default ());
    gp_color_selector_box_populate_colors (self);
    gp_color_selector_box_create_special_buttons (self);

    g_signal_connect (G_OBJECT (priv->color_manager), "active-color-changed",
                      G_CALLBACK (gp_color_selector_box_active_color_changed),
                      self);
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
                                                  active_colors_fixed);
}


GtkWidget *
gp_color_selector_box_new (void)
{
    return g_object_new (GP_TYPE_COLOR_SELECTOR_BOX, NULL);
}

