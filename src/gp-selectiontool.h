/* gp-selectiontool.h
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

#ifndef GP_SELECTION_TOOL_H_
#define GP_SELECTION_TOOL_H_

#include "gp-tool.h"

G_BEGIN_DECLS

#define GP_TYPE_SELECTION_TOOL (gp_selection_tool_get_type ())
G_DECLARE_INTERFACE (GPSelectionTool, gp_selection_tool, GP, SELECTION_TOOL, GPTool)

struct _GPSelectionToolInterface
{
  GTypeInterface parent_iface;

  GdkPixbuf* (*get_selection) (GPSelectionTool *self);
  void (*clear) (GPSelectionTool *self);
  gboolean (*is_in_selection) (GPSelectionTool *self, gdouble x, gdouble y);
};

GdkPixbuf* gp_selection_tool_get_selection (GPSelectionTool *self);

void gp_selection_tool_clear (GPSelectionTool *self);

gboolean gp_selection_tool_is_in_selection (GPSelectionTool *self, gdouble x, gdouble y);

G_END_DECLS

#endif /* GP_SELECTION_TOOL_H_ */
