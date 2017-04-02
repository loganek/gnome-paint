/* gp-selectiontool.c
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

#include "gp-selectiontool.h"

G_DEFINE_INTERFACE (GPSelectionTool, gp_selection_tool, GP_TYPE_TOOL)

static void
gp_selection_tool_default_init (GPSelectionToolInterface *iface)
{
}

GdkPixbuf*
gp_selection_tool_get_selection (GPSelectionTool *self)
{
    GPSelectionToolInterface *iface;

    g_return_val_if_fail (GP_IS_SELECTION_TOOL (self), NULL);

    iface = GP_SELECTION_TOOL_GET_IFACE (self);
    g_return_if_fail (iface->get_selection != NULL);

    return iface->get_selection (self);
}

void
gp_selection_tool_clear (GPSelectionTool *self, GdkRGBA color)
{
    GPSelectionToolInterface *iface;

    g_return_if_fail (GP_IS_SELECTION_TOOL (self) == TRUE);

    iface = GP_SELECTION_TOOL_GET_IFACE (self);
    g_return_if_fail (iface->clear != NULL);

    return iface->clear (self, color);
}
