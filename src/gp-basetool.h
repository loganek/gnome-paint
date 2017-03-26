/* gp-basetool.h
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

#ifndef GP_BASE_TOOL_H_
#define GP_BASE_TOOL_H_

#include "gp-tool.h"

G_BEGIN_DECLS

#define GP_TYPE_BASE_TOOL (gp_base_tool_get_type ())
G_DECLARE_DERIVABLE_TYPE (GPBaseTool, gp_base_tool, GP, BASE_TOOL, GPTool)

struct _GPBaseToolClass
{
    GPToolClass parent_class;

    void (*draw_bbox) (GPBaseTool *tool,
                       cairo_t *cairo_context);
    void (*pre_button_release) (GPBaseTool *tool,
                                GdkEventButton *event,
                                cairo_t *cr);
};

GdkPoint gp_base_tool_get_start_point (GPBaseTool *tool);

GdkPoint gp_base_tool_get_current_point (GPBaseTool *tool);

G_END_DECLS

#endif /* GP_BASE_TOOL_H_ */
