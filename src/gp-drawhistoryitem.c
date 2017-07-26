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

enum
{
    PROP_SURFACE = 1,
    N_PROPERTIES
};

static GParamSpec *obj_properties[N_PROPERTIES] = { NULL };

typedef struct
{
    GPHistoryItem parent_instance;

    cairo_surface_t *prev_surface;
} GPDrawHistoryItemPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GPDrawHistoryItem, gp_draw_history_item, GP_TYPE_HISTORY_ITEM)

static void
gp_draw_history_item_set_property (GObject      *object,
                                   guint         property_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
    GPDrawHistoryItemPrivate *priv = gp_draw_history_item_get_instance_private (GP_DRAW_HISTORY_ITEM (object));
    cairo_surface_t *surface;

    switch (property_id)
    {
    case PROP_SURFACE:
        if (priv->prev_surface)
        {
            cairo_surface_destroy (priv->prev_surface);
        }
        surface = g_value_get_pointer (value);
        // TODO clip only modified region
        priv->prev_surface = gp_cairo_copy_image_surface (surface);
        gp_cairo_repaint_surface (surface, priv->prev_surface, 0, 0);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}

static void
gp_draw_history_item_swap (GPDrawHistoryItem *draw_history_item, GPDocument *document)
{
    GPDrawHistoryItemPrivate *priv = gp_draw_history_item_get_instance_private (draw_history_item);

    cairo_surface_t *doc_surface = gp_document_get_surface (document);
    cairo_surface_t *new_surface = gp_cairo_copy_image_surface (doc_surface);
    gp_cairo_repaint_surface (doc_surface, new_surface, 0, 0);

    gp_cairo_repaint_surface (priv->prev_surface, doc_surface, 0, 0);

    cairo_surface_destroy (priv->prev_surface);
    priv->prev_surface = new_surface;
}

static void
gp_draw_history_item_undo (GPHistoryItem *history_item, GPDocument *document)
{
    GP_DRAW_HISTORY_ITEM_GET_CLASS (history_item)->swap (GP_DRAW_HISTORY_ITEM (history_item), document);
}

static void
gp_draw_history_item_redo (GPHistoryItem *history_item, GPDocument *document)
{
    GP_DRAW_HISTORY_ITEM_GET_CLASS (history_item)->swap (GP_DRAW_HISTORY_ITEM (history_item), document);
}

static void
gp_draw_history_item_init (GPDrawHistoryItem *self)
{
    GPDrawHistoryItemPrivate *priv = gp_draw_history_item_get_instance_private (self);

    priv->prev_surface = NULL;
}

static void
gp_draw_history_item_finalize (GObject *gobj)
{
    GPDrawHistoryItem *history_item = GP_DRAW_HISTORY_ITEM (gobj);
    GPDrawHistoryItemPrivate *priv = gp_draw_history_item_get_instance_private (history_item);

    if (priv->prev_surface)
    {
        cairo_surface_destroy (priv->prev_surface);
        priv->prev_surface = NULL;
    }
}

static void
gp_draw_history_item_class_init (GPDrawHistoryItemClass *klass)
{
    GPHistoryItemClass *history_item_class = GP_HISTORY_ITEM_CLASS (klass);
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    klass->swap = gp_draw_history_item_swap;

    history_item_class->undo = gp_draw_history_item_undo;
    history_item_class->redo = gp_draw_history_item_redo;

    gobject_class->finalize = gp_draw_history_item_finalize;
    gobject_class->set_property = gp_draw_history_item_set_property;

    obj_properties[PROP_SURFACE] =
            g_param_spec_pointer ("surface",
                                  "Surface",
                                  "Initial surface",
                                  G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE);
    g_object_class_install_properties (gobject_class,
                                       N_PROPERTIES,
                                       obj_properties);
}

GPHistoryItem*
gp_draw_history_item_create (cairo_surface_t *surface)
{
    return GP_HISTORY_ITEM (g_object_new (GP_TYPE_DRAW_HISTORY_ITEM, "surface", surface, NULL));
}

