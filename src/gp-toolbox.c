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
#include "gp-toolmanager.h"

#include <glib/gi18n.h>

struct _GPToolBox
{
    /*< private >*/
    GtkApplicationWindow parent_instance;
};

typedef struct
{
    GtkFlowBox *flow_box;
    GSList *buttons_list;
    GPToolManager *tool_manager;
    gboolean set_active_tts;
} GPToolBoxPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GPToolBox, gp_tool_box, GTK_TYPE_BOX)


static void
on_tool_button_toggled (GtkWidget *widget, gpointer user_data)
{
    GPToolBoxPrivate *priv = gp_tool_box_get_instance_private (GP_TOOL_BOX (user_data));
    GPTool *tool = g_object_get_data (G_OBJECT (widget), "tool");

    if (!priv->set_active_tts)
    {
        gp_tool_manager_set_active_tool (priv->tool_manager, tool);
    }
}

static void
gp_tool_box_tool_changed (GPToolManager *tool_manager, gpointer user_data)
{
    GPToolBoxPrivate *priv = gp_tool_box_get_instance_private (GP_TOOL_BOX (user_data));
    GSList *start = priv->buttons_list;
    GPTool *active_tool = gp_tool_manager_get_active_tool (priv->tool_manager);

    while (start != NULL)
    {
        gpointer tool = g_object_get_data (G_OBJECT (start->data), "tool");
        if (active_tool == tool)
        {
            priv->set_active_tts = TRUE;
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (start->data), TRUE);
            priv->set_active_tts = FALSE;
            return;
        }

        start = start->next;
    }

    g_warning ("Corresponding button in toolbox doesn't exist for tool %p", (void*) active_tool);
}

static void
gp_tool_box_tool_added (GPToolManager *tool_manager, GPTool *tool, gpointer user_data)
{
    GPToolBoxPrivate *priv = gp_tool_box_get_instance_private (GP_TOOL_BOX (user_data));

    GtkWidget *btn = gtk_radio_button_new (priv->buttons_list);
    g_object_set (G_OBJECT (btn), "draw-indicator", FALSE, NULL);

    gtk_button_set_image (GTK_BUTTON (btn), gp_tool_create_icon (tool));
    gtk_widget_show (btn);
    gtk_flow_box_insert (priv->flow_box, btn, 0);

    g_object_set_data (G_OBJECT (btn), "tool", g_object_ref (tool)); // TODO UNREF

    g_signal_connect (G_OBJECT (btn),
                      "toggled",
                      G_CALLBACK (on_tool_button_toggled),
                      user_data);

    if (priv->buttons_list == NULL)
    {
        priv->buttons_list = gtk_radio_button_get_group (GTK_RADIO_BUTTON (btn));
    }

    priv->buttons_list = g_slist_append (priv->buttons_list, btn);
}

static void
gp_tool_box_finalize (GObject *gobj)
{
    GPToolBoxPrivate *priv = gp_tool_box_get_instance_private (GP_TOOL_BOX (gobj));

    g_object_unref (priv->tool_manager);
    g_slist_free (priv->buttons_list); // buttons are managed by a window
}

static void
gp_tool_box_class_init (GPToolBoxClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    gobject_class->finalize = gp_tool_box_finalize;

    gtk_widget_class_set_template_from_resource (widget_class,
                                                 "/org/gnome/Paint/gp-toolbox.ui");
    gtk_widget_class_bind_template_child_private (widget_class, GPToolBox,
                                                  flow_box);
}

static void
gp_tool_box_init (GPToolBox *tool_box)
{
    GPToolBoxPrivate *priv = gp_tool_box_get_instance_private (tool_box);

    gtk_widget_init_template (GTK_WIDGET (tool_box));

    priv->buttons_list = NULL;
    priv->set_active_tts = FALSE;
    // TODO finalize
    priv->tool_manager = g_object_ref (gp_tool_manager_default ());

    g_signal_connect (priv->tool_manager, "tool-added", G_CALLBACK (gp_tool_box_tool_added), tool_box);
    g_signal_connect (priv->tool_manager, "active-tool-changed", G_CALLBACK (gp_tool_box_tool_changed), tool_box);
}

GtkWidget *
gp_tool_box_new (void)
{
    return g_object_new (GP_TYPE_TOOL_BOX, NULL);
}
