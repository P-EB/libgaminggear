#ifndef __GAMINGGEAR_PROFILE_LIST_STORE_H__
#define __GAMINGGEAR_PROFILE_LIST_STORE_H__

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

#include "gaminggear/gaminggear_profile_data.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GAMINGGEAR_PROFILE_LIST_STORE_TYPE (gaminggear_profile_list_store_get_type())
#define GAMINGGEAR_PROFILE_LIST_STORE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GAMINGGEAR_PROFILE_LIST_STORE_TYPE, GaminggearProfileListStore))
#define IS_GAMINGGEAR_PROFILE_LIST_STORE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GAMINGGEAR_PROFILE_LIST_STORE_TYPE))

typedef struct _GaminggearProfileListStore GaminggearProfileListStore;

struct _GaminggearProfileListStore {
	GtkListStore parent;
};

GType gaminggear_profile_list_store_get_type(void);

GaminggearProfileListStore *gaminggear_profile_list_store_new(void);

guint gaminggear_profile_list_store_name_column(void);

void gaminggear_profile_list_store_remove(GaminggearProfileListStore *list_store, GtkTreeIter *iter);

void gaminggear_profile_list_store_add_profile(GaminggearProfileListStore *list_store, GtkTreeIter *iter, GaminggearProfileData *profile_data);

/* unref */
GaminggearProfileData *gaminggear_profile_list_store_get_profile(GaminggearProfileListStore *list_store, GtkTreeIter *iter);

void gaminggear_profile_list_store_clear(GaminggearProfileListStore *list_store);

G_END_DECLS

#endif
