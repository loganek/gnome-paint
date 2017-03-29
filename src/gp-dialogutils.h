/* gp-dialogutils.h
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

#ifndef GP_DIALOG_UTILS_H_
#define GP_DIALOG_UTILS_H_

#include <gtk/gtk.h>

G_BEGIN_DECLS

void gp_dialog_utils_show_error (GtkWindow *window, const gchar *format, ...);

gint gp_dialog_utils_show_image_open_dialog (GtkWindow *window, gchar **filename);

gint
gp_dialog_utils_show_image_save_dialog (GtkWindow   *window,
                                        const gchar *suggested_filename,
                                        gchar      **filename);

G_END_DECLS

#endif /* GP_RECTANGLE_TOOL_H_ */
