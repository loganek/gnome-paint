/* gp-toolmanager.h
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

#ifndef GP_TOOL_MANAGER_H_
#define GP_TOOL_MANAGER_H_

#include "gp-tool.h"

#include <glib-object.h>

G_BEGIN_DECLS

#define GP_TYPE_TOOL_MANAGER (gp_tool_manager_get_type ())
G_DECLARE_FINAL_TYPE (GPToolManager, gp_tool_manager, GP, TOOL_MANAGER, GObject)

GPToolManager * gp_tool_manager_default (void);

void gp_tool_manager_add_tool (GPToolManager *tool_manager, GPTool *tool);

GPTool * gp_tool_manager_get_active_tool (GPToolManager *tool_manager);
void gp_tool_manager_set_active_tool (GPToolManager *tool_manager, GPTool *tool);

void gp_tool_manager_create_default_tool_set (GPToolManager *self);

G_END_DECLS

#endif /* GP_APPLICATION_H_ */
