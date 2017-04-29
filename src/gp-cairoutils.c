/* gp-cairoutils.c
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

#include "gp-cairoutils.h"

// TODO copy paste shape tool
static void
get_min_max (gdouble v1, gdouble v2, gdouble *min, gdouble *max)
{
    if (v1 > v2)
    {
        *min = v2;
        *max = v1;
    }
    else
    {
        *min = v1;
        *max = v2;
    }
}

GdkRectangle
gp_cairo_stroke_get_bbox (cairo_t *cairo_context)
{
    gdouble x1, x2, y1, y2;

    cairo_stroke_extents (cairo_context, &x1, &y1, &x2, &y2);

    get_min_max (x1, x2, &x1, &x2);
    get_min_max (y1, y2, &y1, &y2);

    return (GdkRectangle) {
        MAX ((int) x1, 0),
        MAX ((int) y1, 0),
        (int) (x2 - x1),
        (int) (y2 - y1)
    };
}

void
gp_cairo_surface_clear (cairo_surface_t *surface)
{
    cairo_t *cr = cairo_create (surface);
    cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint (cr);
}