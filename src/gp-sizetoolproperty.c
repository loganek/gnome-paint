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
G_DECLARE_FINAL_TYPE (GPSizeToolPropertyWidget, gp_size_tool_property_widget, GP, SIZE_TOOL_PROPERTY_WIDGET, GtkBox)

#define SIZE_BUTTONS_COUNT 5

enum
{
    VALUE_CHANGED,
    LAST_SIGNAL
};

enum
{
    PROP_VALUE = 1,
    LAST_PROPERTY
};

static GParamSpec *gp_size_tool_property_widget_properties[LAST_PROPERTY] = { NULL, };

static guint gp_size_tool_property_widget_signals[LAST_SIGNAL] = { 0 };

struct _GPSizeToolPropertyWidget
{
    GtkBox parent_instance;

    gint value;
};

G_DEFINE_TYPE (GPSizeToolPropertyWidget, gp_size_tool_property_widget, GTK_TYPE_BOX)

static void
gp_size_tool_property_widget_update_ui (GPSizeToolPropertyWidget *self)
{
    GList *list = gtk_container_get_children (GTK_CONTAINER (self));

    while (list)
    {
        gpointer ptr = g_object_get_data (G_OBJECT (list->data), "value");

        if (ptr != NULL)
        {
            if (GPOINTER_TO_INT (ptr) == self->value)
            {
                gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (list->data), TRUE);
                break;
            }
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (list->data), FALSE);
        }

        list = list->next;
    }
}

static void
gp_size_tool_property_widget_set_property (GObject      *object,
                                           guint         property_id,
                                           const GValue *value,
                                           GParamSpec   *pspec)
{
    GPSizeToolPropertyWidget *self = GP_SIZE_TOOL_PROPERTY_WIDGET (object);

    switch (property_id)
    {
    case PROP_VALUE:
        self->value = g_value_get_int (value);
        gp_size_tool_property_widget_update_ui (self);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}

static void
gp_size_tool_property_widget_get_property (GObject    *object,
                                           guint       property_id,
                                           GValue     *value,
                                           GParamSpec *pspec)
{
    GPSizeToolPropertyWidget *self = GP_SIZE_TOOL_PROPERTY_WIDGET (object);

    switch (property_id)
    {
    case PROP_VALUE:
        g_value_set_int (value, self->value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}

static void
gp_size_tool_property_widget_toggled (GtkToggleButton *button, gpointer user_data)
{
    GPSizeToolPropertyWidget *self = GP_SIZE_TOOL_PROPERTY_WIDGET (user_data);
    gint value = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (button), "value"));

    if (gtk_toggle_button_get_active (button) && value != self->value)
    {
        self->value = value;
        g_signal_emit (self, gp_size_tool_property_widget_signals[VALUE_CHANGED], 0);
    }
}

static void
gp_size_tool_property_widget_init (GPSizeToolPropertyWidget *self)
{
    gint i;
    GtkWidget *prev_btn = NULL;

    self->value = 1.0;

    for (i = 0; i < SIZE_BUTTONS_COUNT; i++)
    {
        GtkWidget *btn = prev_btn == NULL ?
                    gtk_radio_button_new_with_label (NULL, "-") :
                    gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (prev_btn), "-");

        g_object_set (G_OBJECT (btn), "draw-indicator", FALSE, NULL);
        g_object_set_data (G_OBJECT (btn), "value", GINT_TO_POINTER (i+1));
        g_signal_connect (btn, "toggled", G_CALLBACK (gp_size_tool_property_widget_toggled), self);
        gtk_box_pack_start (GTK_BOX (self), btn, TRUE, FALSE, 0);
        prev_btn = btn;
    }

    gtk_orientable_set_orientation (GTK_ORIENTABLE (self), GTK_ORIENTATION_VERTICAL);
}

static void
gp_size_tool_property_widget_class_init (GPSizeToolPropertyWidgetClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    gobject_class->set_property = gp_size_tool_property_widget_set_property;
    gobject_class->get_property = gp_size_tool_property_widget_get_property;

    gp_size_tool_property_widget_properties[PROP_VALUE] =
            g_param_spec_int ("value",
                              "Value",
                              "A line width",
                              1, SIZE_BUTTONS_COUNT, 1,
                              G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);

    g_object_class_install_properties (gobject_class,
                                       LAST_PROPERTY,
                                       gp_size_tool_property_widget_properties);

    gp_size_tool_property_widget_signals[VALUE_CHANGED] =
            g_signal_new ("value-changed",
                          G_TYPE_FROM_CLASS (G_OBJECT_CLASS (klass)),
                          G_SIGNAL_RUN_FIRST,
                          0,
                          NULL, NULL, NULL, G_TYPE_NONE, 0);
}

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
gp_size_tool_property_widget_value_changed (GPSizeToolPropertyWidget *widget, GPSizeToolProperty *property)
{
    gint value;
    g_object_get (G_OBJECT (widget), "value", &value, NULL);

    property->size = value;
}

static GtkWidget *
gp_size_tool_get_widget (GPToolProperty *self)
{
    GPSizeToolProperty *property = GP_SIZE_TOOL_PROPERTY (self);

    if (property->widget == NULL)
    {
        property->widget = GTK_WIDGET (g_object_new (GP_TYPE_SIZE_TOOL_PROPERTY_WIDGET, "value", (gint) property->size, NULL));
        g_signal_connect (property->widget, "value-changed", G_CALLBACK (gp_size_tool_property_widget_value_changed), property);
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
    self->size = 1.0;
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
