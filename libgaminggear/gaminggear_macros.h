#ifndef __GAMINGGEAR_MACROS_H__
#define __GAMINGGEAR_MACROS_H__

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

#include "gaminggear/macro.h"

G_BEGIN_DECLS

typedef struct _GaminggearMacros GaminggearMacros;

/*!
 * \brief created new empty macros
 * \retval key file that has to be freed with \c gaminggear_macros_free()
 */
GaminggearMacros *gaminggear_macros_new(void);

GaminggearMacros *gaminggear_macros_load_with_path(gchar const *path, GError **error);

/*!
 * \brief load macro file
 * Returns key file even if file does not exist
 * \retval key file that has to be freed with \c gaminggear_macros_free()
 */
GaminggearMacros *gaminggear_macros_load(void);

gboolean gaminggear_macros_save_with_path(gchar const *path, GaminggearMacros *gaminggear_macros, GError **error);

/*!
 * \brief save macro file to disk
 * \retval If successful returns +TRUE+. On failure, returns +FALSE+ with +error+ set
 */
gboolean gaminggear_macros_save(GaminggearMacros *gaminggear_macros, GError **error);

/*!
 * \brief frees macro file
 */
void gaminggear_macros_free(GaminggearMacros *key_file);

/*!
 * \brief gets macro
 */
GaminggearMacro *gaminggear_macros_get(GaminggearMacros *gaminggear_macros, gchar const *macroset_name, gchar const *macro_name, GError **error);

/*!
 * \brief sets macro
 */
void gaminggear_macros_set(GaminggearMacros *gaminggear_macros, GaminggearMacro const *macro);

/*!
 * \brief returns array of macroset names
 * \retval NULL-terminated array of strings
 *
 * Use g_strfreev() to free array
 */
gchar **gaminggear_macros_get_macrosets(GaminggearMacros *gaminggear_macros, gsize *length);

/*!
 * \brief returns array of macro names for macro set
 * \retval NULL-terminated array of strings
 *
 * Use \c g_strfreev() to free array
 */
gchar **gaminggear_macros_get_macros(GaminggearMacros *gaminggear_macros, gchar const *macroset_name, gsize *length, GError **error);

G_END_DECLS

#endif
