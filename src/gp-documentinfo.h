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

#include <glib-object.h>

G_BEGIN_DECLS

#define GP_TYPE_DOCUMENT_INFO (gp_document_info_get_type ())
G_DECLARE_FINAL_TYPE (GPDocumentInfo, gp_document_info, GP, DOCUMENT_INFO, GObject)

typedef gboolean (*save_file_request_callback) (gpointer user_data);

GPDocumentInfo* gp_document_info_create ();

void gp_document_info_set_save_file_request_callback (GPDocumentInfo *document_info,
                                                   save_file_request_callback callback,
                                                   gpointer user_data);

void gp_document_info_set_filename (GPDocumentInfo *document_info, const gchar *filename);

gboolean gp_document_info_is_modified (GPDocumentInfo *document_info);

gchar* gp_document_info_get_filename (GPDocumentInfo *document_info);

G_END_DECLS

#endif /* GP_DOCUMENT_INFO_H_ */
