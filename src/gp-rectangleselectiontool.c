/* gp-rectangleselectiontool.c
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

#include "gp-rectangleselectiontool.h"

#include "gp-selectiontool.h"
#include "gp-drawingarea.h"

#define RESIZER_SIZE 10

typedef enum {
    STATE_NONE,
    STATE_SELECTING,
    STATE_SELECTED,
    STATE_MOVING,
    STATE_RESIZING
} GPSelectionToolState;

struct _GPRectangleSelectionTool
{
    GPTool parent_instance;

    gint base_x;
    gint base_y;
    gint width;
    gint height;

    gint offset_x;
    gint offset_y;
    gint start_x;
    gint start_y;

    GPSelectionToolState state;
    GPSelectionToolRegion resize_mode;

    gboolean grabbed;
    GdkPixbuf *selection;
};

static void
gp_rectangle_selection_tool_interface_init (GPSelectionToolInterface *iface);

G_DEFINE_TYPE_WITH_CODE (GPRectangleSelectionTool, gp_rectangle_selection_tool, GP_TYPE_TOOL,
                         G_IMPLEMENT_INTERFACE (GP_TYPE_SELECTION_TOOL,
                                                gp_rectangle_selection_tool_interface_init))

static void draw_resizer (cairo_t *cairo_context, gint x, gint y)
{
    cairo_set_source_rgb(cairo_context, 0.0, 0.0, 0.0); // TODO magic color
    cairo_rectangle (cairo_context,
                     x,
                     y,
                     RESIZER_SIZE, RESIZER_SIZE);

    cairo_stroke_preserve(cairo_context);

    cairo_set_source_rgb(cairo_context, 0.6, 0.7, 0.9); // TODO magic color
    cairo_fill(cairo_context);
}

static void
draw_bounding_box (GPRectangleSelectionTool *tool, cairo_t *cairo_context)
{
    static const double dashed3[] = {14.0, 6.0};

    if (tool->state == STATE_NONE)
    {
        return;
    }

    if (tool->width == 0 || tool->height == 0)
    {
        return;
    }

    cairo_save (cairo_context);
    cairo_set_source_rgba(cairo_context, 0.0, 0.0, 0.0, 1.0);
    cairo_set_line_width (cairo_context, 1.0);
    cairo_set_dash(cairo_context, dashed3, 1, 0);
    cairo_set_line_width (cairo_context, 1.0);
    cairo_rectangle (cairo_context,
                     tool->start_x,
                     tool->start_y,
                     tool->width, tool->height);
    cairo_stroke (cairo_context);
    cairo_restore (cairo_context);

    if (!tool->grabbed)
    {
        draw_resizer (cairo_context, tool->start_x, tool->start_y);
        draw_resizer (cairo_context, tool->start_x + tool->width - RESIZER_SIZE, tool->start_y);
        draw_resizer (cairo_context, tool->start_x + tool->width - RESIZER_SIZE, tool->start_y + tool->height - RESIZER_SIZE);
        draw_resizer (cairo_context, tool->start_x, tool->start_y + tool->height - RESIZER_SIZE);

        draw_resizer (cairo_context, tool->start_x, tool->start_y + tool->height / 2 - RESIZER_SIZE / 2);
        draw_resizer (cairo_context, tool->start_x + tool->width - RESIZER_SIZE, tool->start_y + tool->height / 2 - RESIZER_SIZE / 2);

        draw_resizer (cairo_context, tool->start_x + tool->width / 2 - RESIZER_SIZE / 2, tool->start_y);
        draw_resizer (cairo_context, tool->start_x + tool->width / 2 - RESIZER_SIZE / 2, tool->start_y + tool->height - RESIZER_SIZE);
    }
}

static void
draw_selection_to_location (GPRectangleSelectionTool *tool, cairo_t *cairo_context)
{
    GdkPixbuf *pixbuf;

    pixbuf = gp_selection_tool_get_selection (GP_SELECTION_TOOL (tool));

    g_return_if_fail (pixbuf != NULL);

    gdk_cairo_set_source_pixbuf (cairo_context, pixbuf,
                                 tool->start_x,
                                 tool->start_y);

    cairo_rectangle(cairo_context,
                    tool->start_x,
                    tool->start_y,
                    tool->width, tool->height);
    cairo_clip(cairo_context);
    cairo_paint(cairo_context);

    g_object_unref (pixbuf);
}

static void
move_region (GPRectangleSelectionTool *tool, cairo_t *cairo_context)
{
    // Clear previous rectangle TODO copy paste
    GdkRGBA bg_color;
    gp_tool_get_color (GP_TOOL (tool), NULL, &bg_color);
    cairo_set_source_rgba (cairo_context, bg_color.red, bg_color.green, bg_color.blue, bg_color.alpha);
    cairo_rectangle (cairo_context, tool->base_x, tool->base_y, tool->width, tool->height);
    cairo_fill (cairo_context);

    // Draw new
    draw_selection_to_location (tool, cairo_context);
}

static void
gp_rectangle_selection_tool_draw (GPTool *tool,
                                  cairo_t *cairo_context)
{
    GPRectangleSelectionTool *selection_tool = GP_RECTANGLE_SELECTION_TOOL (tool);

    if (selection_tool->state == STATE_MOVING)
    {
        move_region (selection_tool, cairo_context);
    }
    else if (selection_tool->state == STATE_RESIZING)
    {
        // TODO
    }

    draw_bounding_box (selection_tool, cairo_context);
}

static GtkWidget*
gp_line_tool_create_icon (GPTool *tool)
{
    return gtk_image_new_from_resource ("/org/gnome/Paint/toolicons/rectangleselection.png");
}

static void
gp_rectangle_selection_tool_button_press (GPTool *tool, GdkEventButton *event)
{
    GPRectangleSelectionTool *selection_tool = GP_RECTANGLE_SELECTION_TOOL (tool);
    GPSelectionToolState prev_state = selection_tool->state;
    GPSelectionToolRegion region = gp_selection_tool_region_in_selection (GP_SELECTION_TOOL (tool), event->x, event->y);

    switch (region)
    {
    case GP_REGION_NONE:
        selection_tool->state = STATE_SELECTING;
        break;
    case GP_REGION_INSIDE:
        selection_tool->state = STATE_MOVING;
        break;
    default:
        selection_tool->state = STATE_RESIZING;
        selection_tool->resize_mode = region;
        break;
    }

    if (selection_tool->state == STATE_SELECTING)
    {
        if (prev_state == STATE_SELECTED)
        {
            GtkWidget *widget = gp_tool_get_canvas_widget (tool);
            cairo_t *cairo_context = cairo_create (gp_drawing_area_get_surface (GP_DRAWING_AREA (widget)));

            move_region (selection_tool, cairo_context);
            cairo_destroy (cairo_context);
        }

        selection_tool->start_x = selection_tool->base_x = event->x;
        selection_tool->start_y = selection_tool->base_y = event->y;
        selection_tool->width = selection_tool->height = 0;
    }
    else
    {
        selection_tool->offset_x = event->x - selection_tool->start_x;
        selection_tool->offset_y = event->y - selection_tool->start_y;
    }

    selection_tool->grabbed = TRUE;
    gtk_widget_queue_draw (gp_tool_get_canvas_widget (tool));
}

static void
gp_rectangle_selection_tool_button_release (GPTool *tool, GdkEventButton *event, cairo_t *cairo_context)
{
    cairo_surface_t *surface = cairo_get_target (cairo_context);
    GPRectangleSelectionTool *selection_tool = GP_RECTANGLE_SELECTION_TOOL (tool);

    if (selection_tool->state == STATE_SELECTING
            && selection_tool->width != 0 && selection_tool->height != 0)
    {
        if (selection_tool->width < 0)
        {
            selection_tool->base_x = selection_tool->start_x = selection_tool->base_x + selection_tool->width;
            selection_tool->width = -selection_tool->width;
        }
        if (selection_tool->height < 0)
        {
            selection_tool->base_y = selection_tool->start_y = selection_tool->base_y + selection_tool->height;
            selection_tool->height = -selection_tool->height;
        }

        g_clear_object (&selection_tool->selection);
        selection_tool->selection = gdk_pixbuf_get_from_surface (surface,
                                                                 selection_tool->base_x,
                                                                 selection_tool->base_y,
                                                                 selection_tool->width,
                                                                 selection_tool->height);
    }

    selection_tool->state = STATE_SELECTED;
    selection_tool->grabbed = FALSE;

    gtk_widget_queue_draw (gp_tool_get_canvas_widget (tool));
}

static void
gp_rectangle_selection_tool_move (GPTool *tool, GdkEventMotion *event)
{
    GPRectangleSelectionTool *selection_tool = GP_RECTANGLE_SELECTION_TOOL (tool);

    if (selection_tool->grabbed == FALSE)
    {
        return;
    }

    if (selection_tool->state == STATE_MOVING)
    {
        selection_tool->start_x = event->x - selection_tool->offset_x;
        selection_tool->start_y = event->y - selection_tool->offset_y;
    }
    else if (selection_tool->state == STATE_SELECTING)
    {
        selection_tool->width = event->x - selection_tool->base_x;
        selection_tool->height = event->y - selection_tool->base_y;
    }
    gtk_widget_queue_draw (gp_tool_get_canvas_widget (tool));
}

static void
gp_rectangle_selection_tool_deactivate (GPTool *tool)
{
    GPRectangleSelectionTool *selection_tool = GP_RECTANGLE_SELECTION_TOOL (tool);

    if (selection_tool->state == STATE_SELECTED
            && (selection_tool->base_x != selection_tool->start_x || selection_tool->base_y != selection_tool->start_y))
    {
        GtkWidget *widget = gp_tool_get_canvas_widget (tool);
        cairo_t *cairo_context = cairo_create (gp_drawing_area_get_surface (GP_DRAWING_AREA (widget)));

        move_region (selection_tool, cairo_context);
        cairo_destroy (cairo_context);
    }

    selection_tool->grabbed = FALSE;
    selection_tool->state = STATE_NONE;

    if (selection_tool->selection != NULL)
    {
        g_clear_object (&selection_tool->selection);
    }

    gtk_widget_queue_draw (gp_tool_get_canvas_widget (tool));
}

static GdkPixbuf*
gp_rectangle_selection_tool_get_selection (GPSelectionTool *self)
{
    GdkPixbuf *selection = GP_RECTANGLE_SELECTION_TOOL (self)->selection;

    if (selection == NULL)
    {
        return NULL;
    }

    return g_object_ref (selection);
}

static void
gp_rectangle_selection_tool_clear (GPSelectionTool *self)
{
    GPDrawingArea *canvas = GP_DRAWING_AREA (gp_tool_get_canvas_widget (GP_TOOL (self)));
    cairo_surface_t *surface = gp_drawing_area_get_surface (canvas);
    cairo_t *cairo_context = cairo_create (surface);
    GPRectangleSelectionTool *tool = GP_RECTANGLE_SELECTION_TOOL (self);
    GdkRGBA bg_color;

    gp_tool_get_color (GP_TOOL (tool), NULL, &bg_color);

    cairo_set_source_rgba (cairo_context, bg_color.red, bg_color.green, bg_color.blue, bg_color.alpha);
    cairo_rectangle (cairo_context,
                     tool->base_x, tool->base_y,
                     tool->width,
                     tool->height);
    cairo_fill (cairo_context);

    cairo_destroy (cairo_context);
    tool->state = STATE_NONE;
    gtk_widget_queue_draw (GTK_WIDGET (canvas));
}

static gboolean
is_between (gdouble x, gdouble a, gdouble b)
{
    return a <= x && x <= b;
}

static GPSelectionToolRegion
gp_rectangle_selection_tool_region_in_selection (GPSelectionTool *self, gdouble x, gdouble y)
{
    GPRectangleSelectionTool *tool = GP_RECTANGLE_SELECTION_TOOL (self);

    if (tool->state == STATE_NONE)
    {
        return FALSE;
    }

    if (!is_between (x, tool->start_x, tool->start_x + tool->width)
            || !is_between (y, tool->start_y, tool->start_y + tool->height))
    {
        return GP_REGION_NONE;
    }

    // TODO move somewhere else, optimize
    if (is_between (x, tool->start_x, tool->start_x + RESIZER_SIZE))
    {
        if (is_between (y, tool->start_y, tool->start_y + RESIZER_SIZE))
        {
            return GP_REGION_TL_CORNER;
        }
        if (is_between (y, tool->start_y + tool->height - RESIZER_SIZE, tool->start_y + tool->height))
        {
            return GP_REGION_BL_CORNER;
        }
        if (is_between (y, tool->start_y + tool->height / 2 - RESIZER_SIZE, tool->start_y + tool->height / 2 + RESIZER_SIZE))
        {
            return GP_REGION_L_SIDE;
        }
    }
    else if (is_between (x, tool->start_x + tool->width - RESIZER_SIZE, tool->start_x + tool->width))
    {
        if (is_between (y, tool->start_y, tool->start_y + RESIZER_SIZE))
        {
            return GP_REGION_TR_CORNER;
        }
        if (is_between (y, tool->start_y + tool->height - RESIZER_SIZE, tool->start_y + tool->height))
        {
            return GP_REGION_BR_CORNER;
        }
        if (is_between (y, tool->start_y + tool->height / 2 - RESIZER_SIZE, tool->start_y + tool->height / 2 + RESIZER_SIZE))
        {
            return GP_REGION_R_SIDE;
        }
    }
    else if (is_between (x, tool->start_x + tool->width / 2- RESIZER_SIZE, tool->start_x + tool->width / 2 + RESIZER_SIZE))
    {
        if (is_between (y, tool->start_y, tool->start_y + RESIZER_SIZE))
        {
            return GP_REGION_T_SIDE;
        }
        if (is_between (y, tool->start_y + tool->height - RESIZER_SIZE, tool->start_y + tool->height))
        {
            return GP_REGION_B_SIDE;
        }
    }

    return GP_REGION_INSIDE;
}

static void
gp_rectangle_selection_tool_interface_init (GPSelectionToolInterface *iface)
{
    iface->get_selection = gp_rectangle_selection_tool_get_selection;
    iface->clear = gp_rectangle_selection_tool_clear;
    iface->region_in_selection = gp_rectangle_selection_tool_region_in_selection;
}

static void
gp_rectangle_selection_tool_init (GPRectangleSelectionTool *self)
{
    self->grabbed = FALSE;
    self->selection = NULL;
    self->offset_x = self->offset_y = 0.0;
}

static void
gp_rectangle_selection_tool_class_init (GPRectangleSelectionToolClass *klass)
{
    GPToolClass *tool_class = GP_TOOL_CLASS (klass);

    // todo finalize

    tool_class->create_icon = gp_line_tool_create_icon;
    tool_class->button_press = gp_rectangle_selection_tool_button_press;
    tool_class->button_release = gp_rectangle_selection_tool_button_release;
    tool_class->draw = gp_rectangle_selection_tool_draw;
    tool_class->move = gp_rectangle_selection_tool_move;
    tool_class->deactivate = gp_rectangle_selection_tool_deactivate;
}

GPTool*
gp_rectangle_selection_tool_create ()
{
    return  GP_TOOL (g_object_new (GP_TYPE_RECTANGLE_SELECTION_TOOL, NULL));
}

