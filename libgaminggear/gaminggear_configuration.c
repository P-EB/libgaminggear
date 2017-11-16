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

#include "gaminggear_configuration.h"

static gchar const * const group_name = "General";
static gchar const * const timer_save_path_key = "TimerFilePath";
static gchar const * const macro_save_path_key = "MacroFilePath";

gchar *gaminggear_configuration_dir(void) {
	return g_build_path("/", g_get_user_config_dir(), "gaminggear", NULL);
}

static gchar *gaminggear_configuration_path(void) {
	gchar *dir = gaminggear_configuration_dir();
	gchar *path = g_build_filename(dir, "gaminggear.ini", NULL);
	g_free(dir);
	return path;
}

GaminggearKeyFile *gaminggear_configuration_load(void) {
	GaminggearKeyFile *config;
	gchar *path;

	path = gaminggear_configuration_path();
	config = gaminggear_key_file_load(path);
	g_free(path);

	return config;
}

gboolean gaminggear_configuration_save(GaminggearKeyFile *config, GError **error) {
	if (config == NULL)
		return TRUE;

	return gaminggear_key_file_save(config, error);
}

void gaminggear_configuration_free(GaminggearKeyFile *config) {
	if (config == NULL)
		return;

	gaminggear_key_file_free(config);
}

gchar *gaminggear_configuration_get_timer_save_path(GaminggearKeyFile *config) {
	return gaminggear_key_file_get_string_with_default(config, group_name, timer_save_path_key, g_get_home_dir());
}

void gaminggear_configuration_set_timer_save_path(GaminggearKeyFile *config, gchar const *path) {
	gaminggear_key_file_set_string(config, group_name, timer_save_path_key, path);
}

gchar *gaminggear_configuration_get_macro_save_path(GaminggearKeyFile *config) {
	return gaminggear_key_file_get_string_with_default(config, group_name, macro_save_path_key, g_get_home_dir());
}

void gaminggear_configuration_set_macro_save_path(GaminggearKeyFile *config, gchar const *path) {
	gaminggear_key_file_set_string(config, group_name, macro_save_path_key, path);
}
