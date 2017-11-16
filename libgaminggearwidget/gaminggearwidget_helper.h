#ifndef __GAMINGGEAR_H__
#define __GAMINGGEAR_H__

/*
 * This file is part of libgaminggear.
 *
 * libgaminggear is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * libgaminggear is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libgaminggear. If not, see <http://www.gnu.org/licenses/>.
 */

#include <gtk/gtk.h>

G_BEGIN_DECLS

gboolean gtk_gaminggear_tree_model_get_iter_last(GtkTreeModel *model, GtkTreeIter *iter, GtkTreeIter *parent);

gboolean gtk_gaminggear_tree_model_get_iter_previous(GtkTreeModel *model, GtkTreeIter *iter);

gboolean gtk_gaminggear_tree_row_reference_get_iter(GtkTreeRowReference *reference, GtkTreeIter *iter);

GtkTreeRowReference *gtk_gaminggear_tree_row_reference_new_from_iter(GtkTreeModel *model, GtkTreeIter *iter);

gboolean gtk_gaminggear_tree_model_iter_has_parent(GtkTreeModel *tree_model, GtkTreeIter *child);

void gtk_gaminggear_table_clear(GtkTable *table);

void gtk_gaminggear_tree_store_append_with_values(GtkTreeStore *tree_store, GtkTreeIter *iter, GtkTreeIter *parent, ...);

void gtk_gaminggear_tree_view_toggle_row_expansion(GtkTreeView *treeview, GtkTreePath *path, gboolean open_all);

/* case insensitive search */
gboolean gtk_gaminggear_tree_view_search_string_case_equal_func(GtkTreeModel *model, gint column, gchar const *key, GtkTreeIter *iter, gpointer dummy);

/* case insensitive sort */
gint gtk_gaminggear_tree_iter_string_case_compare_func(GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer pcolumn);

G_END_DECLS

#endif
