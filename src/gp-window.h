/* gp-window.h
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

#ifndef GP_WINDOW_H_
#define GP_WINDOW_H_

#include "gp-application.h"
#include "gp-imageeditor.h"

G_BEGIN_DECLS

#define GP_TYPE_WINDOW (gp_window_get_type ())
G_DECLARE_FINAL_TYPE (GPWindow, gp_window, GP, WINDOW, GtkApplicationWindow)

GtkWidget * gp_window_new (GtkApplication *application);

GPImageEditor * gp_window_get_active_image_editor (GPWindow *window);

G_END_DECLS

#endif /* GP_WINDOW_H_ */
