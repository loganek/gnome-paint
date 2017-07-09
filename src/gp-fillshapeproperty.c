/* gp-fillshapeproperty.c
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

#include "gp-fillshapeproperty.h"

#include "gp-colormanager.h"

#define GP_TYPE_FILL_SHAPE_PROPERTY_WIDGET (gp_fill_shape_property_widget_get_type ())
G_DECLARE_FINAL_TYPE (GPFillShapePropertyWidget, gp_fill_shape_property_widget, GP, FILL_SHAPE_PROPERTY_WIDGET, GtkListBox)

struct _GPFillShapePropertyWidget
{
    GtkListBox parent_instance;

    GtkWidget *options[GP_FILL_SHAPE_TYPE_LAST];
};

G_DEFINE_TYPE (GPFillShapePropertyWidget, gp_fill_shape_property_widget, GTK_TYPE_LIST_BOX)

static void
_create_fill_shape_widget_option (GPFillShapePropertyWidget *widget, const gchar *resource_path, GPFillShapeType type)
{
    GtkWidget *row = gtk_list_box_row_new ();
    gtk_container_add (GTK_CONTAINER (row), gtk_image_new_from_resource (resource_path));
    widget->options[type] = row;
    gtk_list_box_insert (GTK_LIST_BOX (widget), row, type);
}

static void
gp_fill_shape_property_widget_init (GPFillShapePropertyWidget *self)
{
    _create_fill_shape_widget_option (self, "/org/gnome/Paint/propertyicons/nofillrectangle.png", GP_FILL_SHAPE_TYPE_NO_FILL);
    _create_fill_shape_widget_option (self, "/org/gnome/Paint/propertyicons/fillbgrectangle.png", GP_FILL_SHAPE_TYPE_FILL_WITH_BG_COLOR);
    _create_fill_shape_widget_option (self, "/org/gnome/Paint/propertyicons/fillfgrectangle.png", GP_FILL_SHAPE_TYPE_FILL_WITH_FG_COLOR);
}

static void
gp_fill_shape_property_widget_class_init (GPFillShapePropertyWidgetClass *klass)
{
}

static void
gp_fill_shape_property_widget_set_fill_type (GPFillShapePropertyWidget *widget, GPFillShapeType type)
{
    g_return_if_fail (type < GP_FILL_SHAPE_TYPE_LAST && type >= 0);

    gtk_list_box_select_row (GTK_LIST_BOX (widget), GTK_LIST_BOX_ROW (widget->options[type]));
}

GPFillShapeType
gp_fill_shape_property_widget_get_fill_type (GPFillShapePropertyWidget *widget)
{
    GtkWidget *row = GTK_WIDGET (gtk_list_box_get_selected_row (GTK_LIST_BOX (widget)));
    gint i;

    for (i = 0; i < GP_FILL_SHAPE_TYPE_LAST; i++)
    {
        if (widget->options[i] == row)
        {
            return i;
        }
    }

    g_return_val_if_reached (GP_FILL_SHAPE_TYPE_LAST);
}

struct _GPFillShapeProperty
{
    GPToolProperty parent_instance;

    GtkWidget *widget;
};

G_DEFINE_TYPE (GPFillShapeProperty, gp_fill_shape_property, GP_TYPE_TOOL_PROPERTY)

static GtkWidget *
gp_fill_shape_property_get_widget (GPToolProperty *self)
{
    GPFillShapeProperty *property = GP_FILL_SHAPE_PROPERTY (self);

    return g_object_ref (property->widget);
}

static void
gp_fill_shape_property_finalize (GObject *object)
{
    GPFillShapeProperty *self = GP_FILL_SHAPE_PROPERTY (object);

    g_clear_object (&self->widget);
}

static void
gp_fill_shape_property_init (GPFillShapeProperty *self)
{
    self->widget = GTK_WIDGET (g_object_new (GP_TYPE_FILL_SHAPE_PROPERTY_WIDGET, NULL));
    gp_fill_shape_property_widget_set_fill_type (GP_FILL_SHAPE_PROPERTY_WIDGET (self->widget), GP_FILL_SHAPE_TYPE_NO_FILL);
}

static void
gp_fill_shape_property_class_init (GPFillShapePropertyClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    GPToolPropertyClass *tool_property_class = GP_TOOL_PROPERTY_CLASS (klass);

    gobject_class->finalize = gp_fill_shape_property_finalize;

    tool_property_class->get_widget = gp_fill_shape_property_get_widget;
}

GPToolProperty*
gp_fill_shape_property_create ()
{
    return  GP_TOOL_PROPERTY (g_object_new (GP_TYPE_FILL_SHAPE_PROPERTY, NULL));
}

void
gp_fill_shape_property_execute (GPFillShapeProperty *property, cairo_t *cairo_context)
{
    GPColorManager *color_manager = gp_color_manager_default ();
    GdkRGBA bg_color;

    gp_color_manager_get_color (color_manager, NULL, &bg_color);

    switch (gp_fill_shape_property_widget_get_fill_type (GP_FILL_SHAPE_PROPERTY_WIDGET (property->widget)))
    {
    case GP_FILL_SHAPE_TYPE_NO_FILL:
        cairo_stroke (cairo_context);
        break;
    case GP_FILL_SHAPE_TYPE_FILL_WITH_FG_COLOR:
        cairo_fill (cairo_context);
        break;
    case GP_FILL_SHAPE_TYPE_FILL_WITH_BG_COLOR:
        cairo_save (cairo_context);
        cairo_set_source_rgba (cairo_context,
                               bg_color.red,
                               bg_color.green,
                               bg_color.blue,
                               bg_color.alpha);
        cairo_fill_preserve (cairo_context);
        cairo_restore (cairo_context);
        cairo_stroke (cairo_context);
        break;
    default:
        g_return_if_reached ();
    }
}
