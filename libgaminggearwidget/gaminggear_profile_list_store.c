/*
 * This file is part of roccat-tools.
 *
 * roccat-tools is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * roccat-tools is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with roccat-tools. If not, see <http://www.gnu.org/licenses/>.
 */

#include "gaminggear_profile_list_store.h"
#include "gtk_gaminggear_helper.h"

#define GAMINGGEAR_PROFILE_LIST_STORE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GAMINGGEAR_PROFILE_LIST_STORE_TYPE, GaminggearProfileListStoreClass))
#define IS_GAMINGGEAR_PROFILE_LIST_STORE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GAMINGGEAR_PROFILE_LIST_STORE_TYPE))

typedef struct _GaminggearProfileListStoreClass GaminggearProfileListStoreClass;

struct _GaminggearProfileListStoreClass {
	GtkListStoreClass parent_class;
};

G_DEFINE_TYPE(GaminggearProfileListStore, gaminggear_profile_list_store, GTK_TYPE_LIST_STORE);

enum {
	GAMINGGEAR_PROFILE_LIST_STORE_PROFILE_COLUMN,
	GAMINGGEAR_PROFILE_LIST_STORE_N_COLUMNS
};

GaminggearProfileListStore *gaminggear_profile_list_store_new(void) {
	GaminggearProfileListStore *list_store;

	list_store = g_object_new(GAMINGGEAR_PROFILE_LIST_STORE_TYPE, NULL);

	return list_store;
}

static void gaminggear_profile_list_store_init(GaminggearProfileListStore *list_store) {
	GType types[GAMINGGEAR_PROFILE_LIST_STORE_N_COLUMNS] = {
		G_TYPE_OBJECT,
	};

	gtk_list_store_set_column_types(GTK_LIST_STORE(list_store),
			GAMINGGEAR_PROFILE_LIST_STORE_N_COLUMNS, types);
}

static void gaminggear_profile_list_store_class_init(GaminggearProfileListStoreClass *klass) {
}

void gaminggear_profile_list_store_remove(GaminggearProfileListStore *list_store, GtkTreeIter *iter) {
	gtk_list_store_remove(GTK_LIST_STORE(list_store), iter);
}

void gaminggear_profile_list_store_add_profile(GaminggearProfileListStore *list_store, GtkTreeIter *iter, GaminggearProfileData *profile_data) {
	gtk_gaminggear_list_store_append_with_values(GTK_LIST_STORE(list_store), iter,
				GAMINGGEAR_PROFILE_LIST_STORE_PROFILE_COLUMN, profile_data,
				-1);
}

GaminggearProfileData *gaminggear_profile_list_store_get_profile(GaminggearProfileListStore *list_store, GtkTreeIter *iter) {
	GaminggearProfileData *profile_data;

	gtk_tree_model_get(GTK_TREE_MODEL(list_store), iter,
			GAMINGGEAR_PROFILE_LIST_STORE_PROFILE_COLUMN, &profile_data,
			-1);

	return profile_data;
}

void gaminggear_profile_list_store_clear(GaminggearProfileListStore *list_store) {
	gtk_list_store_clear(GTK_LIST_STORE(list_store));
}
