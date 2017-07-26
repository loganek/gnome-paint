/* gp-selecthistoryitem.h
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

#ifndef GP_SELECT_HISTORY_ITEM_H_
#define GP_SELECT_HISTORY_ITEM_H_

#include "gp-historyitem.h"
#include "gp-iselectiontool.h"

#include <gdk/gdk.h>

G_BEGIN_DECLS

#define GP_TYPE_SELECT_HISTORY_ITEM (gp_select_history_item_get_type ())
G_DECLARE_FINAL_TYPE (GPSelectHistoryItem, gp_select_history_item, GP, SELECT_HISTORY_ITEM, GPHistoryItem)

GPHistoryItem* gp_select_history_item_create (GPISelectionTool *selection_tool,
                                              cairo_surface_t  *surface,
                                              gpointer          selection_params);

G_END_DECLS

#endif /* GP_SELECT_HISTORY_ITEM_H_ */
