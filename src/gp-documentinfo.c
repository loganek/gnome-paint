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
    SIGNAL_CHANGED,
    LAST_SIGNAL
};

static guint gp_document_info_signals[LAST_SIGNAL] = { 0 };

static const gchar* default_document_name = NULL;

struct _GPDocumentInfo
{
    GObject parent_instance;

    GdkPixbuf *pixbuf;

    gboolean modified;
    gchar *filename;
};

G_DEFINE_TYPE (GPDocumentInfo, gp_document_info, G_TYPE_OBJECT)

static void
gp_document_info_set_filename (GPDocumentInfo *document, const gchar *filename)
{
    gboolean notify = g_strcmp0 (document->filename, filename) != 0;

    g_free (document->filename);
    document->filename = g_strdup (filename);

    if (notify)
    {
        g_signal_emit (document, gp_document_info_signals[SIGNAL_CHANGED], 0, NULL);
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
    self->pixbuf = NULL;
}

static void
gp_document_info_class_init (GPDocumentInfoClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    gobject_class->finalize = gp_document_info_finalize;

    default_document_name = _("Untitled Document");

    gp_document_info_signals[SIGNAL_CHANGED] =
            g_signal_new ("changed",
                          G_TYPE_FROM_CLASS (klass),
                          0,
                          0,
                          NULL,
                          NULL,
                          NULL,
                          G_TYPE_NONE, 0);
}

gchar*
gp_document_info_get_filename (GPDocumentInfo *document_info)
{
    const gchar *filename = gp_document_info_has_custom_name (document_info)
            ? document_info->filename : default_document_name;

    return g_strdup (filename);
}

void
gp_document_info_set_pixbuf (GPDocumentInfo *document, GdkPixbuf *pixbuf)
{
    if (document->pixbuf != NULL)
    {
        g_object_unref (document->pixbuf);
    }
    document->pixbuf = pixbuf;
}

void
gp_document_info_save_file (GPDocumentInfo *document, const gchar *filename, GError **error)
{
    g_return_if_fail (document->pixbuf != NULL);

    gdk_pixbuf_save (document->pixbuf, filename, "png", error, NULL); // TODO possible formats can be loaded automatically (see documentation), support parameters

    gp_document_info_set_filename (document, filename);
}

gboolean
gp_document_info_has_custom_name (GPDocumentInfo *document)
{
    return document->filename != NULL;
}
