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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libgaminggear. If not, see <http://www.gnu.org/licenses/>.
 */

#include "gtk_gaminggear_helper.h"

void gtk_gaminggear_list_store_append_with_values(GtkListStore *list_store, GtkTreeIter *iter, ...) {
	va_list var_args;
	GtkTreeIter tmp_iter;

	if (!iter)
		iter = &tmp_iter;

	gtk_list_store_append(list_store, iter);
	va_start(var_args, iter);
	gtk_list_store_set_valist(list_store, iter, var_args);
	va_end(var_args);
}

gboolean gtk_gaminggear_tree_model_iter_find_int(GtkTreeModel *model, gint key, gint value, GtkTreeIter *result, GtkTreeIter *first) {
	GtkTreeIter iter, first_child;
	gboolean valid, has_child;
	gint data;

	if (first) {
		iter = *first;
		valid = TRUE;
	} else {
		valid = gtk_tree_model_get_iter_first(model, &iter);
	}

	while (valid) {
		gtk_tree_model_get(model, &iter, key, &data, -1);
		if (data == value) {
			if (result != NULL)
				*result = iter;
			return TRUE;
		}
		has_child = gtk_tree_model_iter_children(model, &first_child, &iter);
		if (has_child) {
			if (gtk_gaminggear_tree_model_iter_find_int(model, key, value, result, &first_child)) {
				/* result is already set */
				return TRUE;
			}
		}
		valid = gtk_tree_model_iter_next(model, &iter);
	}
	return FALSE;
}
