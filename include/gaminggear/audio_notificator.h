#ifndef __GAMINGGEAR_AUDIO_NOTIFICATOR__
#define __GAMINGGEAR_AUDIO_NOTIFICATOR__

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

typedef struct _GaminggearAudioNotificator GaminggearAudioNotificator;
typedef struct _GaminggearAudioNotification GaminggearAudioNotification;

/*! \brief Initializes an audio notificator.
 *
 *  The nomenclature init/deinit means there is only one instance of this object
 *  supported by an application.
 *
 *  \param app_name The name of the binary.
 *  \param version_string Version of the binary.
 *  \param sound_driver "alsa" or "pulse".
 *  \retval notificator A new notificator object to be freed with \c gaminggear_audio_notificator_deinit()
 *          or \c NULL on error.
 *  \since 1.0
 */
GaminggearAudioNotificator *gaminggear_audio_notificator_init(gchar const *app_name, gchar const *version_string, gchar const *sound_driver);

/*! \brief Frees an existing notificator.
 *  \param notificator The notificator to be freed.
 *  \since 1.0
 */
void gaminggear_audio_notificator_deinit(GaminggearAudioNotificator *notificator);

/*! \brief Create new notification.
 *  \param notificator A notificator.
 *  \retval notification A new notification to be freed with \c gaminggear_audio_notification_free().
 *  \since 1.0
 */
GaminggearAudioNotification *gaminggear_audio_notification_new(GaminggearAudioNotificator *notificator);

/*! \brief Frees an existing notification.
 *  \param notification The notification to be freed.
 *  \since 1.0
 */
void gaminggear_audio_notification_free(GaminggearAudioNotification *notification);

/*! \brief Play a sound file as notification.
 *
 *  A already running playback is cancelled before the new file is played.
 *  \param notification The notification that plays the sound.
 *  \param filename The full path of the file to play.
 *  \param volume The playback volume in dB.
 *  \retval bool \c TRUE on success, else \c FALSE.
 *  \since 1.0
 */
gboolean gaminggear_audio_notification_update(GaminggearAudioNotification *notification, gchar const *filename, gdouble volume);

/*! \brief Cancel a running notification.
 *  \param notification Notification to be cancelled.
 *  \retval bool \c TRUE on success, else \c FALSE.
 *  \since 1.0
 */
gboolean gaminggear_audio_notification_cancel(GaminggearAudioNotification *notification);

G_END_DECLS

#endif
