/* gp-cairoutils.h
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

#ifndef GP_CAIRO_UTILS_H_
#define GP_CAIRO_UTILS_H_

#include <gtk/gtk.h>

G_BEGIN_DECLS

GdkRectangle gp_cairo_stroke_get_bbox (cairo_t *cairo_context);

void gp_cairo_surface_clear (cairo_surface_t *surface);

gboolean gp_cairo_point_is_in_rectangle (const cairo_rectangle_t *rect, gdouble x, gdouble y);

G_END_DECLS

#endif /* GP_CAIRO_UTILS_H_ */
