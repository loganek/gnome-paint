/* gp-toolmanager.c
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

#include "gp-toolmanager.h"
#include "gp-tool.h"
#include "gp-marshal.h"

// Tools
#include "gp-linetool.h"
#include "gp-rectangletool.h"
#include "gp-rectangleselectiontool.h"
#include "gp-pentool.h"

/* Signals */
enum
{
    ACTIVE_TOOL_CHANGED,
    TOOL_ADDED,
    LAST_SIGNAL
};

static guint gp_tool_manager_signals[LAST_SIGNAL] = { 0 };

struct _GPToolManager
{
    GObject parent_instance;

    GPTool *active_tool;
    GSList *tools;
};

G_DEFINE_TYPE (GPToolManager, gp_tool_manager, G_TYPE_OBJECT)

static void
gp_tools_destroy (gpointer data)
{
    g_object_unref (data);
}

static void
gp_tool_manager_finalize (GObject *gobj)
{
    GPToolManager *self = GP_TOOL_MANAGER (gobj);

    g_slist_free_full (self->tools, gp_tools_destroy);
    self->tools = NULL;
}

static void
gp_tool_manager_class_init (GPToolManagerClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    gobject_class->finalize = gp_tool_manager_finalize;

    gp_tool_manager_signals[ACTIVE_TOOL_CHANGED] = g_signal_new ("active-tool-changed",
                                                                 G_TYPE_FROM_CLASS (G_OBJECT_CLASS (klass)),
                                                                 G_SIGNAL_RUN_FIRST,
                                                                 0,
                                                                 NULL, NULL, NULL,
                                                                 G_TYPE_NONE, 0);

    gp_tool_manager_signals[TOOL_ADDED] = g_signal_new ("tool-added",
                                                        G_TYPE_FROM_CLASS (G_OBJECT_CLASS (klass)),
                                                        G_SIGNAL_RUN_FIRST,
                                                        0,
                                                        NULL, NULL, gp_VOID__BOXED,
                                                        G_TYPE_NONE, 1, GP_TYPE_TOOL);

}

void
gp_tool_manager_create_default_tool_set (GPToolManager *self)
{
    gp_tool_manager_add_tool (self, gp_line_tool_create ());
    gp_tool_manager_add_tool (self, gp_rectangle_tool_create ());
    gp_tool_manager_add_tool (self, gp_rectangle_selection_tool_create ());
    gp_tool_manager_add_tool (self, gp_pen_tool_create ());
}

static void
gp_tool_manager_init (GPToolManager *self)
{
    self->tools = NULL;
    self->active_tool = NULL;
}

void gp_tool_manager_add_tool (GPToolManager *tool_manager, GPTool *tool)
{
    g_return_if_fail (tool != NULL);

    tool_manager->tools = g_slist_append (tool_manager->tools, g_object_ref (tool));

    g_signal_emit (tool_manager, gp_tool_manager_signals[TOOL_ADDED], 0, tool, NULL);

    if (tool_manager->active_tool == NULL)
    {
        gp_tool_manager_set_active_tool (tool_manager, tool);
    }
}

GPTool * gp_tool_manager_get_active_tool (GPToolManager *tool_manager)
{
    g_assert (tool_manager->active_tool != NULL);

    return tool_manager->active_tool;
}

void gp_tool_manager_set_active_tool (GPToolManager *tool_manager, GPTool *tool)
{
    GSList *found_tool = g_slist_find (tool_manager->tools, tool);

    g_return_if_fail (found_tool != NULL);

    g_clear_object (&tool_manager->active_tool);
    tool_manager->active_tool = g_object_ref (found_tool->data);

    g_signal_emit (tool_manager, gp_tool_manager_signals[ACTIVE_TOOL_CHANGED], 0);
}

GPToolManager *
gp_tool_manager_default (void)
{
    // TODO default() pattern??
    static GPToolManager *default_manager = NULL;

    if (default_manager == NULL)
    {
        default_manager = g_object_new (GP_TYPE_TOOL_MANAGER, NULL);
    }

    return default_manager;
}
