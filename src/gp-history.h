/* gp-history.h
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

#ifndef GP_HISTORY_H_
#define GP_HISTORY_H_

#include "gp-historyitem.h"

#include <glib-object.h>

G_BEGIN_DECLS

#define GP_TYPE_HISTORY (gp_history_get_type ())
G_DECLARE_FINAL_TYPE (GPHistory, gp_history, GP, HISTORY, GObject)

GPHistory * gp_history_new (void);

/**
 * Add an item to a history.
 *
 * @item (transfer full): history item.
 */
void gp_history_add_item (GPHistory *history, GPHistoryItem *item);

gboolean gp_history_can_undo (GPHistory *history);

void gp_history_undo (GPHistory *history);

gboolean gp_history_can_redo (GPHistory *history);

void gp_history_redo (GPHistory *history);

guint gp_history_get_pointer (GPHistory *history);

G_END_DECLS

#endif /* GP_HISTORY_H_ */
