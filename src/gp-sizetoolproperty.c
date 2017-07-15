/* gp-sizetoolproperty.c
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

#include "gp-sizetoolproperty.h"

#define GP_TYPE_SIZE_TOOL_PROPERTY_WIDGET (gp_size_tool_property_widget_get_type ())
G_DECLARE_FINAL_TYPE (GPSizeToolPropertyWidget, gp_size_tool_property_widget, GP, SIZE_TOOL_PROPERTY_WIDGET, GtkListBox)

#define SIZE_DATA_ID "property-size"
#define SIZE_OPTIONS_COUNT 5

struct _GPSizeToolPropertyWidget
{
    GtkListBox parent_instance;

    gint value;
};

G_DEFINE_TYPE (GPSizeToolPropertyWidget, gp_size_tool_property_widget, GTK_TYPE_LIST_BOX)

static GtkWidget*
_create_size_tool_property_widget_option (GPSizeToolPropertyWidget *widget,
                                          const gchar              *resource_path,
                                          guint                     size)
{
    GtkWidget *row = gtk_list_box_row_new ();

    g_object_set_data (G_OBJECT(row), SIZE_DATA_ID, GUINT_TO_POINTER (size));
    gtk_container_add (GTK_CONTAINER (row), gtk_image_new_from_resource (resource_path));
    gtk_list_box_insert (GTK_LIST_BOX (widget), row, 0);

    return row;
}

static void
gp_size_tool_property_widget_init (GPSizeToolPropertyWidget *self)
{
    GtkWidget *default_row;

    _create_size_tool_property_widget_option (self, "/org/gnome/Paint/propertyicons/size_7.png", 7);
    _create_size_tool_property_widget_option (self, "/org/gnome/Paint/propertyicons/size_5.png", 5);
    _create_size_tool_property_widget_option (self, "/org/gnome/Paint/propertyicons/size_3.png", 3);
    _create_size_tool_property_widget_option (self, "/org/gnome/Paint/propertyicons/size_2.png", 2);
    default_row = _create_size_tool_property_widget_option (self, "/org/gnome/Paint/propertyicons/size_1.png", 1);

    gtk_list_box_select_row (GTK_LIST_BOX (self), GTK_LIST_BOX_ROW (default_row));
}

static void
gp_size_tool_property_widget_class_init (GPSizeToolPropertyWidgetClass *klass)
{
}

static gint
gp_size_tool_property_widget_get_size (GPSizeToolPropertyWidget *widget)
{
    GtkWidget *row = GTK_WIDGET (gtk_list_box_get_selected_row (GTK_LIST_BOX (widget)));

    return GPOINTER_TO_UINT (g_object_get_data (G_OBJECT (row), SIZE_DATA_ID));
}

struct _GPSizeToolProperty
{
    GPToolProperty parent_instance;

    GtkWidget *widget;
};

G_DEFINE_TYPE (GPSizeToolProperty, gp_size_tool_property, GP_TYPE_TOOL_PROPERTY)

static GtkWidget *
gp_size_tool_get_widget (GPToolProperty *self)
{
    GPSizeToolProperty *property = GP_SIZE_TOOL_PROPERTY (self);

    return g_object_ref (property->widget);
}

static void
gp_size_tool_property_finalize (GObject *object)
{
    GPSizeToolProperty *self = GP_SIZE_TOOL_PROPERTY (object);

    g_clear_object (&self->widget);
}

static void
gp_size_tool_property_init (GPSizeToolProperty *self)
{
    self->widget = GTK_WIDGET (g_object_new (GP_TYPE_SIZE_TOOL_PROPERTY_WIDGET, NULL));
}

static void
gp_size_tool_property_class_init (GPSizeToolPropertyClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    GPToolPropertyClass *tool_property_class = GP_TOOL_PROPERTY_CLASS (klass);

    gobject_class->finalize = gp_size_tool_property_finalize;

    tool_property_class->get_widget = gp_size_tool_get_widget;
}

GPToolProperty*
gp_size_tool_property_create ()
{
    return  GP_TOOL_PROPERTY (g_object_new (GP_TYPE_SIZE_TOOL_PROPERTY, NULL));
}

void
gp_size_tool_property_apply (GPSizeToolProperty *self, cairo_t *cairo_context)
{
    gint line_width = gp_size_tool_property_widget_get_size (GP_SIZE_TOOL_PROPERTY_WIDGET (self->widget));

    cairo_set_line_width (cairo_context, line_width);
}
