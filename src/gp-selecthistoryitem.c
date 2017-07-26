/* gp-selecthistoryitem.c
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

#include "gp-selecthistoryitem.h"

#include "gp-document.h"
#include "gp-toolmanager.h"
#include "gp-cairoutils.h"

struct _GPSelectHistoryItem
{
    GPDrawHistoryItem parent_instance;

    GPISelectionTool *tool;
    gpointer selection_params;
};

G_DEFINE_TYPE (GPSelectHistoryItem, gp_select_history_item, GP_TYPE_DRAW_HISTORY_ITEM)

static void
_set_active_tool (GPSelectHistoryItem *history_item)
{
    GPToolManager* manager = gp_tool_manager_default ();
    gp_tool_manager_set_active_tool (manager, GP_TOOL (history_item->tool));
}

static void
_apply_selection (GPSelectHistoryItem *history_item)
{
    GPISelectionTool *selection_tool = history_item->tool;

    gp_iselection_tool_init_from_params (selection_tool,
                                         history_item->selection_params);
}

static void
gp_select_history_item_swap (GPDrawHistoryItem *history_item, GPDocument *document)
{
    GPSelectHistoryItem *select_history_item = GP_SELECT_HISTORY_ITEM (history_item);
    GP_DRAW_HISTORY_ITEM_CLASS (gp_select_history_item_parent_class)->swap (history_item, document);

    _set_active_tool (select_history_item);
    _apply_selection (select_history_item);
}

static void
gp_select_history_item_init (GPSelectHistoryItem *self)
{
    self->tool = NULL;
}

static void
gp_select_history_item_finalize (GObject *gobj)
{
    GPSelectHistoryItem *history_item = GP_SELECT_HISTORY_ITEM (gobj);

    gp_iselection_tool_destroy_params (GP_ISELECTION_TOOL (history_item->tool), history_item->selection_params);
    history_item->selection_params = NULL;

    g_clear_object (&history_item->tool);
}

static void
gp_select_history_item_class_init (GPSelectHistoryItemClass *klass)
{
    GPDrawHistoryItemClass *history_item_class = GP_DRAW_HISTORY_ITEM_CLASS (klass);
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    history_item_class->swap = gp_select_history_item_swap;

    gobject_class->finalize = gp_select_history_item_finalize;
}

GPHistoryItem*
gp_select_history_item_create (GPISelectionTool *selection_tool, cairo_surface_t *surface, gpointer selection_params)
{
    GPSelectHistoryItem *select_history_item = g_object_new (GP_TYPE_SELECT_HISTORY_ITEM, "surface", surface, NULL);

    select_history_item->tool = g_object_ref (selection_tool);
    select_history_item->selection_params = selection_params;

    return GP_HISTORY_ITEM (select_history_item);
}

