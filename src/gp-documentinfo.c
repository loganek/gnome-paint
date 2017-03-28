/* gp-documentinfo.c
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

#include "gp-documentinfo.h"

enum
{
    SIGNAL_STATE_CHANGED,
    LAST_SIGNAL
};

static guint gp_document_info_signals[LAST_SIGNAL] = { 0 };

struct _GPDocumentInfo
{
    GObject parent_instance;

    gboolean modified;
    gchar *filename;
};

G_DEFINE_TYPE (GPDocumentInfo, gp_document_info, G_TYPE_OBJECT)

static void
gp_document_info_finalize (GObject *gobj)
{
    GPDocumentInfo *document_info = GP_DOCUMENT_INFO (gobj);

    g_free (document_info->filename);
}

static void
gp_document_info_init (GPDocumentInfo *self)
{
    self->filename = NULL;
    self->modified = FALSE;
}

static void
gp_document_info_class_init (GPDocumentInfoClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    gobject_class->finalize = gp_document_info_finalize;

    gp_document_info_signals[SIGNAL_STATE_CHANGED] =
            g_signal_new ("state-changed",
                          G_TYPE_FROM_CLASS (klass),
                          0,
                          0,
                          NULL,
                          NULL,
                          NULL,
                          G_TYPE_NONE, 0);
}

GPDocumentInfo*
gp_document_info_create ()
{
    return  GP_DOCUMENT_INFO (g_object_new (GP_TYPE_DOCUMENT_INFO, NULL));
}

void
gp_document_info_set_filename (GPDocumentInfo *document_info, const gchar *filename)
{
    g_free (document_info->filename);

    document_info->filename = g_strdup (filename);
    document_info->modified = FALSE;

    g_signal_emit (document_info, gp_document_info_signals[SIGNAL_STATE_CHANGED], 0, NULL);
}

gboolean
gp_document_info_is_modified (GPDocumentInfo *document_info)
{
    return document_info->modified;
}

gchar*
gp_document_info_get_filename (GPDocumentInfo *document_info)
{
    return g_strdup (document_info->filename);
}
