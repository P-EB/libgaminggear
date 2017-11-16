#ifndef __GAMINGGEAR_PROFILE_LIST_H__
#define __GAMINGGEAR_PROFILE_LIST_H__

/*
 * This file is part of gaminggear-tools.
 *
 * gaminggear-tools is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * gaminggear-tools is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with gaminggear-tools. If not, see <http://www.gnu.org/licenses/>.
 */

/*! \file gaminggear/profiles.h
 *  \brief Profiles
 */

#include "gaminggear/gaminggear_profile_data.h"

G_BEGIN_DECLS

typedef struct _GaminggearProfiles GaminggearProfiles;

/*! \brief Find profile.
 *  \param profiles Instance.
 *  \param hardware_index Index of profile to find.
 *  \retval profile_data or \c NULL if not found.
 *  \since 1.0
 */
GaminggearProfileData *gaminggear_profiles_find_by_hardware_index(GaminggearProfiles *profiles, gint hardware_index);

/*! \brief Find profile.
 *  \param profiles Instance.
 *  \param name Name of profile to find.
 *  \retval profile_data or \c NULL if not found.
 *  \since 1.0
 */
GaminggearProfileData *gaminggear_profiles_find_by_name(GaminggearProfiles *profiles, gchar const *name);

/*! \brief Switch profile.
 *
 *  gaminggear_profiles_store needs to be called to apply the new settings.
 *
 *  \param profiles Instance.
 *  \param target_profile profile to activate.
 *  \param hardware_index Index to write to.
 *  \since 1.0
 */
void gaminggear_profiles_activate(GaminggearProfiles *profiles, GaminggearProfileData *target_profile, gint hardware_index);

/*! \brief Switch profile.
 *
 *  gaminggear_profiles_store needs to be called to apply the new settings.
 *
 *  \param profiles Instance.
 *  \param name Name of profile to activate.
 *  \param hardware_index Index to write to.
 *  \retval bool \c TRUE if successful, else \c FALSE.
 *  \since 1.0
 */
gboolean gaminggear_profiles_activate_per_name(GaminggearProfiles *profiles, gchar const *name, gint hardware_index);

/*! \brief Save and write profiles.
 *  \param profiles Instance.
 *  \param device Device to write to.
 *  \param error Location to return an error.
 *  \retval bool \c TRUE if successful, else \c FALSE.
 *  \since 1.0
 */
gboolean gaminggear_profiles_store(GaminggearProfiles *profiles, GaminggearDevice *device, GError **error);

/*! \brief Fill profile list.
 *
 *  ?
 *
 *  \param profiles Instance.
 *  \param device Device to read from.
 *  \param count Profile count to fill.
 *  \param error Location to return an error.
 *  \retval bool \c TRUE if successful, else \c FALSE.
 *  \since 1.0
 */
gboolean gaminggear_profiles_fill(GaminggearProfiles *profiles, GaminggearDevice *device, guint count, GError **error);

/*! \brief Read hardware data.
 *  \param profiles Instance.
 *  \param device Device to read from.
 *  \param error Location to return an error.
 *  \retval bool \c TRUE if successful, else \c FALSE.
 *  \since 1.0
 */
gboolean gaminggear_profiles_read(GaminggearProfiles *profiles, GaminggearDevice *device, GError **error);

/*! \brief Load filesystem data.
 *  \param profiles Instance.
 *  \param error Location to return an error.
 *  \retval bool \c TRUE if successful, else \c FALSE.
 *  \since 1.0
 */
gboolean gaminggear_profiles_load(GaminggearProfiles *profiles, GError **error);

/*! \brief Create new profiles structure.
 *  \param type Type of profile_data to handle.
 *  \param path Path to load files from.
 *  \retval profiles that should be freed with gaminggear_profiles_free.
 *  \since 1.0
 */
GaminggearProfiles *gaminggear_profiles_new(GType type, gchar const *path);

/*! \brief Free profiles structure.
 *  \param profiles Instance that should be freed.
 *  \since 1.0
 */
void gaminggear_profiles_free(GaminggearProfiles *profiles);

/*! \brief Get first profile in list.
 *  \param profiles Instance
 *  \retval iterator First element of single linked list.
 *  \since 1.0
 */
GSList *gaminggear_profiles_first(GaminggearProfiles *profiles);

G_END_DECLS

#endif
