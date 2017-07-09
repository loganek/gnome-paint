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

struct _GPFillShapeProperty
{
    GPToolProperty parent_instance;

    GPFillShapeType fill_type;
};

G_DEFINE_TYPE (GPFillShapeProperty, gp_fill_shape_property, GP_TYPE_TOOL_PROPERTY)

static GtkWidget *
gp_fill_shape_property_get_widget (GPToolProperty *self)
{
    return gtk_label_new ("TODO: FIXME");
}

static void
gp_fill_shape_property_finalize (GObject *object)
{
}

static void
gp_fill_shape_property_init (GPFillShapeProperty *self)
{
    self->fill_type = GP_FILL_SHAPE_TYPE_NO_FILL;
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

    switch (property->fill_type)
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
