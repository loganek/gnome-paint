/* gp-documentinfo.h
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

#ifndef GP_DOCUMENT_INFO_H_
#define GP_DOCUMENT_INFO_H_

#include "gp-tool.h"

G_BEGIN_DECLS

#define GP_TYPE_DOCUMENT_INFO (gp_document_info_get_type ())
G_DECLARE_FINAL_TYPE (GPDocumentInfo, gp_document_info, GP, DOCUMENT_INFO, GObject)

typedef gboolean (*save_file_request_callback) (gpointer user_data);

gchar* gp_document_info_get_filename (GPDocumentInfo *document_info);

void gp_document_info_set_pixbuf (GPDocumentInfo *document, GdkPixbuf *pixbuf);

void gp_document_info_save_file (GPDocumentInfo *document, const gchar *filename, GError **error);

gboolean gp_document_info_has_custom_name (GPDocumentInfo *document);

G_END_DECLS

#endif /* GP_DOCUMENT_INFO_H_ */
