/* gp-propertybox.c
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

#include "gp-propertybox.h"
#include "gp-toolproperty.h"
#include "gp-toolmanager.h"

struct _GPPropertyBox
{
    GtkBox parent_instance;
};

G_DEFINE_TYPE (GPPropertyBox, gp_property_box, GTK_TYPE_BOX)

// TODO public, toolmanager independent, also other boxes (i.e. toolbox)
static void
gp_property_box_remove_children (GPPropertyBox *self)
{
    GList *children = gtk_container_get_children (GTK_CONTAINER (self));
    while (children != NULL)
    {
        gtk_container_remove (GTK_CONTAINER (self), GTK_WIDGET (children->data));
        children = children->next;
    }
}

static void
gp_property_box_load_tool_properties (GPPropertyBox *property_box, const GPtrArray *properties)
{
    GtkBox *box = GTK_BOX (property_box);
    guint i;

    gp_property_box_remove_children (property_box);

    if (properties == NULL)
    {
        return;
    }

    for (i = 0; i < properties->len; i++)
    {
        GPToolProperty *property = GP_TOOL_PROPERTY (g_ptr_array_index (properties, i));
        gtk_box_pack_start (box, gp_tool_property_get_widget (property), FALSE, TRUE, 0);
        gtk_box_pack_start (box, gtk_separator_new (GTK_ORIENTATION_HORIZONTAL), FALSE, TRUE, 0);
    }

    gtk_widget_show_all (GTK_WIDGET (property_box));
}

static void
gp_property_box_tool_changed (GPToolManager *manager, gpointer user_data)
{
    GPTool *active_tool = gp_tool_manager_get_active_tool (manager);
    const GPtrArray *properties = gp_tool_get_properties (active_tool);

    gp_property_box_load_tool_properties (GP_PROPERTY_BOX (user_data), properties);
}

static void
gp_property_box_class_init (GPPropertyBoxClass *klass)
{
}

static void
gp_property_box_init (GPPropertyBox *self)
{
    g_signal_connect (gp_tool_manager_default (), "active-tool-changed", G_CALLBACK (gp_property_box_tool_changed), self);
}
