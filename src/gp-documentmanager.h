/* gp-documentmanager.h
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

#ifndef GP_DOCUMENT_MANAGER_H_
#define GP_DOCUMENT_MANAGER_H_

#include "gp-documentinfo.h"

#include <glib-object.h>

G_BEGIN_DECLS

#define GP_TYPE_DOCUMENT_MANAGER (gp_document_manager_get_type ())
G_DECLARE_FINAL_TYPE (GPDocumentManager, gp_document_manager, GP, DOCUMENT_MANAGER, GObject)

GPDocumentManager * gp_document_manager_get_default (void);

GPDocumentInfo* gp_document_manager_create_document (GPDocumentManager *manager);

GPDocumentInfo * gp_document_manager_get_active_document (GPDocumentManager *manager);

void gp_document_manager_set_active_document (GPDocumentManager *manager, GPDocumentInfo *document);

G_END_DECLS

#endif /* GP_DOCUMENT_MANAGER_H_ */
