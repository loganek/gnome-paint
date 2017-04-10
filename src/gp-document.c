/* gp-document.c
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

#include "gp-document.h"

#include "cairo.h"

struct _GPDocument
{
    /*< private >*/
    GObject parent_instance;

    gchar *filename;
    gboolean is_modified;
    cairo_surface_t *base_layer;
    cairo_surface_t *active_layer;
};

G_DEFINE_TYPE (GPDocument, gp_document, G_TYPE_OBJECT)

static void
gp_document_init (GPDocument *document)
{
    document->active_layer = NULL;
    document->base_layer = NULL;
    document->is_modified = FALSE;
    document->filename = NULL;
}

void
gp_document_finalize (GObject *gobj)
{
    GPDocument *document = GP_DOCUMENT (gobj);

    g_free (document->filename);
    document->filename = NULL;

    g_clear_object (&document->active_layer);
    g_clear_object (&document->base_layer);
}

static void
gp_document_class_init (GPDocumentClass *klass)
{
    GObjectClass *gobject_class;

    gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->finalize = gp_document_finalize;
}
