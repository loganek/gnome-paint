/* gp-history.c
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

#include "gp-history.h"

struct _GPHistory
{
    /*< private >*/
    GObject parent_instance;

    GPtrArray *items;
    guint pointer;
};

G_DEFINE_TYPE (GPHistory, gp_history, G_TYPE_OBJECT)

static void
gp_history_finalize (GObject *object)
{
    GPHistory *history = GP_HISTORY (object);

    g_ptr_array_unref (history->items);
}

static void
gp_history_init (GPHistory *history)
{
    history->items = g_ptr_array_new_full(8, NULL); // todo free_func g_object_unref
    history->pointer = 0;
}

static void
gp_history_class_init (GPHistoryClass *klass)
{
    GObjectClass *gobject_class;

    gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->finalize = gp_history_finalize;
}

GPHistory *
gp_history_new (void)
{
    return g_object_new (GP_TYPE_HISTORY, NULL);
}

void
gp_history_add_item (GPHistory *history, GPHistoryItem *item)
{
    if (history->pointer < history->items->len)
    {
        g_ptr_array_remove_range (history->items, history->pointer, history->items->len - history->pointer);
    }

    g_ptr_array_add (history->items, item);
    history->pointer++;
}

gboolean
gp_history_can_undo (GPHistory *history)
{
    return history->pointer > 0;
}

void
gp_history_undo (GPHistory *history)
{
    GPHistoryItem *item;

    g_return_if_fail (gp_history_can_undo (history));

    item = g_ptr_array_index (history->items, --history->pointer);
    gp_history_item_undo (item);
}

gboolean
gp_history_can_redo (GPHistory *history)
{
    return history->items->len > history->pointer;
}

void
gp_history_redo (GPHistory *history)
{
    GPHistoryItem *item;

    g_return_if_fail (gp_history_can_redo (history));

    item = g_ptr_array_index (history->items, history->pointer++);
    gp_history_item_redo (item);
}

guint
gp_history_get_pointer (GPHistory *history)
{
    return history->pointer;
}
