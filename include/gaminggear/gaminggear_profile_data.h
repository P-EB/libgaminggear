#ifndef __GAMINGGEAR_PROFILE_DATA_H__
#define __GAMINGGEAR_PROFILE_DATA_H__

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

/*! \file gaminggear/gaminggear_profile_data.h
 *  \brief GaminggearProfileData
 */

#include "gaminggear/gaminggear_device.h"
#include <glib-object.h>

G_BEGIN_DECLS

#define GAMINGGEAR_PROFILE_DATA_TYPE (gaminggear_profile_data_get_type())
#define GAMINGGEAR_PROFILE_DATA(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GAMINGGEAR_PROFILE_DATA_TYPE, GaminggearProfileData))
#define IS_GAMINGGEAR_PROFILE_DATA(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GAMINGGEAR_PROFILE_DATA_TYPE))
#define GAMINGGEAR_PROFILE_DATA_CLASS(obj) (G_TYPE_CHECK_CLASS_CAST((obj), GAMINGGEAR_PROFILE_DATA_TYPE, GaminggearProfileDataClass))
#define IS_GAMINGGEAR_PROFILE_DATA_CLASS(obj) (G_TYPE_CHECK_CLASS_TYPE((obj), GAMINGGEAR_PROFILE_DATA_TYPE))
#define GAMINGGEAR_PROFILE_DATA_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), GAMINGGEAR_PROFILE_DATA_TYPE, GaminggearProfileDataClass))

typedef struct _GaminggearProfileData GaminggearProfileData;
typedef struct _GaminggearProfileDataClass GaminggearProfileDataClass;
typedef struct _GaminggearProfileDataPrivate GaminggearProfileDataPrivate;

enum {
	GAMINGGEAR_PROFILE_DATA_GAMEFILE_NUM = 5,
	GAMINGGEAR_PROFILE_DATA_GAMEFILE_PROFILE_INVALID = -1,
};

struct _GaminggearProfileData {
	GObject parent;
	GaminggearProfileDataPrivate *priv;
};

struct _GaminggearProfileDataClass {
	GObjectClass parent_class;

	gchar const *(*get_name)(GaminggearProfileData *self);
	void (*set_name)(GaminggearProfileData *self, gchar const *new_name);

	gchar const *(*get_gamefile)(GaminggearProfileData *self, guint index);
	void (*set_gamefile)(GaminggearProfileData *self, guint index, gchar const *new_gamefile);

	gint (*get_hardware_index)(GaminggearProfileData *self);
	void (*set_hardware_index)(GaminggearProfileData *self, gint new_index);

	gboolean (*save)(GaminggearProfileData *self, GError **error);
	gboolean (*load)(GaminggearProfileData *self, gchar const *path, GError **error);
	gboolean (*remove)(GaminggearProfileData *self, GError **error);

	gboolean (*write)(GaminggearProfileData *self, GaminggearDevice *device, GError **error);
	gboolean (*read)(GaminggearProfileData *self, GaminggearDevice *device, GError **error);

	gboolean (*get_modified)(GaminggearProfileData *self);
	void (*reset)(GaminggearProfileData *self);

	void (*copy)(GaminggearProfileData *self, GaminggearProfileData *other);
	gboolean (*equal)(GaminggearProfileData *self, GaminggearProfileData *other);

	void (*discriminate)(GaminggearProfileData *self);
};

GType gaminggear_profile_data_get_type(void);

/*! \brief Get profile name.
 *
 *  Pointer is only valid at the moment, make a copy if needed.
 *
 *  \param self Instance.
 *  \retval string that should not be freed.
 *  \since 1.0
 */
gchar const *gaminggear_profile_data_get_name(GaminggearProfileData *self);

/*! \brief Set profile name.
 *  \param self Instance.
 *  \param new_name New name.
 *  \since 1.0
 */
void gaminggear_profile_data_set_name(GaminggearProfileData *self, gchar const *new_name);

/*! \brief Get gamefile.
 *
 *  Pointer is only valid at the moment, make a copy if needed.
 *
 *  \param self Instance.
 *  \param index Index in range of [0, GAMINGGEAR_PROFILE_DATA_GAMEFILE_NUM[.
 *  \retval string
 *  \since 1.0
 */
gchar const *gaminggear_profile_data_get_gamefile(GaminggearProfileData *self, guint index);

/*! \brief Set gamefile.
 *  \param self Instance.
 *  \param index Index in range of [0, GAMINGGEAR_PROFILE_DATA_GAMEFILE_NUM[.
 *  \param new_gamefile
 *  \since 1.0
 */
void gaminggear_profile_data_set_gamefile(GaminggearProfileData *self, guint index, gchar const *new_gamefile);

/*! \brief Get profile index in hardware.
 *  \param self Instance.
 *  \retval index of profile in hardware or GAMINGGEAR_PROFILE_DATA_GAMEFILE_PROFILE_INVALID.
 *  \since 1.0
 */
gint gaminggear_profile_data_get_hardware_index(GaminggearProfileData *self);

/*! \brief Set profile index in hardware.
 *  \param self Instance.
 *  \param new_index Profile index in hardware or GAMINGGEAR_PROFILE_DATA_GAMEFILE_PROFILE_INVALID.
 *  \since 1.0
 */
void gaminggear_profile_data_set_hardware_index(GaminggearProfileData *self, gint new_index);

/*! \brief Save filesystem and hardware data.
 *
 *  Filesystem data is either saved to file it has been loaded from or a unique
 *  Filename is created. Hardware data is only stored if device and hardware index
 *  is valid.
 *
 *  \param self Instance.
 *  \param device Device to write to or \c NULL.
 *  \param error Location to return an error.
 *  \retval bool \c TRUE if successful, else \c FALSE.
 *  \since 1.0
 */
gboolean gaminggear_profile_data_store(GaminggearProfileData *self, GaminggearDevice *device, GError **error);

/*! \brief Load filesystem data.
 *  \param self Instance.
 *  \param path Filesystem path of file to load.
 *  \param error Location to return an error.
 *  \retval bool \c TRUE if successful, else \c FALSE.
 *  \since 1.0
 */
gboolean gaminggear_profile_data_load(GaminggearProfileData *self, gchar const *path, GError **error);

/*! \brief Remove filesystem data.
 *
 *  Instance should not be used after calling this function.
 *
 *  \param self Instance.
 *  \param error Location to return an error.
 *  \retval bool \c TRUE if successful, else \c FALSE.
 *  \since 1.0
 */
gboolean gaminggear_profile_data_remove(GaminggearProfileData *self, GError **error);

/*! \brief Read hardware data.
 *
 *  Data is only read if device and hardware index are valid.
 *
 *  \param self Instance.
 *  \param device Device to read from or \c NULL.
 *  \param error Location to return an error.
 *  \retval bool \c TRUE if successful, else \c FALSE.
 *  \since 1.0
 */
gboolean gaminggear_profile_data_read(GaminggearProfileData *self, GaminggearDevice *device, GError **error);

/*! \brief Get modified state
 *  \param self Instance.
 *  \retval bool \c TRUE if data should be saved/written, else \c FALSE.
 *  \since 1.0
 */
gboolean gaminggear_profile_data_get_modified(GaminggearProfileData *self);

/*! \brief Reset data
 *
 *  Resets the actual data with the last stored values.
 *
 *  \param self Instance.
 *  \since 1.0
 */
void gaminggear_profile_data_reset(GaminggearProfileData *self);

/*! \brief Get if instance is valid
 *  \param self Instance.
 *  \since 1.0
 */
gboolean gaminggear_profile_data_get_valid(GaminggearProfileData *self);

/*! \brief Duplicates profile data
 *
 *  Creates unmodified deep copy of other.
 *
 *  \param other The object to duplicate.
 *  \retval profile_data New object.
 *  \since 1.0
 */
GaminggearProfileData *gaminggear_profile_data_dup(GaminggearProfileData *other);

/*! \brief Checks equality of two profile datas
 *
 *  Only checks the data, not where it would be saved.
 *
 *  \param self One object.
 *  \param other Other object to compare against self.
 *  \retval bool \c TRUE if equal, else \c FALSE.
 *  \since 1.0
 */
gboolean gaminggear_profile_data_equal(GaminggearProfileData *self, GaminggearProfileData *other);

/*! \brief Makes profile data unique
 *
 *  Changes profile data so that a new file is created.
 *
 *  \param self Instance.
 *  \since 1.0
 */
void gaminggear_profile_data_discriminate(GaminggearProfileData *self);

G_END_DECLS

#endif
