/* gp-toolbox.h
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

#ifndef GP_TOOL_BOX_H_
#define GP_TOOL_BOX_H_

#include "gp-tool.h"

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GP_TYPE_TOOL_BOX (gp_tool_box_get_type ())
G_DECLARE_FINAL_TYPE (GPToolBox, gp_tool_box, GP, TOOL_BOX, GtkBox)

GtkWidget * gp_tool_box_new (void);

G_END_DECLS

#endif /* GP_TOOL_BOX_H_ */
