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
#include "gp-iselectiontool.h"
#include "gp-toolmanager.h"
#include "gp-cairoutils.h"

struct _GPSelectHistoryItem
{
    GPHistoryItem parent_instance;
    GPISelectionTool *tool;
    cairo_surface_t *prev_surface;
    gpointer selection_params;
};

G_DEFINE_TYPE (GPSelectHistoryItem, gp_select_history_item, GP_TYPE_HISTORY_ITEM)

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
_swap (GPSelectHistoryItem *history_item, GPDocument *document)
{
    cairo_surface_t *new_surface;
    cairo_surface_t *doc_surface = gp_document_get_surface (document);

    new_surface = gp_cairo_copy_image_surface (doc_surface);
    gp_cairo_repaint_surface (doc_surface, new_surface, 0, 0);

    gp_cairo_repaint_surface (history_item->prev_surface, doc_surface, 0, 0);

    cairo_surface_destroy (history_item->prev_surface);
    history_item->prev_surface = new_surface;

    _set_active_tool (history_item);
    _apply_selection (history_item);
}

static void
gp_select_history_item_undo (GPHistoryItem *history_item, GPDocument *document)
{
    _swap (GP_SELECT_HISTORY_ITEM (history_item), document);
}

static void
gp_select_history_item_redo (GPHistoryItem *history_item, GPDocument *document)
{
    _swap (GP_SELECT_HISTORY_ITEM (history_item), document);
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
    GPHistoryItemClass *history_item_class = GP_HISTORY_ITEM_CLASS (klass);
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    history_item_class->undo = gp_select_history_item_undo;
    history_item_class->redo = gp_select_history_item_redo;

    gobject_class->finalize = gp_select_history_item_finalize;
}

// TODO this class should inherit from the drawhistoryitem
GPHistoryItem*
gp_select_history_item_create (GPISelectionTool *selection_tool, cairo_surface_t *surface, gpointer selection_params)
{
    GPSelectHistoryItem *select_history_item = g_object_new (GP_TYPE_SELECT_HISTORY_ITEM, NULL);

    select_history_item->tool = g_object_ref (selection_tool);
    select_history_item->selection_params = selection_params;

    // TODO clip only modified region
    select_history_item->prev_surface = gp_cairo_copy_image_surface (surface);
    gp_cairo_repaint_surface (surface, select_history_item->prev_surface, 0, 0);

    return GP_HISTORY_ITEM (select_history_item);
}

