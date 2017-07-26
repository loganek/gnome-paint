/* gp-drawhistoryitem.h
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

#ifndef GP_DRAW_HISTORY_ITEM_H_
#define GP_DRAW_HISTORY_ITEM_H_

#include "gp-historyitem.h"

#include <gdk/gdk.h>

G_BEGIN_DECLS

#define GP_TYPE_DRAW_HISTORY_ITEM (gp_draw_history_item_get_type ())
G_DECLARE_DERIVABLE_TYPE (GPDrawHistoryItem, gp_draw_history_item, GP, DRAW_HISTORY_ITEM, GPHistoryItem)

struct _GPDrawHistoryItemClass
{
    GPHistoryItemClass parent_instance;

    void (*swap) (GPDrawHistoryItem *history_item, GPDocument *document);
};


GPHistoryItem* gp_draw_history_item_create (cairo_surface_t *surface);

G_END_DECLS

#endif /* GP_DRAW_HISTORY_ITEM_H_ */
