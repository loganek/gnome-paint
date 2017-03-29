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

#include <glib/gi18n.h>

enum
{
    SIGNAL_STATE_CHANGED,
    LAST_SIGNAL
};

static guint gp_document_info_signals[LAST_SIGNAL] = { 0 };

static const gchar* default_document_name = NULL;

struct _GPDocumentInfo
{
    GObject parent_instance;

    gboolean modified;
    gchar *filename;
};

G_DEFINE_TYPE (GPDocumentInfo, gp_document_info, G_TYPE_OBJECT)

static void
gp_document_info_emit_state_changed (GPDocumentInfo *document_info, gboolean condition)
{
    if (condition == TRUE)
    {
        g_signal_emit (document_info, gp_document_info_signals[SIGNAL_STATE_CHANGED], 0, NULL);
    }
}

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

    default_document_name = _("Untitled Document");
}

GPDocumentInfo*
gp_document_info_create ()
{
    return  GP_DOCUMENT_INFO (g_object_new (GP_TYPE_DOCUMENT_INFO, NULL));
}

void
gp_document_info_set_filename (GPDocumentInfo *document_info, const gchar *filename)
{
    gboolean emit_signal = g_strcmp0 (document_info->filename, filename) != 0 || filename == NULL;

    g_free (document_info->filename);

    document_info->filename = g_strdup (filename);
    document_info->modified = FALSE;

    gp_document_info_emit_state_changed (document_info, emit_signal);
}

gboolean
gp_document_info_get_is_modified (GPDocumentInfo *document_info)
{
    return document_info->modified;
}

void
gp_document_info_set_is_modified (GPDocumentInfo *document_info, gboolean modified)
{
    gboolean emit_signal = document_info->modified != modified;

    document_info->modified = modified;

    gp_document_info_emit_state_changed (document_info, emit_signal);
}

gboolean
gp_document_info_has_user_defined_name (GPDocumentInfo *document_info)
{
    return document_info->filename != NULL;
}

gchar*
gp_document_info_get_filename (GPDocumentInfo *document_info)
{
    const gchar *filename = gp_document_info_has_user_defined_name (document_info)
            ? document_info->filename : default_document_name;

    return g_strdup (filename);
}
