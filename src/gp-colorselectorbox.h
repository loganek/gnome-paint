/* gp-colorselectorbox.h
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

#ifndef GP_COLOR_SELECTOR_BOX_H_
#define GP_COLOR_SELECTOR_BOX_H_

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GP_TYPE_COLOR_SELECTOR_BOX (gp_color_selector_box_get_type ())
G_DECLARE_FINAL_TYPE (GPColorSelectorBox, gp_color_selector_box, GP, COLOR_SELECTOR_BOX, GtkBox)

GtkWidget* gp_color_selector_box_new (void);

void gp_color_selector_box_set_color (GPColorSelectorBox *color_selector_box, const GdkRGBA *rgba);

void gp_color_selector_box_get_color (GPColorSelectorBox *color_selector_box, GdkRGBA *rgba);

G_END_DECLS

#endif /* GP_COLOR_SELECTOR_BOX_H_ */