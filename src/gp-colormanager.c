/* gp-colormanager.c
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

#include "config.h"

#include "gp-colormanager.h"


/* Signals */
enum
{
    ACTIVE_COLOR_CHANGED,
    LAST_SIGNAL
};

static guint gp_color_manager_signals[LAST_SIGNAL] = { 0 };

struct _GPColorManager
{
    /*< private >*/
    GObject parent_instance;

    GdkRGBA bg_color;
    GdkRGBA fg_color;
};

G_DEFINE_TYPE (GPColorManager, gp_color_manager, G_TYPE_OBJECT)

static void
gp_application_finalize (GObject *object)
{
}

static void
gp_color_manager_init (GPColorManager *self)
{
}

static void
gp_color_manager_class_init (GPColorManagerClass *klass)
{
    GObjectClass *gobject_class;

    gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->finalize = gp_application_finalize;

    gp_color_manager_signals[ACTIVE_COLOR_CHANGED] = g_signal_new ("active-color-changed",
                                                                   G_TYPE_FROM_CLASS (G_OBJECT_CLASS (klass)),
                                                                   G_SIGNAL_RUN_FIRST,
                                                                   0,
                                                                   NULL, NULL, NULL,
                                                                   G_TYPE_NONE, 0);
}

GPColorManager *
gp_color_manager_default (void)
{

    // TODO default() pattern??
    static GPColorManager *default_manager = NULL;

    if (default_manager == NULL)
    {
        default_manager = g_object_new (GP_TYPE_COLOR_MANAGER, NULL);
    }

    return default_manager;
}

void gp_color_manager_get_color (const GPColorManager *color_manager, GdkRGBA *fg_color, GdkRGBA *bg_color)
{
    if (fg_color)
    {
        *fg_color = color_manager->fg_color;
    }

    if (bg_color)
    {
        *bg_color = color_manager->bg_color;
    }
}

void gp_color_manager_set_color (GPColorManager *color_manager, const GdkRGBA *fg_color, const GdkRGBA *bg_color)
{
    gboolean color_changed = FALSE;

    if (fg_color && !gdk_rgba_equal (fg_color, &color_manager->fg_color))
    {
        color_manager->fg_color = *fg_color;
        color_changed = TRUE;
    }

    if (bg_color && !gdk_rgba_equal (bg_color, &color_manager->bg_color))
    {
        color_manager->bg_color = *bg_color;
        color_changed = TRUE;
    }

    if (color_changed)
    {
        g_signal_emit (color_manager, gp_color_manager_signals[ACTIVE_COLOR_CHANGED], 0);
    }
}
