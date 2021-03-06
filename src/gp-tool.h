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

typedef struct {
    double x;
    double y;
} GdkPointD;

struct _GPToolClass
{
    GObjectClass parent_class;

    void (*button_press) (GPTool *tool,
                          GdkEventButton *event,
                          GdkPointD pos);
    void (*move) (GPTool *tool,
                  GdkEventMotion *event,
                  GdkPointD pos);
    void (*button_release) (GPTool *tool,
                            GdkEventButton *event,
                            GdkPointD pos);
    void (*activate) (GPTool *tool);
    void (*deactivate) (GPTool *tool);

    GtkWidget* (*create_icon) (GPTool *tool);
    const GPtrArray* (*get_properties) (GPTool *tool);
};

GtkWidget* gp_tool_create_icon (GPTool *tool);

void gp_tool_button_press (GPTool *tool, GdkEventButton *event, GdkPointD pos);

void gp_tool_button_release (GPTool *tool, GdkEventButton *event, GdkPointD pos);

void gp_tool_move (GPTool *tool, GdkEventMotion *event, GdkPointD pos);

void gp_tool_activate (GPTool *tool);

void gp_tool_deactivate (GPTool *tool);

const GPtrArray* gp_tool_get_properties (GPTool *tool);

void gp_tool_set_canvas_widget (GPTool *tool, GtkWidget *widget);
GtkWidget* gp_tool_get_canvas_widget (GPTool *tool);

G_END_DECLS

#endif /* GP_TOOL_H_ */

