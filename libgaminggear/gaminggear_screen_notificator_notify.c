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

#include "gaminggear/screen_notificator.h"
#include "config.h"
#include "i18n-lib.h"
#include <libnotify/notify.h>

struct _GaminggearScreenNotificator {
	gchar *icon;
};

struct _GaminggearScreenNotification {
	NotifyNotification *notification;
	GaminggearScreenNotificator *notificator;
};

GaminggearScreenNotificator *gaminggear_screen_notificator_init(gchar const *app_name, gchar const *icon) {
	GaminggearScreenNotificator *notificator;

	if (!notify_init(app_name)) {
		g_warning(_("Could not initialize notify"));
		return NULL;
	}

	notificator = (GaminggearScreenNotificator *)g_malloc0(sizeof(GaminggearScreenNotificator));
	notificator->icon = g_strdup(icon);
	return notificator;
}

void gaminggear_screen_notificator_deinit(GaminggearScreenNotificator *notificator) {
	g_free(notificator->icon);
	notify_uninit();
}

GaminggearScreenNotification *gaminggear_screen_notification_new(GaminggearScreenNotificator *notificator) {
	GaminggearScreenNotification *notification;
	notification = (GaminggearScreenNotification *)g_malloc0(sizeof(GaminggearScreenNotification));
#ifdef HAVE_NOTIFY_0_7
	notification->notification = notify_notification_new("dummy", NULL, NULL);
#else
	notification->notification = notify_notification_new("dummy", NULL, NULL, NULL);
#endif
	notify_notification_set_timeout(notification->notification, NOTIFY_EXPIRES_DEFAULT);
	notification->notificator = notificator;
	return notification;
}

void gaminggear_screen_notification_free(GaminggearScreenNotification *notification) {
	if (notification->notification)
		g_object_unref(G_OBJECT(notification->notification));
	g_free(notification);
}

gboolean gaminggear_screen_notification_cancel(GaminggearScreenNotification *notification) {
	GError *local_error = NULL;
	gboolean result;

	if (!notification->notification)
		return FALSE;

	result = notify_notification_close(notification->notification, &local_error);
	g_clear_error(&local_error);
	return result;
}

gboolean gaminggear_screen_notification_update_v(GaminggearScreenNotification *notification, gchar const *format, va_list args) {
	gchar *summary;
	gboolean result;
	GError *local_error = NULL;

	if (!notification->notification)
		return TRUE;

	summary = g_strdup_vprintf(format, args);

	result = notify_notification_update(notification->notification, summary, NULL, notification->notificator->icon);
	if (!result)
		goto free_summary;
	result = notify_notification_show(notification->notification, &local_error);
	g_clear_error(&local_error);

free_summary:
	g_free(summary);
	return result;
}

gboolean gaminggear_screen_notification_update(GaminggearScreenNotification *notification, gchar const *format, ...) {
	va_list args;
	gboolean retval;

	if (!notification->notification)
		return TRUE;

	va_start(args, format);
	retval = gaminggear_screen_notification_update_v(notification, format, args);
	va_end(args);

	return retval;
}
