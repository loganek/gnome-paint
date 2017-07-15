/* gp-drawhistoryitem.c
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

#include "gp-drawhistoryitem.h"

#include "gp-document.h"
#include "gp-cairoutils.h"

struct _GPDrawHistoryItem
{
    GPHistoryItem parent_instance;

    cairo_surface_t *prev_surface;
};

G_DEFINE_TYPE (GPDrawHistoryItem, gp_draw_history_item, GP_TYPE_HISTORY_ITEM)

static cairo_surface_t *
gp_draw_history_item_copy_surface (cairo_surface_t *surface)
{
    gint width = cairo_image_surface_get_width (surface);
    gint height = cairo_image_surface_get_height (surface);
    cairo_format_t format = cairo_image_surface_get_format (surface);

    return cairo_surface_create_similar_image (surface, format, width, height);
}

static void
gp_draw_history_item_swap (GPDrawHistoryItem *draw_history_item, cairo_surface_t *doc_surface)
{
    cairo_surface_t *new_surface = gp_draw_history_item_copy_surface (doc_surface);
    gp_cairo_repaint_surface (doc_surface, new_surface, 0, 0);

    gp_cairo_repaint_surface (draw_history_item->prev_surface, doc_surface, 0, 0);

    cairo_surface_destroy (draw_history_item->prev_surface);
    draw_history_item->prev_surface = new_surface;
}

static void
gp_draw_history_item_undo (GPHistoryItem *history_item, GPDocument *document)
{
    gp_draw_history_item_swap (GP_DRAW_HISTORY_ITEM (history_item), gp_document_get_surface (document));
}

static void
gp_draw_history_item_redo (GPHistoryItem *history_item, GPDocument *document)
{
    gp_draw_history_item_swap (GP_DRAW_HISTORY_ITEM (history_item), gp_document_get_surface (document));
}

static void
gp_draw_history_item_init (GPDrawHistoryItem *self)
{
    self->prev_surface = NULL;
}

static void
gp_draw_history_item_finalize (GObject *gobj)
{
    GPDrawHistoryItem *history_item = GP_DRAW_HISTORY_ITEM (gobj);

    if (history_item->prev_surface)
    {
        cairo_surface_destroy (history_item->prev_surface);
        history_item->prev_surface = NULL;
    }
}

static void
gp_draw_history_item_class_init (GPDrawHistoryItemClass *klass)
{
    GPHistoryItemClass *history_item_class = GP_HISTORY_ITEM_CLASS (klass);
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    history_item_class->undo = gp_draw_history_item_undo;
    history_item_class->redo = gp_draw_history_item_redo;

    gobject_class->finalize = gp_draw_history_item_finalize;
}

GPHistoryItem*
gp_draw_history_item_create (cairo_surface_t *surface)
{
    GPDrawHistoryItem *item = GP_DRAW_HISTORY_ITEM (g_object_new (GP_TYPE_DRAW_HISTORY_ITEM, NULL));
    cairo_t *cr;

    // TODO clip only modified region
    item->prev_surface = gp_draw_history_item_copy_surface (surface);
    cr = cairo_create (item->prev_surface);
    cairo_set_source_surface (cr, surface, 0, 0);
    cairo_paint (cr);
    cairo_destroy (cr);

    return GP_HISTORY_ITEM (item);
}

