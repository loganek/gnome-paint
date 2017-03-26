/* gp-tool.c
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

#include "gp-tool.h"

typedef struct
{
    GtkWidget *widget;
} GPToolPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GPTool, gp_tool, G_TYPE_OBJECT)

static GtkWidget *
gp_tool_default_create_icon (GPTool *self)
{
    return NULL; // TODO
}

static void
gp_tool_default_draw (GPTool *self, cairo_t *cairo_context)
{
}

static void
gp_tool_default_button_press (GPTool *tool, GdkEventButton *event)
{
}

static void
gp_tool_default_button_release (GPTool *tool, GdkEventButton *event, cairo_t *cairo_context)
{
}

static void
gp_tool_default_move (GPTool *tool, GdkEventMotion *event)
{
}

static void
gp_tool_class_init (GPToolClass *klass)
{
    klass->create_icon = gp_tool_default_create_icon;
    klass->draw = gp_tool_default_draw;
    klass->button_press = gp_tool_default_button_press;
    klass->button_release = gp_tool_default_button_release;
    klass->move = gp_tool_default_move;
}

static void
gp_tool_init (GPTool *self)
{
    GPToolPrivate *priv = gp_tool_get_instance_private (self);

    priv->widget = NULL;
}

GtkWidget*
gp_tool_create_icon (GPTool *tool)
{
    return GP_TOOL_GET_CLASS (tool)->create_icon (tool);
}

void
gp_tool_draw (GPTool *tool, cairo_t *cairo_context)
{
    GP_TOOL_GET_CLASS (tool)->draw (tool, cairo_context);
}

void
gp_tool_button_press (GPTool *tool, GdkEventButton *event)
{
    GP_TOOL_GET_CLASS (tool)->button_press (tool, event);
}

void
gp_tool_button_release (GPTool *tool, GdkEventButton *event, cairo_t *cairo_context)
{
    GP_TOOL_GET_CLASS (tool)->button_release (tool, event, cairo_context);
}

void
gp_tool_move (GPTool *tool, GdkEventMotion *event)
{
    GP_TOOL_GET_CLASS (tool)->move (tool, event);
}

void
gp_tool_set_canvas_widget (GPTool *tool, GtkWidget *widget)
{
    GPToolPrivate *priv = gp_tool_get_instance_private (tool);

    priv->widget = widget;
}

GtkWidget*
gp_tool_get_canvas_widget (GPTool *tool)
{
    GPToolPrivate *priv = gp_tool_get_instance_private (tool);

    return priv->widget;
}
