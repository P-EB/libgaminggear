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

#include "gaminggearwidget_helper.h"
#include <string.h>

gboolean gtk_gaminggear_tree_model_get_iter_last(GtkTreeModel *model, GtkTreeIter *iter, GtkTreeIter *parent) {
	gint n;
	n = gtk_tree_model_iter_n_children(model, parent);
	if (n == 0)
		return FALSE;
	return gtk_tree_model_iter_nth_child(model, iter, parent, n - 1);
}

gboolean gtk_gaminggear_tree_model_get_iter_previous(GtkTreeModel *model, GtkTreeIter *iter) {
	GtkTreePath *path;
	gboolean retval;

	path = gtk_tree_model_get_path(model, iter);
	if (!gtk_tree_path_prev(path)) {
		retval = FALSE;
		goto exit;
	}
	retval = gtk_tree_model_get_iter(model, iter, path);
exit:
	gtk_tree_path_free(path);
	return retval;
}

gboolean gtk_gaminggear_tree_row_reference_get_iter(GtkTreeRowReference *reference, GtkTreeIter *iter) {
	GtkTreePath *path;
	GtkTreeModel *model;
	gboolean retval;

	model = gtk_tree_row_reference_get_model(reference);
	path = gtk_tree_row_reference_get_path(reference);
	retval = gtk_tree_model_get_iter(model, iter, path);
	gtk_tree_path_free(path);
	return retval;
}

GtkTreeRowReference *gtk_gaminggear_tree_row_reference_new_from_iter(GtkTreeModel *model, GtkTreeIter *iter) {
	GtkTreeRowReference *reference;
	GtkTreePath *path;

	path = gtk_tree_model_get_path(model, iter);
	reference = gtk_tree_row_reference_new(model, path);
	gtk_tree_path_free(path);

	return reference;
}

gboolean gtk_gaminggear_tree_model_iter_has_parent(GtkTreeModel *tree_model, GtkTreeIter *child) {
	GtkTreeIter temp_iter;
	return gtk_tree_model_iter_parent(tree_model, &temp_iter, child);
}

void gtk_gaminggear_table_clear(GtkTable *table) {
	GList *list, *children;

	children = gtk_container_get_children(GTK_CONTAINER(table));

	for (list = children; list; list = g_list_next(list))
		gtk_container_remove(GTK_CONTAINER(table), list->data);

	g_list_free(children);

	gtk_table_resize(table, 1, 1);
}

void gtk_gaminggear_tree_store_append_with_values(GtkTreeStore *tree_store, GtkTreeIter *iter, GtkTreeIter *parent, ...) {
	va_list var_args;
	GtkTreeIter tmp_iter;

	if (!iter)
		iter = &tmp_iter;

	gtk_tree_store_append(tree_store, iter, parent);
	va_start(var_args, parent);
	gtk_tree_store_set_valist(tree_store, iter, var_args);
	va_end(var_args);
}

void gtk_gaminggear_tree_view_toggle_row_expansion(GtkTreeView *treeview, GtkTreePath *path, gboolean open_all) {
	if (gtk_tree_view_row_expanded(treeview, path))
		gtk_tree_view_collapse_row(treeview, path);
	else
		gtk_tree_view_expand_row(treeview, path, open_all);
}

gboolean gtk_gaminggear_tree_view_search_string_case_equal_func(GtkTreeModel *model, gint column, gchar const *key, GtkTreeIter *iter, gpointer dummy) {
	gchar *string;
	gchar *folded_string, *folded_key;
	gboolean result;
	guint byte_size;

	gtk_tree_model_get(model, iter, column, &string, -1);
	byte_size = strlen(key);

	folded_string = g_utf8_casefold(string, byte_size);
	folded_key = g_utf8_casefold(key, byte_size);

	result = strncmp(folded_string, folded_key, byte_size) == 0 ? FALSE : TRUE;

	g_free(folded_string);
	g_free(folded_key);
	g_free(string);

	return result;
}

gint gtk_gaminggear_tree_iter_string_case_compare_func(GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer pcolumn) {
	gchar *a_string;
	gchar *b_string;
	gint retval;
	gint column = GPOINTER_TO_INT(pcolumn);

	gtk_tree_model_get(model, a, column, &a_string, -1);
	gtk_tree_model_get(model, b, column, &b_string, -1);

	if (a_string == NULL && b_string == NULL)
		retval = 0;
	else if (a_string == NULL)
		retval = 1;
	else if (b_string == NULL)
		retval = -1;
	else
		retval = g_ascii_strcasecmp(a_string, b_string);

	g_free(a_string);
	g_free(b_string);

	return retval;
}
