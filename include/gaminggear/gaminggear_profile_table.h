#ifndef __GAMINGGEAR_PROFILE_TABLE_H__
#define __GAMINGGEAR_PROFILE_TABLE_H__

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

/*! \file gaminggear/gaminggear_profile_table.h
 *  \brief GaminggearProfileTable
 */

#include "gaminggear/gaminggear_device.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GAMINGGEAR_PROFILE_TABLE_TYPE (gaminggear_profile_table_get_type())
#define GAMINGGEAR_PROFILE_TABLE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GAMINGGEAR_PROFILE_TABLE_TYPE, GaminggearProfileTable))
#define IS_GAMINGGEAR_PROFILE_TABLE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GAMINGGEAR_PROFILE_TABLE_TYPE))

typedef struct _GaminggearProfileTable GaminggearProfileTable;
typedef struct _GaminggearProfileTablePrivate GaminggearProfileTablePrivate;

struct _GaminggearProfileTable {
	GtkTreeView parent;
	GaminggearProfileTablePrivate *priv;
};

GType gaminggear_profile_table_get_type(void);

/*! \brief Create new profile table.
 *  \param type Type of profile_data to handle.
 *  \param path Path to load files from.
 *  \param profile_num Maximum count of profiles the device can handle.
 *  \retval new profile table.
 *  \since 1.0
 */
GtkWidget *gaminggear_profile_table_new(GType type, gchar const *path, guint profile_num);

/*! \brief Add profile.
 *
 *  Adds new profile_data with default values.
 *
 *  \param table Instance.
 *  \since 1.0
 */
void gaminggear_profile_table_add_profile(GaminggearProfileTable *table);

/*! \brief Load filesystem profile_data.
 *
 *  Filesystem data has to be read before hardware data since it contains the
 *  hardware profile index.
 *
 *  \param table Instance.
 *  \param error Location to return an error.
 *  \retval bool \c TRUE if successful, else \c FALSE.
 *  \since 1.0
 */
gboolean gaminggear_profile_table_load(GaminggearProfileTable *table, GError **error);

/*! \brief Fill profile data list with missing hardware profiles.
 *
 *  Filesystem data has to be read before hardware data since it contains the
 *  hardware profile index.
 *
 *  Only reads the missing profiles from hardware.
 *
 *  \param table Instance.
 *  \param device Device to read from or \c NULL.
 *  \param count Number of profiles to fill.
 *  \param error Location to return an error.
 *  \retval bool \c TRUE if successful, else \c FALSE.
 *  \since 1.0
 */
gboolean gaminggear_profile_table_fill(GaminggearProfileTable *table, GaminggearDevice *device, guint count, GError **error);

/*! \brief Read hardware profile_data.
 *
 *  Filesystem data has to be read before hardware data since it contains the
 *  hardware profile index.
 *
 *  \param table Instance.
 *  \param device Device to read from or \c NULL.
 *  \param error Location to return an error.
 *  \retval bool \c TRUE if successful, else \c FALSE.
 *  \since 1.0
 */
gboolean gaminggear_profile_table_read(GaminggearProfileTable *table, GaminggearDevice *device, GError **error);

/*! \brief Saves and writes all profile datas.
 *  \param table Instance.
 *  \param device Device to write to or \c NULL.
 *  \param error Location to return an error.
 *  \retval bool \c TRUE if successful, else \c FALSE.
 *  \since 1.0
 */
gboolean gaminggear_profile_table_store(GaminggearProfileTable *table, GaminggearDevice *device, GError **error);

/*! \brief Get modified state.
 *
 *  Returns \c TRUE if any profile data has been modified.
 *
 *  \param table Instance.
 *  \retval bool \c TRUE if modified, else \c FALSE.
 *  \since 1.0
 */
gboolean gaminggear_profile_table_get_modified(GaminggearProfileTable *table);

/*! \brief Get name of default profile.
 *  \param table Instance.
 *  \retval string which shouldn't be freed or \c NULL if no default profile is selected.
 *  \since 1.0
 */
gchar const *gaminggear_profile_table_get_default_profile_name(GaminggearProfileTable *table);

/*! \brief Set default profile.
 *
 *  If no profile with the given name exists, no profile is selected as default profile.
 *  If more than one profile with the same name exists, one is randomly selected.
 *
 *  \param table Instance.
 *  \param name of the profile to select.
 *  \since 1.0
 */
void gaminggear_profile_table_set_default_profile_name(GaminggearProfileTable *table, gchar const *name);

G_END_DECLS

#endif
