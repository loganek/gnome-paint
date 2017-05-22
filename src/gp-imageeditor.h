/* gp-imageeditor.h
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

#ifndef GP_IMAGE_EDITOR_H_
#define GP_IMAGE_EDITOR_H_

#include "gp-tool.h"
#include "gp-document.h"

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GP_TYPE_IMAGE_EDITOR (gp_image_editor_get_type ())
G_DECLARE_FINAL_TYPE (GPImageEditor, gp_image_editor, GP, IMAGE_EDITOR, GtkFixed)

GtkWidget* gp_image_editor_new (void);

void gp_image_editor_set_color (GPImageEditor *image_editor, const GdkRGBA *color);

void gp_image_editor_set_tool (GPImageEditor *image_editor, GPTool *tool);

void gp_image_editor_set_document (GPImageEditor *image_editor, GPDocument *document);

G_END_DECLS

#endif /* GP_DRAWING_AREA_H_ */
