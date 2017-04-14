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

#include <glib/gi18n.h>
#include <gdk/gdk.h>

/* Signals */
enum
{
    STATUS_CHANGED,
    LAST_SIGNAL
};

static guint gp_document_signals[LAST_SIGNAL] = { 0 };
static const gchar* default_document_name = NULL;

struct _GPDocument
{
    /*< private >*/
    GObject parent_instance;

    gchar *filename;
    gboolean is_dirty;
    cairo_surface_t *base_layer;
    cairo_surface_t *active_layer;
};

G_DEFINE_TYPE (GPDocument, gp_document, G_TYPE_OBJECT)

static void
gp_document_set_filename (GPDocument *document, const gchar *filename)
{
    gboolean notify = g_strcmp0 (document->filename, filename) != 0;

    g_free (document->filename);
    document->filename = g_strdup (filename);

    if (notify)
    {
        g_signal_emit (document, gp_document_signals[STATUS_CHANGED], 0, NULL);
    }
}

static void
gp_document_init (GPDocument *document)
{
    document->active_layer = NULL;
    document->base_layer = NULL;
    document->is_dirty = FALSE;
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

    gp_document_signals[STATUS_CHANGED] =
            g_signal_new ("status-changed",
                          G_TYPE_FROM_CLASS (G_OBJECT_CLASS (klass)),
                          G_SIGNAL_RUN_FIRST,
                          0,
                          NULL,
                          NULL,
                          NULL,
                          G_TYPE_NONE, 0);

    default_document_name = _("Untitled Document");
}

void
gp_document_set_is_dirty (GPDocument *document, gboolean is_dirty)
{
    document->is_dirty = is_dirty;

    g_signal_emit (document, gp_document_signals[STATUS_CHANGED], 0);
}

gboolean
gp_document_get_is_dirty (GPDocument *document)
{
    return document->is_dirty;
}

gchar*
gp_document_get_filename (GPDocument *document)
{
    const gchar *filename = gp_document_has_custom_name (document)
            ? document->filename : default_document_name;

    return g_strdup (filename);
}

// TODO save/load impls should be moved somewhere else
void
gp_document_save_file (GPDocument *document, const gchar *filename, GError **error)
{
    GdkPixbuf *pixbuf = gdk_pixbuf_get_from_surface (document->base_layer,
                                                     0, 0,
                                                     cairo_image_surface_get_width (document->base_layer),
                                                     cairo_image_surface_get_height (document->base_layer));

    if (pixbuf == NULL)
    {
        *error = g_error_new (GDK_PIXBUF_ERROR, 0, "Cannot read pixbuf");
        return;
    }

    gdk_pixbuf_save (pixbuf, filename, "png", error, NULL); // TODO possible formats can be loaded automatically (see documentation), support parameters

    gp_document_set_filename (document, filename);
}

void
gp_document_load_file (GPDocument *document, const gchar *filename, GError **error)
{
    GdkPixbuf *pixbuf = NULL;

    g_assert (error != NULL);

    pixbuf = gdk_pixbuf_new_from_file (filename, error);

    if (*error != NULL)
    {
        goto cleanup;
    }

    if (pixbuf == NULL)
    {
        *error = g_error_new (GDK_PIXBUF_ERROR, 0, "Cannot load pixbuf from file %s", filename);
        goto cleanup;
    }

    document->base_layer = gdk_cairo_surface_create_from_pixbuf (pixbuf, 1.0, NULL);

cleanup:
    g_clear_object (&pixbuf);
}

gboolean
gp_document_has_custom_name (GPDocument *document)
{
    return document->filename != NULL;
}