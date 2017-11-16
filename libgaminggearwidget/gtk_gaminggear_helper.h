#ifndef __GAMINGGEAR_GTK_HELPER_H__
#define __GAMINGGEAR_GTK_HELPER_H__

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

#include <gtk/gtk.h>

G_BEGIN_DECLS

/* recursive search */
gboolean gtk_gaminggear_tree_model_iter_find_int(GtkTreeModel *model, gint key, gint value, GtkTreeIter *result, GtkTreeIter *first);

void gtk_gaminggear_list_store_append_with_values(GtkListStore *list_store, GtkTreeIter *iter, ...);

G_END_DECLS

#endif
