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

struct _GPSizeToolProperty
{
    GPToolProperty parent_instance;

    gdouble size;
    GtkWidget *widget;
};

G_DEFINE_TYPE (GPSizeToolProperty, gp_size_tool_property, GP_TYPE_TOOL_PROPERTY)

static void
gp_size_tool_property_apply (GPToolProperty *self, cairo_t *cairo_context)
{
    cairo_set_line_width (cairo_context, GP_SIZE_TOOL_PROPERTY (self)->size);
}

static void
gp_size_tool_property_widget_activated (GtkWidget *widget, GPSizeToolProperty *property)
{
    property->size = g_ascii_strtod (gtk_entry_get_text (GTK_ENTRY (widget)), NULL);
}

static GtkWidget *
gp_size_tool_get_widget (GPToolProperty *self)
{
    GPSizeToolProperty *property = GP_SIZE_TOOL_PROPERTY (self);

    if (property->widget == NULL)
    {
        gchar *buf = g_strdup_printf ("%f", property->size);
        property->widget = gtk_entry_new ();
        gtk_entry_set_width_chars (GTK_ENTRY (property->widget), 4);
        gtk_entry_set_text (GTK_ENTRY (property->widget), buf);
        g_free (buf);
        g_signal_connect (property->widget, "activate", G_CALLBACK (gp_size_tool_property_widget_activated), property);
    }

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
    self->size = 1;
    self->widget = NULL;
}

static void
gp_size_tool_property_class_init (GPSizeToolPropertyClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    GPToolPropertyClass *tool_property_class = GP_TOOL_PROPERTY_CLASS (klass);

    gobject_class->finalize = gp_size_tool_property_finalize;

    tool_property_class->apply = gp_size_tool_property_apply;
    tool_property_class->get_widget = gp_size_tool_get_widget;
}

GPToolProperty*
gp_size_tool_property_create ()
{
    return  GP_TOOL_PROPERTY (g_object_new (GP_TYPE_SIZE_TOOL_PROPERTY, NULL));
}

gdouble
gp_size_tool_property_get_size (GPSizeToolProperty *property)
{
    return property->size;
}
