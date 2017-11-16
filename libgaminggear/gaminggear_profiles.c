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

#include "gaminggear/profiles.h"
#include "gaminggear_helper.h"
#include <string.h>

struct _GaminggearProfiles {
	GType type;
	GSList *list;
	gchar *path;
};

GSList *gaminggear_profiles_first(GaminggearProfiles *profiles) {
	return profiles->list;
}

static gint find_by_hardware_index(gconstpointer a, gconstpointer b) {
	gint first = gaminggear_profile_data_get_hardware_index(GAMINGGEAR_PROFILE_DATA(a));
	gint second = GPOINTER_TO_INT(b);

	return first - second;
}

static gint find_by_name(gconstpointer a, gconstpointer b) {
	gchar const *first = gaminggear_profile_data_get_name(GAMINGGEAR_PROFILE_DATA(a));
	gchar const *second = (gchar const *)b;

	return strcmp(first, second);
}

GaminggearProfileData *gaminggear_profiles_find_by_hardware_index(GaminggearProfiles *profiles, gint hardware_index) {
	GSList *element;

	element = g_slist_find_custom(profiles->list, GINT_TO_POINTER(hardware_index), find_by_hardware_index);
	if (element)
		return GAMINGGEAR_PROFILE_DATA(element->data);

	return NULL;
}

GaminggearProfileData *gaminggear_profiles_find_by_name(GaminggearProfiles *profiles, gchar const *name) {
	GSList *element;

	element = g_slist_find_custom(profiles->list, name, find_by_name);
	if (element)
		return GAMINGGEAR_PROFILE_DATA(element->data);
	
	return NULL;
}

void gaminggear_profiles_activate(GaminggearProfiles *profiles, GaminggearProfileData *target_profile, gint hardware_index) {
	GaminggearProfileData *hardware_profile = gaminggear_profiles_find_by_hardware_index(profiles, hardware_index);

	if (hardware_profile)
		gaminggear_profile_data_set_hardware_index(hardware_profile, GAMINGGEAR_PROFILE_DATA_GAMEFILE_PROFILE_INVALID);

	gaminggear_profile_data_set_hardware_index(target_profile, hardware_index);
}

gboolean gaminggear_profiles_activate_per_name(GaminggearProfiles *profiles, gchar const *name, gint hardware_index) {
	GaminggearProfileData *named_profile = gaminggear_profiles_find_by_name(profiles, name);

	if (!named_profile)
		return FALSE;

	gaminggear_profiles_activate(profiles, named_profile, hardware_index);
	return TRUE;
}

gboolean gaminggear_profiles_store(GaminggearProfiles *profiles, GaminggearDevice *device, GError **error) {
	GSList *iterator;
	GaminggearProfileData *profile_data;

	for (iterator = profiles->list; iterator; iterator = g_slist_next(iterator)) {
		profile_data = GAMINGGEAR_PROFILE_DATA(iterator->data);
		if (!gaminggear_profile_data_store(profile_data, device, error))
			return FALSE;
	}

	return TRUE;
}

gboolean gaminggear_profiles_fill(GaminggearProfiles *profiles, GaminggearDevice *device, guint count, GError **error) {
	GSList *element;
	GaminggearProfileData *profile_data;
	gint i;

	for (i = 0; i < (gint)count; ++i) {
		element = g_slist_find_custom(profiles->list, GINT_TO_POINTER(i), find_by_hardware_index);
		if (!element) {
			profile_data = GAMINGGEAR_PROFILE_DATA(g_object_new(profiles->type, NULL));;
			gaminggear_profile_data_set_hardware_index(profile_data, i);
			profiles->list = g_slist_prepend(profiles->list, profile_data);
			if (!gaminggear_profile_data_read(profile_data, device, error))
				return FALSE;
		}
	}

	return TRUE;
}

gboolean gaminggear_profiles_read(GaminggearProfiles *profiles, GaminggearDevice *device, GError **error) {
	GSList *iterator;
	GaminggearProfileData *profile_data;

	for (iterator = profiles->list; iterator; iterator = g_slist_next(iterator)) {
		profile_data = GAMINGGEAR_PROFILE_DATA(iterator->data);
		if (!gaminggear_profile_data_read(profile_data, device, error))
			return FALSE;
	}

	return TRUE;
}

gboolean gaminggear_profiles_load(GaminggearProfiles *profiles, GError **error) {
	GaminggearProfileData *profile_data;
	GDir *directory;
	gchar const *name;
	gchar *abs_path;
	gboolean retval;

	directory = g_dir_open(profiles->path, 0, error);
	if (directory == NULL)
		return FALSE;

	while ((name = g_dir_read_name(directory))) {
		profile_data = GAMINGGEAR_PROFILE_DATA(g_object_new(profiles->type, NULL));
		abs_path = g_build_filename(profiles->path, name, NULL);
		retval = gaminggear_profile_data_load(profile_data, abs_path, error);
		g_free(abs_path);

		if (!retval) {
			g_object_unref(G_OBJECT(profile_data));
			g_dir_close(directory);
			// TODO warning and continue
			return FALSE;
		}

		profiles->list = g_slist_prepend(profiles->list, profile_data);
	}

	g_dir_close(directory);

	return TRUE;
}
GaminggearProfiles *gaminggear_profiles_new(GType type, gchar const *path) {
	GaminggearProfiles *profiles;

	profiles = (GaminggearProfiles *)g_malloc(sizeof(GaminggearProfiles));
	profiles->type = type;
	profiles->path = g_strdup(path);
	profiles->list = NULL;
	return profiles;
}

void gaminggear_profiles_free(GaminggearProfiles *profiles) {
	g_slist_free_full(profiles->list, g_object_unref);
	g_free(profiles->path);
	g_free(profiles);
}
