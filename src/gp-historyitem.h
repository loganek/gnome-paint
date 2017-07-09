/* gp-historyitem.h
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

#ifndef GP_HISTORY_ITEM_H_
#define GP_HISTORY_ITEM_H_

#include <glib-object.h>

G_BEGIN_DECLS

#define GP_TYPE_HISTORY_ITEM (gp_history_item_get_type ())
G_DECLARE_DERIVABLE_TYPE (GPHistoryItem, gp_history_item, GP, HISTORY_ITEM, GObject)

typedef struct _GPDocument GPDocument;

struct _GPHistoryItemClass
{
  GObjectClass parent_class;

  void (* undo) (GPHistoryItem *history_item, GPDocument *document);
  void (* redo) (GPHistoryItem *history_item, GPDocument *document);
};

void gp_history_item_undo (GPHistoryItem *history_item);

void gp_history_item_redo (GPHistoryItem *history_item);

G_END_DECLS

#endif /* GP_HISTORY_ITEM_H_ */
