/* gp-widget-init.c
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

#include "gp-widget-init.h"

#include "gp-window.h"
#include "gp-colorselectorbox.h"
#include "gp-drawingarea.h"
#include "gp-toolbox.h"
#include "gp-imageeditor.h"
#include "gp-headerbar.h"

void
gp_widget_gtype_init ()
{
    gp_window_get_type ();
    gp_drawing_area_get_type ();
    gp_color_selector_box_get_type ();
    gp_tool_box_get_type ();
    gp_image_editor_get_type ();
    gp_header_bar_get_type ();
}
