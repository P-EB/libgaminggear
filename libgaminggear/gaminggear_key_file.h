#ifndef __GAMINGGEAR_KEY_FILE_H__
#define __GAMINGGEAR_KEY_FILE_H__

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

#include <glib.h>

G_BEGIN_DECLS

typedef struct _GaminggearKeyFile GaminggearKeyFile;

GaminggearKeyFile *gaminggear_key_file_load(gchar const *filename);

/*
 * Does not create dir
 */
gboolean gaminggear_key_file_save(GaminggearKeyFile *key_file, GError **error);

gboolean gaminggear_key_file_save_as(GaminggearKeyFile *key_file, gchar const *filename, GError **error);
void gaminggear_key_file_free(GaminggearKeyFile *key_file);

gchar *gaminggear_key_file_get_string_with_default(GaminggearKeyFile *key_file, char const *group, gchar const *key, gchar const *defval);
void gaminggear_key_file_set_string(GaminggearKeyFile *key_file, char const *group, gchar const *key, gchar const *new_value);

gint gaminggear_key_file_get_integer_with_default(GaminggearKeyFile *key_file, char const *group, gchar const *key, gint defval);
void gaminggear_key_file_set_integer(GaminggearKeyFile *key_file, char const *group, gchar const *key, gint new_value);

gboolean gaminggear_key_file_get_boolean_with_default(GaminggearKeyFile *key_file, char const *group, gchar const *key, gboolean defval);
void gaminggear_key_file_set_boolean(GaminggearKeyFile *key_file, char const *group, gchar const *key, gboolean new_value);

gdouble gaminggear_key_file_get_double_with_default(GaminggearKeyFile *key_file, char const *group, gchar const *key, gdouble defval);
void gaminggear_key_file_set_double(GaminggearKeyFile *key_file, char const *group, gchar const *key, gdouble new_value);

G_END_DECLS

#endif
