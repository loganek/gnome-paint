/* gp-toolproperty.c
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

#include "gp-toolproperty.h"

G_DEFINE_TYPE (GPToolProperty, gp_tool_property, G_TYPE_OBJECT)

static void
gp_tool_property_default_apply_property (GPToolProperty *tool_property,
                                         cairo_t        *cairo_context)
{
}

static void
gp_tool_property_class_init (GPToolPropertyClass *klass)
{
    klass->apply = gp_tool_property_default_apply_property;
}

static void
gp_tool_property_init (GPToolProperty *self)
{
}

void
gp_tool_property_apply (GPToolProperty *tool_property, cairo_t *cairo_context)
{
    GP_TOOL_PROPERTY_GET_CLASS (tool_property)->apply (tool_property, cairo_context);
}

GtkWidget *
gp_tool_property_get_widget (GPToolProperty *tool_property)
{
    return GP_TOOL_PROPERTY_GET_CLASS (tool_property)->get_widget (tool_property);
}
