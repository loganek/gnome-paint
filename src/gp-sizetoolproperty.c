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
};

G_DEFINE_TYPE (GPSizeToolProperty, gp_size_tool_property, GP_TYPE_TOOL_PROPERTY)

static void
gp_size_tool_property_apply (GPToolProperty *self, cairo_t *cairo_context)
{
    cairo_set_line_width (cairo_context, GP_SIZE_TOOL_PROPERTY (self)->size);
}

static void
gp_size_tool_property_init (GPSizeToolProperty *self)
{
    self->size = 1;
}

static void
gp_size_tool_property_class_init (GPSizeToolPropertyClass *klass)
{
    GPToolPropertyClass *tool_property_class = GP_TOOL_PROPERTY_CLASS (klass);

    tool_property_class->apply = gp_size_tool_property_apply;
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
