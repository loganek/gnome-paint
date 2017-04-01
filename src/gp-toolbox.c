/* gp-toolbox.c
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

#include "gp-toolbox.h"
#include "gp-linetool.h" // TODO REMOVE
#include "gp-rectangletool.h" // TODO REMOVE
#include "gp-rectangleselectiontool.h" // TODO REMOVE
#include <glib/gi18n.h>

/* Signals */
enum
{
    TOOL_CHANGED,
    LAST_SIGNAL
};

static guint gp_tool_box_signals[LAST_SIGNAL] = { 0 };

struct _GPToolBox
{
    /*< private >*/
    GtkApplicationWindow parent_instance;
};

typedef struct
{
    guint active_tool_idx;
    GtkFlowBox *flow_box;
    GSList *buttons_list;
    GArray *tools;
} GPToolBoxPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GPToolBox, gp_tool_box, GTK_TYPE_BOX)

static void
gp_tool_box_set_active_tool (GPToolBox *tool_box, guint index)
{
    GPToolBoxPrivate *priv = gp_tool_box_get_instance_private (tool_box);

    g_return_if_fail (priv->tools->len > index);

    priv->active_tool_idx = (gint) index;

    g_signal_emit (tool_box, gp_tool_box_signals[TOOL_CHANGED], 0);
}

static void
on_tool_button_toggled (GtkWidget *widget, gpointer user_data)
{
    guint index = GPOINTER_TO_UINT (g_object_get_data (
                                        G_OBJECT (widget), "tool-index"));

    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)) == FALSE)
    {
        return;
    }

    gp_tool_box_set_active_tool (GP_TOOL_BOX (user_data), index);
}

static void
gp_tool_box_add_tool (GPToolBox *tool_box, GPTool *tool) // TODO make it public
{
    GPToolBoxPrivate *priv = gp_tool_box_get_instance_private (tool_box);
    GSList *buttons_list = priv->buttons_list;

    GtkWidget *btn = gtk_radio_button_new (buttons_list);
    g_object_set (G_OBJECT (btn), "draw-indicator", FALSE, NULL);

    gtk_button_set_image (GTK_BUTTON (btn), gp_tool_create_icon (tool));
    gtk_widget_show (btn);
    gtk_flow_box_insert (priv->flow_box, btn, 0);

    g_object_set_data (G_OBJECT (btn), "tool-index",
                       GUINT_TO_POINTER (priv->tools->len));

    g_signal_connect (G_OBJECT (btn),
                      "toggled",
                      G_CALLBACK (on_tool_button_toggled),
                      tool_box);

    if (priv->buttons_list == NULL)
    {
        priv->buttons_list = gtk_radio_button_get_group (GTK_RADIO_BUTTON (btn));
    }

    g_array_append_val (priv->tools, tool);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (btn), TRUE);
}

static void
gp_tool_box_class_init (GPToolBoxClass *klass)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    gtk_widget_class_set_template_from_resource (widget_class,
                                                 "/org/gnome/Paint/gp-toolbox.ui");
    gtk_widget_class_bind_template_child_private (widget_class, GPToolBox,
                                                  flow_box);

    gp_tool_box_signals[TOOL_CHANGED] = g_signal_new ("tool-changed",
                                                      G_TYPE_FROM_CLASS (G_OBJECT_CLASS (klass)),
                                                      G_SIGNAL_RUN_FIRST,
                                                      0,
                                                      NULL, NULL, NULL,
                                                      G_TYPE_NONE, 0);
}

static void
gp_tool_box_init (GPToolBox *tool_box)
{
    GPToolBoxPrivate *priv = gp_tool_box_get_instance_private (tool_box);

    gtk_widget_init_template (GTK_WIDGET (tool_box));

    priv->buttons_list = NULL;
    priv->tools = g_array_new (FALSE, FALSE, sizeof (GPTool*));
    priv->active_tool_idx = 0;

    gp_tool_box_add_tool (tool_box, gp_line_tool_create ());
    gp_tool_box_add_tool (tool_box, gp_rectangle_tool_create ());
    gp_tool_box_add_tool (tool_box, gp_rectangle_selection_tool_create ());
}

GPTool *
gp_tool_box_get_active_tool (GPToolBox *tool_box)
{
    GPToolBoxPrivate *priv = gp_tool_box_get_instance_private (tool_box);

    g_return_val_if_fail (priv->tools->len > 0
                          && priv->active_tool_idx < priv->tools->len, NULL);

    return g_array_index (priv->tools, GPTool*, priv->active_tool_idx);
}

GtkWidget *
gp_tool_box_new (void)
{
    return g_object_new (GP_TYPE_TOOL_BOX, NULL);
}

