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

#include "gaminggear_macros.h"
#include "gaminggear_helper.h"
#include "gaminggear_configuration.h"
#include <errno.h>

struct _GaminggearMacros {
	GKeyFile *key_file;
};

/*!
 * \brief Return path of macro file
 * \retval Path that has to be freed with g_free()
 */
static gchar *gaminggear_macros_path(void) {
	gchar *dir = gaminggear_configuration_dir();
	gchar *path = g_build_path("/", dir, "gaminggear_macros.ini", NULL);
	g_free(dir);
	return path;
}

GaminggearMacros *gaminggear_macros_new(void) {
	GaminggearMacros *gaminggear_macros;
	gaminggear_macros = g_malloc(sizeof(GaminggearMacros));
	gaminggear_macros->key_file = g_key_file_new();
	return gaminggear_macros;
}

GaminggearMacros *gaminggear_macros_load_with_path(gchar const *path, GError **error) {
	GaminggearMacros *gaminggear_macros;

	gaminggear_macros = gaminggear_macros_new();

	if (!g_key_file_load_from_file(gaminggear_macros->key_file, path, G_KEY_FILE_KEEP_COMMENTS, error)) {
		gaminggear_macros_free(gaminggear_macros);
		return NULL;
	}

	return gaminggear_macros;
}

GaminggearMacros *gaminggear_macros_load(void) {
	gchar *path;
	GaminggearMacros *gaminggear_macros;

	path = gaminggear_macros_path();
	/* file might not exist yet */
	gaminggear_macros = gaminggear_macros_load_with_path(path, NULL);
	g_free(path);
	if (gaminggear_macros == NULL)
		gaminggear_macros = gaminggear_macros_new();

	return gaminggear_macros;
}

gboolean gaminggear_macros_save_with_path(gchar const *path, GaminggearMacros *gaminggear_macros, GError **error) {
	gchar *data;
	gsize length;
	gboolean retval;

	data = g_key_file_to_data(gaminggear_macros->key_file, &length, error);
	retval = g_file_set_contents(path, data, length, error);
	g_free(data);

	return retval;
}

gboolean gaminggear_macros_save(GaminggearMacros *gaminggear_macros, GError **error) {
	gchar *dir, *path;
	gboolean retval;

	dir = gaminggear_configuration_dir();
	if (!gaminggear_create_dir_if_needed(dir, error))
		return FALSE;
	g_free(dir);

	path = gaminggear_macros_path();
	retval = gaminggear_macros_save_with_path(path, gaminggear_macros, error);
	g_free(path);

	return retval;
}

void gaminggear_macros_free(GaminggearMacros *gaminggear_macros) {
	if (gaminggear_macros) {
		g_key_file_free(gaminggear_macros->key_file);
		g_free(gaminggear_macros);
	}
}

GaminggearMacro *gaminggear_macros_get(GaminggearMacros *gaminggear_macros, gchar const *macroset_name, gchar const *macro_name, GError **error) {
	GaminggearMacroKeystrokes *gaminggear_macro_keystrokes;
	GaminggearMacro *result;

	gaminggear_macro_keystrokes = gaminggear_key_file_get_macro_keystrokes(gaminggear_macros->key_file, macroset_name, macro_name, error);
	if (!gaminggear_macro_keystrokes)
		return NULL;

	result = gaminggear_macro_new(macroset_name, macro_name, gaminggear_macro_keystrokes);
	gaminggear_macro_keystrokes_free(gaminggear_macro_keystrokes);
	return result;
}

void gaminggear_macros_set(GaminggearMacros *gaminggear_macros, GaminggearMacro const *macro) {
	g_gaminggear_key_file_set_binary(gaminggear_macros->key_file, macro->macroset, macro->macro, &macro->keystrokes, sizeof(GaminggearMacroKeystrokes));
}

gchar **gaminggear_macros_get_macrosets(GaminggearMacros *gaminggear_macros, gsize *length) {
	return g_key_file_get_groups(gaminggear_macros->key_file, length);
}

gchar **gaminggear_macros_get_macros(GaminggearMacros *gaminggear_macros, gchar const *macroset_name, gsize *length, GError **error) {
	return g_key_file_get_keys(gaminggear_macros->key_file, macroset_name, length, error);
}

gboolean gaminggear_macros_store_macro(GaminggearMacro const *gaminggear_macro, GError **error) {
	GaminggearMacros *gaminggear_macros;
	gboolean retval;

	gaminggear_macros = gaminggear_macros_load();
	gaminggear_macros_set(gaminggear_macros, gaminggear_macro);
	retval = gaminggear_macros_save(gaminggear_macros, error);
	gaminggear_macros_free(gaminggear_macros);

	return retval;
}
