#ifndef __GAMINGGEAR_PROFILE_PAGE_H__
#define __GAMINGGEAR_PROFILE_PAGE_H__

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

/*! \file gaminggear/gaminggear_profile_page.h
 *  \brief GaminggearProfilePage
 */
#include "gaminggear/gaminggear_profile_data.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GAMINGGEAR_PROFILE_PAGE_TYPE (gaminggear_profile_page_get_type())
#define GAMINGGEAR_PROFILE_PAGE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GAMINGGEAR_PROFILE_PAGE_TYPE, GaminggearProfilePage))
#define IS_GAMINGGEAR_PROFILE_PAGE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GAMINGGEAR_PROFILE_PAGE_TYPE))

#define GAMINGGEAR_PROFILE_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GAMINGGEAR_PROFILE_PAGE_TYPE, GaminggearProfilePageClass))
#define IS_GAMINGGEAR_PROFILE_PAGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GAMINGGEAR_PROFILE_PAGE_TYPE))
#define GAMINGGEAR_PROFILE_PAGE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), GAMINGGEAR_PROFILE_PAGE_TYPE, GaminggearProfilePageClass))

typedef struct _GaminggearProfilePageClass GaminggearProfilePageClass;
typedef struct _GaminggearProfilePage GaminggearProfilePage;
typedef struct _GaminggearProfilePagePrivate GaminggearProfilePagePrivate;

struct _GaminggearProfilePage {
	GtkVBox parent;
	GaminggearProfilePagePrivate *priv;
};

struct _GaminggearProfilePageClass {
	GtkVBoxClass parent_class;
	void (*set_from_data)(GaminggearProfilePage *self, GaminggearProfileData *profile_data);
	void (*update_data)(GaminggearProfilePage *self, GaminggearProfileData *profile_data);
};

GType gaminggear_profile_page_get_type(void);

/*! \brief Set profile data.
 *
 *  GaminggearProfilePage keeps a reference to profile_data. If profile_data is
 *  \c NULL, all widgets are made unsensitive.
 *
 *  If there already was valid profile_data, that instance gets updated on user
 *  request before it's being replaced with new profile_data.
 *
 *  \param self Instance.
 *  \param profile_data or \c NULL.
 *  \since 1.0
 */
void gaminggear_profile_page_set_profile_data(GaminggearProfilePage *self, GaminggearProfileData *profile_data);

/*! \brief Get profile data.
 *  \param self Instance.
 *  \retval profile_data which should be freed with g_object_unref() or \c NULL.
 *  \since 1.0
 */
GaminggearProfileData *gaminggear_profile_page_get_profile_data(GaminggearProfilePage *self);

/*! \brief Update profile_data.
 *
 *  Updates the profile_data with information from GUI on users request.
 *
 *  \param self Instance.
 *  \since 1.0
 */
void gaminggear_profile_page_update_data(GaminggearProfilePage *self);

G_END_DECLS

#endif
