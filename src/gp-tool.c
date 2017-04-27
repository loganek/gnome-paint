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
#include "gp-toolproperty.h"

typedef struct
{
    GtkWidget *widget;
    GdkRGBA fg_color;
    GdkRGBA bg_color;
} GPToolPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (GPTool, gp_tool, G_TYPE_OBJECT)

static void
apply_property (gpointer ptr, gpointer user_data)
{
    GP_TOOL_PROPERTY_GET_CLASS (ptr)->apply (GP_TOOL_PROPERTY (ptr), user_data);
}

static void
gp_tool_default_button_press (GPTool *tool, GdkEventButton *event, GdkPoint pos)
{
}

static void
gp_tool_default_button_release (GPTool *tool, GdkEventButton *event, GdkPoint pos)
{
}

static void
gp_tool_default_move (GPTool *tool, GdkEventMotion *event, GdkPoint pos)
{
}

static void
gp_tool_default_activate (GPTool *tool)
{
}

static void
gp_tool_default_deactivate (GPTool *tool)
{
}

static const GPtrArray*
gp_tool_default_get_properties (GPTool *tool)
{
    return NULL;
}

static void
gp_tool_class_init (GPToolClass *klass)
{
    klass->create_icon = NULL;
    klass->button_press = gp_tool_default_button_press;
    klass->button_release = gp_tool_default_button_release;
    klass->move = gp_tool_default_move;
    klass->activate = gp_tool_default_activate;
    klass->deactivate = gp_tool_default_deactivate;
    klass->get_properties = gp_tool_default_get_properties;
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
    g_assert (GP_TOOL_GET_CLASS (tool)->create_icon != NULL);

    return GP_TOOL_GET_CLASS (tool)->create_icon (tool);
}

void
gp_tool_button_press (GPTool *tool, GdkEventButton *event, GdkPoint pos)
{
    GP_TOOL_GET_CLASS (tool)->button_press (tool, event, pos);
}

void
gp_tool_button_release (GPTool *tool, GdkEventButton *event, GdkPoint pos)
{
    GP_TOOL_GET_CLASS (tool)->button_release (tool, event, pos);
}

void
gp_tool_move (GPTool *tool, GdkEventMotion *event, GdkPoint pos)
{
    GP_TOOL_GET_CLASS (tool)->move (tool, event, pos);
}

void
gp_tool_activate (GPTool *tool)
{
    GP_TOOL_GET_CLASS (tool)->activate (tool);
}

void
gp_tool_deactivate (GPTool *tool)
{
    GP_TOOL_GET_CLASS (tool)->deactivate (tool);
}

void
gp_tool_apply_properties (GPTool *tool, cairo_t *cairo_context)
{
    const GPtrArray *properties = gp_tool_get_properties (tool);

    g_ptr_array_foreach ((GPtrArray *) properties,
                         apply_property,
                         cairo_context);
}

const GPtrArray*
gp_tool_get_properties (GPTool *tool)
{
    return GP_TOOL_GET_CLASS (tool)->get_properties (tool);
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

void
gp_tool_set_color (GPTool *tool, const GdkRGBA *fg_color, const GdkRGBA *bg_color)
{
    GPToolPrivate *priv = gp_tool_get_instance_private (tool);

    g_return_if_fail (fg_color != NULL);
    g_return_if_fail (bg_color != NULL);

    priv->fg_color = *fg_color;
    priv->bg_color = *bg_color;
}

void
gp_tool_get_color (GPTool *tool, GdkRGBA *fg_color, GdkRGBA *bg_color)
{
    GPToolPrivate *priv = gp_tool_get_instance_private (tool);

    if (fg_color != NULL)
    {
        *fg_color = priv->fg_color;
    }
    if (bg_color != NULL)
    {
        *bg_color = priv->bg_color;
    }
}
