/* gp-tool.h
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

#ifndef GP_TOOL_H_
#define GP_TOOL_H_

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GP_TYPE_TOOL (gp_tool_get_type ())
G_DECLARE_DERIVABLE_TYPE (GPTool, gp_tool, GP, TOOL, GObject)

struct _GPToolClass
{
    GObjectClass parent_class;

    void (*draw) (GPTool  *tool,
                  cairo_t *cairo_context);
    void (*button_press) (GPTool *tool,
                          GdkEventButton *event);
    void (*move) (GPTool *tool,
                  GdkEventMotion *event);
    void (*button_release) (GPTool *tool,
                            GdkEventButton *event,
                            cairo_t *cairo_context);
    GtkWidget* (*create_icon) (GPTool *tool);
};

GtkWidget* gp_tool_create_icon (GPTool *tool);

void gp_tool_draw (GPTool *tool, cairo_t *cairo_context);

void gp_tool_button_press (GPTool *tool, GdkEventButton *event);

void gp_tool_button_release (GPTool *tool, GdkEventButton *event, cairo_t *cairo_context);

void gp_tool_move (GPTool *tool, GdkEventMotion *event);

void gp_tool_set_canvas_widget (GPTool *tool, GtkWidget *widget);
GtkWidget* gp_tool_get_canvas_widget (GPTool *tool);

G_END_DECLS

#endif /* GP_TOOL_H_ */

