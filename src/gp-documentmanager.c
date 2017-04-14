/* gp-documentmanager.c
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

#include "config.h"

#include "gp-documentmanager.h"
#include "gp-marshal.h"

/* Signals */
enum
{
    ACTIVE_DOCUMENT_CHANGED,
    DOCUMENT_STATUS_CHANGED,
    LAST_SIGNAL
};

static guint gp_document_manager_signals[LAST_SIGNAL] = { 0 };

struct _GPDocumentManager
{
    /*< private >*/
    GObject parent_instance;

    GSList *documents;
    GPDocument *active_document;
};

G_DEFINE_TYPE (GPDocumentManager, gp_document_manager, G_TYPE_OBJECT)

static void on_document_dirty_changed (GPDocument *document, gpointer user_data)
{
    GPDocumentManager *manager = GP_DOCUMENT_MANAGER (user_data);

    g_signal_emit (manager, gp_document_manager_signals[DOCUMENT_STATUS_CHANGED], 0, document, NULL);
}

static void
gp_document_manager_init (GPDocumentManager *self)
{
    self->documents = NULL;
}

static void
gp_document_manager_finalize (GObject *self)
{
    GPDocumentManager *manager = GP_DOCUMENT_MANAGER (self);

    g_slist_free_full (manager->documents, g_object_unref);
}

static void
gp_document_manager_class_init (GPDocumentManagerClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    gp_document_manager_signals[DOCUMENT_STATUS_CHANGED] =
            g_signal_new ("document-status-changed",
                          G_TYPE_FROM_CLASS (G_OBJECT_CLASS (klass)),
                          G_SIGNAL_RUN_FIRST,
                          0,
                          NULL,
                          NULL,
                          gp_VOID__BOXED,
                          G_TYPE_NONE, 1, GP_TYPE_DOCUMENT);

    gp_document_manager_signals[ACTIVE_DOCUMENT_CHANGED] =
            g_signal_new ("active-document-changed",
                          G_TYPE_FROM_CLASS (G_OBJECT_CLASS (klass)),
                          G_SIGNAL_RUN_FIRST,
                          0,
                          NULL,
                          NULL,
                          NULL,
                          G_TYPE_NONE, 0, NULL);

    gobject_class->finalize = gp_document_manager_finalize;
}

GPDocument *
gp_document_manager_get_active_document (GPDocumentManager *manager)
{
    return manager->active_document;
}

GPDocumentManager *
gp_document_manager_get_default (void)
{
    static GPDocumentManager *manager = NULL;

    if (manager == NULL)
    {
        manager = GP_DOCUMENT_MANAGER (g_object_new (GP_TYPE_DOCUMENT_MANAGER, NULL));
    }

    return manager;
}

GPDocument*
gp_document_manager_create_new_document (GPDocumentManager *manager)
{
    GPDocument *document = GP_DOCUMENT (g_object_new (GP_TYPE_DOCUMENT, NULL));

    if (manager->documents == NULL)
    {
        gp_document_manager_set_active_document (manager, document);
    }

    manager->documents = g_slist_append (manager->documents, document);

    g_signal_connect(G_OBJECT (document), "status-changed",
                     G_CALLBACK (on_document_dirty_changed),
                     manager);

    return document;
}

GPDocument*
gp_document_manager_create_document_from_file (GPDocumentManager *manager, GFile *file)
{
    GPDocument *document = gp_document_manager_create_new_document (manager);
    GError *error = NULL;

    g_return_val_if_fail (file != NULL, document);

    gp_document_load_file (document, g_file_get_path (file), &error); // TODO error handling

    return document;
}

void
gp_document_manager_set_active_document (GPDocumentManager *manager, GPDocument *document)
{
    if (document == manager->active_document)
    {
        return;
    }

    g_return_if_fail (g_slist_find (manager->documents, document) == NULL);

    manager->active_document = document;
    g_signal_emit (manager, gp_document_manager_signals[ACTIVE_DOCUMENT_CHANGED], 0);
}
