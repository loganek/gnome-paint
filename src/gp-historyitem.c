/* gp-historyitem.c
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

#include "gp-historyitem.h"

#include "gp-documentmanager.h"

G_DEFINE_ABSTRACT_TYPE (GPHistoryItem, gp_history_item, G_TYPE_OBJECT)

static void
gp_history_item_init (GPHistoryItem *history_item)
{
}

static void
gp_history_item_class_init (GPHistoryItemClass *klass)
{
    klass->redo = NULL;
    klass->undo = NULL;
}

void
gp_history_item_undo (GPHistoryItem *history_item)
{
    GPHistoryItemClass *klass = GP_HISTORY_ITEM_GET_CLASS (history_item);
    GPDocumentManager *doc_manager = gp_document_manager_get_default ();
    GPDocument *document = gp_document_manager_get_active_document (doc_manager);

    g_return_if_fail (document && klass && klass->undo);

    klass->undo (history_item, document);
}

void
gp_history_item_redo (GPHistoryItem *history_item)
{
    GPHistoryItemClass *klass = GP_HISTORY_ITEM_GET_CLASS (history_item);
    GPDocumentManager *doc_manager = gp_document_manager_get_default ();
    GPDocument *document = gp_document_manager_get_active_document (doc_manager);

    g_return_if_fail (document && klass && klass->redo);

    klass->redo (history_item, document);
}
