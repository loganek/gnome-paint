/* gp-drawingarea.h
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

#ifndef GP_DRAWING_AREA_H_
#define GP_DRAWING_AREA_H_

#include "gp-tool.h"

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GP_TYPE_DRAWING_AREA (gp_drawing_area_get_type ())
G_DECLARE_FINAL_TYPE (GPDrawingArea, gp_drawing_area, GP, DRAWING_AREA, GtkDrawingArea)

GtkWidget* gp_drawing_area_new (void);

cairo_surface_t* gp_drawing_area_get_surface (GPDrawingArea *drawing_area);

G_END_DECLS

#endif /* GP_DRAWING_AREA_H_ */
