#ifndef __GAMINGGEAR_CONFIGURATION_H__
#define __GAMINGGEAR_CONFIGURATION_H__

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

#include "gaminggear_key_file.h"

G_BEGIN_DECLS

gchar *gaminggear_configuration_dir(void);
GaminggearKeyFile *gaminggear_configuration_load(void);
gboolean gaminggear_configuration_save(GaminggearKeyFile *config, GError **error);
void gaminggear_configuration_free(GaminggearKeyFile *config);
gchar *gaminggear_configuration_get_timer_save_path(GaminggearKeyFile *config);
void gaminggear_configuration_set_timer_save_path(GaminggearKeyFile *config, gchar const *path);
gchar *gaminggear_configuration_get_macro_save_path(GaminggearKeyFile *config);
void gaminggear_configuration_set_macro_save_path(GaminggearKeyFile *config, gchar const *path);

G_END_DECLS

#endif
