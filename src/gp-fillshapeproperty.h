/* gp-fillshapeproperty.h
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

#ifndef GP_FILL_SHAPE_PROPERTY_H_
#define GP_FILL_SHAPE_PROPERTY_H_

#include "gp-toolproperty.h"

G_BEGIN_DECLS

typedef enum
{
    GP_FILL_SHAPE_TYPE_NO_FILL = 0,
    GP_FILL_SHAPE_TYPE_FILL_WITH_BG_COLOR = 1,
    GP_FILL_SHAPE_TYPE_FILL_WITH_FG_COLOR = 2,

    GP_FILL_SHAPE_TYPE_LAST // Keep this last
} GPFillShapeType;

#define GP_TYPE_FILL_SHAPE_PROPERTY (gp_fill_shape_property_get_type ())
G_DECLARE_FINAL_TYPE (GPFillShapeProperty, gp_fill_shape_property, GP, FILL_SHAPE_PROPERTY, GPToolProperty)

GPToolProperty* gp_fill_shape_property_create ();

void gp_fill_shape_property_execute (GPFillShapeProperty *property, cairo_t *cairo_context);

G_END_DECLS

#endif /* GP_FILL_SHAPE_PROPERTY_H_ */
