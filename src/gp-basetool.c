/* gp-basetool.c
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

#include "gp-basetool.h"

typedef struct
{
    GdkPoint start_point;
    GdkPoint current_point;
    gboolean grabbed;
} GPBaseToolPrivate;

#define GP_BASE_TOOL_GET_PRIV(tool) ((GPBaseToolPrivate* )gp_base_tool_get_instance_private (GP_BASE_TOOL (tool)))

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (GPBaseTool, gp_base_tool, GP_TYPE_TOOL)

static void
gp_base_tool_button_press (GPTool *self, GdkEventButton *event)
{
    GPBaseToolPrivate *priv = GP_BASE_TOOL_GET_PRIV (self);

    if (event->button == GDK_BUTTON_PRIMARY)
    {
        GdkPoint pt = { event->x, event->y };
        priv->start_point = priv->current_point = pt;
        priv->grabbed = TRUE;

        gtk_widget_queue_draw (gp_tool_get_canvas_widget (self));
    }
    else if (event->button == GDK_BUTTON_SECONDARY)
    {
        priv->grabbed = FALSE;

        gtk_widget_queue_draw (gp_tool_get_canvas_widget (self));
    }
}

static void
gp_base_tool_button_release (GPTool *self, GdkEventButton *event, cairo_t *cr)
{
    GPBaseToolPrivate *priv = GP_BASE_TOOL_GET_PRIV (self);

    if (event->button == GDK_BUTTON_PRIMARY && priv->grabbed == TRUE)
    {
        GP_BASE_TOOL_GET_CLASS (self)->pre_button_release (GP_BASE_TOOL (self), event, cr);
        priv->grabbed = FALSE;

        gtk_widget_queue_draw (gp_tool_get_canvas_widget (self));
    }
}

static void
gp_base_tool_move (GPTool *self, GdkEventMotion *event)
{
    GPBaseToolPrivate *priv = GP_BASE_TOOL_GET_PRIV (self);

    if (event->state & GDK_BUTTON1_MASK && priv->grabbed == TRUE)
    {
        GdkPoint pt = { event->x, event->y };
        priv->current_point = pt;

        gtk_widget_queue_draw (gp_tool_get_canvas_widget (self));
    }
}

static void
gp_base_tool_draw (GPTool *self,
                   cairo_t *cairo_context)
{
    GPBaseToolPrivate *priv = GP_BASE_TOOL_GET_PRIV (self);

    if (priv->grabbed)
    {
        GP_BASE_TOOL_GET_CLASS (self)->draw_bbox (GP_BASE_TOOL (self), cairo_context);
    }
}

static void
gp_base_tool_init (GPBaseTool *self)
{
    GPBaseToolPrivate *priv = GP_BASE_TOOL_GET_PRIV (self);
    GdkPoint init_point = { -1, -1 };

    priv->grabbed = FALSE;
    priv->start_point = priv->current_point = init_point;
}

static void
gp_base_tool_class_init (GPBaseToolClass *klass)
{
    GPToolClass *tool_class = GP_TOOL_CLASS (klass);

    tool_class->button_press = gp_base_tool_button_press;
    tool_class->button_release = gp_base_tool_button_release;
    tool_class->move = gp_base_tool_move;
    tool_class->draw = gp_base_tool_draw;
}

GdkPoint
gp_base_tool_get_start_point (GPBaseTool *tool)
{
    return GP_BASE_TOOL_GET_PRIV (tool)->start_point;
}

GdkPoint
gp_base_tool_get_current_point (GPBaseTool *tool)
{
    return GP_BASE_TOOL_GET_PRIV (tool)->current_point;
}

