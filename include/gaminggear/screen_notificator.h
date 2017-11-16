#ifndef __GAMINGGEAR_SCREEN_NOTIFICATOR__
#define __GAMINGGEAR_SCREEN_NOTIFICATOR__

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

typedef struct _GaminggearScreenNotificator GaminggearScreenNotificator;
typedef struct _GaminggearScreenNotification GaminggearScreenNotification;

/*! \brief Initializes an screen notificator.
 *
 *  The nomenclature init/deinit means there is only one instance of this object
 *  supported by an application.
 *
 *  \param app_name The name of the binary.
 *  \param icon The name of the icon to be shown in notification.
 *  \retval notificator A new notificator object to be freed with \c gaminggear_screen_notificator_deinit()
 *          or \c NULL on error.
 *  \since 1.0
 */
GaminggearScreenNotificator *gaminggear_screen_notificator_init(gchar const *app_name, gchar const *icon);

/*! \brief Frees an existing notificator.
 *  \param notificator The notificator to be freed.
 *  \since 1.0
 */
void gaminggear_screen_notificator_deinit(GaminggearScreenNotificator *notificator);

/*! \brief Create new notification.
 *  \param notificator A notificator.
 *  \retval notification A new notification to be freed with \c gaminggear_screen_notification_free().
 *  \since 1.0
 */
GaminggearScreenNotification *gaminggear_screen_notification_new(GaminggearScreenNotificator *notificator);

/*! \brief Frees an existing notification.
 *  \param notification The notification to be freed.
 *  \since 1.0
 */
void gaminggear_screen_notification_free(GaminggearScreenNotification *notification);

/*! \brief Show notification.
 *
 *  If this notification is already visible its content is updated.
 *
 *  \param notification The notification to be updated.
 *  \param format A \c printf() compatible format string.
 *  \param ... The arguments to be inserted in the output.
 *  \retval bool \c TRUE on success, else \c FALSE.
 *  \since 1.0
 */
gboolean gaminggear_screen_notification_update(GaminggearScreenNotification *notification, gchar const *format, ...);

/*! \brief Show notification.
 *
 *  If this notification is already visible its content is updated.
 *
 *  \param notification The notification to be updated.
 *  \param format A \c printf() compatible format string.
 *  \param args The arguments to be inserted in the output.
 *  \retval bool \c TRUE on success, else \c FALSE.
 *  \since 1.0
 */
gboolean gaminggear_screen_notification_update_v(GaminggearScreenNotification *notification, gchar const *format, va_list args);

/*! \brief Cancel a running notification.
 *  \param notification Notification to be cancelled.
 *  \retval bool \c TRUE on success, else \c FALSE.
 *  \since 1.0
 */
gboolean gaminggear_screen_notification_cancel(GaminggearScreenNotification *notification);

G_END_DECLS

#endif
