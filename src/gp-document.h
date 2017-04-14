/* gp-document.h
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

#ifndef GP_DOCUMENT_H_
#define GP_DOCUMENT_H_

#include <glib-object.h>

G_BEGIN_DECLS

#define GP_TYPE_DOCUMENT (gp_document_get_type ())
G_DECLARE_FINAL_TYPE (GPDocument, gp_document, GP, DOCUMENT, GObject)

void gp_document_set_is_dirty (GPDocument *document, gboolean is_dirty);
gboolean gp_document_get_is_dirty (GPDocument *document);

gchar* gp_document_get_filename (GPDocument *document);

void gp_document_save_file (GPDocument*document, const gchar *filename, GError **error);
void gp_document_load_file (GPDocument*document, const gchar *filename, GError **error);

gboolean gp_document_has_custom_name (GPDocument *document);

G_END_DECLS

#endif /* GP_DOCUMENT_H_ */
