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

#include "gaminggear/audio_notificator.h"
#include "i18n-lib.h"
#include <canberra.h>
#include <unistd.h>

struct _GaminggearAudioNotificator {
	ca_context *context;
	guint next_id;
};

struct _GaminggearAudioNotification {
	GaminggearAudioNotificator *notificator;
	guint id;
};

static guint gaminggear_audio_notificator_claim_next_id(GaminggearAudioNotificator *notificator) {
	if (notificator == NULL)
		return 0;

	return notificator->next_id++;
}

GaminggearAudioNotificator *gaminggear_audio_notificator_init(gchar const *app_name, gchar const *version_string, gchar const *sound_driver) {
	GaminggearAudioNotificator *notificator;
	gint retval;
	gchar *pid_string;

	notificator = (GaminggearAudioNotificator *)g_malloc0(sizeof(GaminggearAudioNotificator));

	/* Creating own context to be independent of sound scheme. */
	retval = ca_context_create(&notificator->context);
	if (retval) {
		g_warning(_("Could not create canberra context: %s"), ca_strerror(retval));
		goto out;
	}

	/* Setting driver explicit to be independent of warning sound volume */
	retval = ca_context_set_driver(notificator->context, sound_driver);
	if (retval) {
		g_warning(_("Could not set driver %s for canberra context: %s"), sound_driver, ca_strerror(retval));
		goto destroy_context;
	}

	pid_string = g_strdup_printf("%lu", (gulong)getpid());
	retval = ca_context_change_props(notificator->context,
			CA_PROP_APPLICATION_NAME, app_name,
			CA_PROP_APPLICATION_VERSION, version_string,
			CA_PROP_APPLICATION_PROCESS_ID, pid_string,
			NULL);
	g_free(pid_string);
	if (retval) {
		g_warning(_("Could not set properties for canberra context: %s"), ca_strerror(retval));
		goto destroy_context;
	}

	retval = ca_context_open(notificator->context);
	if (retval) {
		g_warning(_("Could not open canberra context: %s"), ca_strerror(retval));
		goto destroy_context;
	}

	notificator->next_id = 1;

	return notificator;

destroy_context:
	ca_context_destroy(notificator->context);
out:
	g_free(notificator);
	return NULL;
}

void gaminggear_audio_notificator_deinit(GaminggearAudioNotificator *notificator) {
	gint retval;

	if (notificator == NULL)
		return;

	retval = ca_context_destroy(notificator->context);
	if (retval)
		g_warning(_("Could not destroy canberra context: %s"), ca_strerror(retval));
	g_free(notificator);
}

GaminggearAudioNotification *gaminggear_audio_notification_new(GaminggearAudioNotificator *notificator) {
	GaminggearAudioNotification *notification;

	if (notificator == NULL)
		return NULL;

	notification = (GaminggearAudioNotification *)g_malloc0(sizeof(GaminggearAudioNotification));
	notification->notificator = notificator;
	notification->id = gaminggear_audio_notificator_claim_next_id(notificator);
	return notification;
}

void gaminggear_audio_notification_free(GaminggearAudioNotification *notification) {
	g_free(notification);
}

gboolean gaminggear_audio_notification_cancel(GaminggearAudioNotification *notification) {
	gint retval;

	if (notification == NULL)
		return FALSE;

	retval = ca_context_cancel(notification->notificator->context, notification->id);
	if (retval != 0 && retval != CA_ERROR_STATE) {
		g_warning(_("Could not cancel audio notificator: %s"), ca_strerror(retval));
		return FALSE;
	}
	return TRUE;
}

gboolean gaminggear_audio_notification_update(GaminggearAudioNotification *notification, gchar const *filename, gdouble volume) {
	gint retval;
	gchar *volume_string;

	if (notification == NULL)
		return FALSE;

	volume_string = g_strdup_printf("%f", volume);

	gaminggear_audio_notification_cancel(notification);
	retval = ca_context_play(notification->notificator->context, notification->id,
			CA_PROP_CANBERRA_VOLUME, volume_string,
			CA_PROP_MEDIA_FILENAME, filename,
			NULL);
	g_free(volume_string);
	if (retval) {
		g_warning(_("Could not update audio notificator: %s"), ca_strerror(retval));
		return FALSE;
	}
	return TRUE;
}
