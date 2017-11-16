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

#include "gaminggear/input_events.h"
#include "gaminggear/key_translations.h"
#include "config.h"
#include "i18n-lib.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/uinput.h>

static int uinput = -1;
static gchar const * const identifier = "GaminggearUinput";
static guint8 active_hids[256];

gboolean gaminggear_input_event_init(guint vendor_id, guint product_id, GError **error) {
	guint i;
	struct uinput_user_dev uidev;

	if (uinput >= 0)
		return TRUE;

	uinput = open(UINPUT_DIR "/uinput", O_WRONLY | O_NONBLOCK);
	if (uinput < 0) {
		g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errno), _("Could not open uinput: %s"), g_strerror(errno));
		goto error;
	}
	g_debug(_("Uinput %1$s has file descriptor %2$i"), identifier, uinput);

	for (i = KEY_ESC; i <= KEY_DELETE; ++i) {
		if (ioctl(uinput, UI_SET_KEYBIT, i) < 0)
			g_warning(_("Could not set uinput key bit %u: %s"), i, g_strerror(errno));
	}

	if (ioctl(uinput, UI_SET_EVBIT, EV_KEY) < 0)
		g_warning(_("Could not set uinput event bit %u: %s"), EV_KEY, g_strerror(errno));
	if (ioctl(uinput, UI_SET_KEYBIT, KEY_MUTE) < 0)
		g_warning(_("Could not set uinput key bit %u: %s"), KEY_MUTE, g_strerror(errno));
	if (ioctl(uinput, UI_SET_KEYBIT, KEY_VOLUMEDOWN) < 0)
		g_warning(_("Could not set uinput key bit %u: %s"), KEY_VOLUMEDOWN, g_strerror(errno));
	if (ioctl(uinput, UI_SET_KEYBIT, KEY_VOLUMEUP) < 0)
		g_warning(_("Could not set uinput key bit %u: %s"), KEY_VOLUMEUP, g_strerror(errno));
	if (ioctl(uinput, UI_SET_KEYBIT, KEY_PREVIOUSSONG) < 0)
		g_warning(_("Could not set uinput key bit %u: %s"), KEY_PREVIOUSSONG, g_strerror(errno));
	if (ioctl(uinput, UI_SET_KEYBIT, KEY_NEXTSONG) < 0)
		g_warning(_("Could not set uinput key bit %u: %s"), KEY_NEXTSONG, g_strerror(errno));
	if (ioctl(uinput, UI_SET_KEYBIT, KEY_PLAYPAUSE) < 0)
		g_warning(_("Could not set uinput key bit %u: %s"), KEY_PLAYPAUSE, g_strerror(errno));
	if (ioctl(uinput, UI_SET_KEYBIT, KEY_STOPCD) < 0)
		g_warning(_("Could not set uinput key bit %u: %s"), KEY_STOPCD, g_strerror(errno));

	for (i = BTN_LEFT; i <= BTN_BACK; ++i) {
		if (ioctl(uinput, UI_SET_KEYBIT, i) < 0)
			g_warning(_("Could not set uinput key bit %u: %s"), i, g_strerror(errno));
	}

	memset(&uidev, 0, sizeof(uidev));
	memset(&active_hids, 0, sizeof(active_hids));

	snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "%s", identifier);
	uidev.id.bustype = BUS_USB;
	uidev.id.vendor = vendor_id;
	uidev.id.product = product_id;
	uidev.id.version = 1;

	if (write(uinput, &uidev, sizeof(uidev)) < 0) {
		g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errno), _("Could not initialize uinput %i: %s"), uinput, g_strerror(errno));
		goto close;
	}

	if (ioctl(uinput, UI_DEV_CREATE) < 0) {
		g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errno), _("Could not create uinput %i: %s"), uinput, g_strerror(errno));
		goto close;
	}

	return TRUE;
close:
	close(uinput);
error:
	uinput = -1;
	return FALSE;
}

gboolean gaminggear_input_event_deinit(GError **error) {
	gboolean retval = TRUE;

	if (uinput < 0)
		return TRUE;

	if (ioctl(uinput, UI_DEV_DESTROY) < 0) {
		g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errno), _("Could not destroy uinput %i: %s"), uinput, g_strerror(errno));
		retval = FALSE;
	}

	close(uinput);
	uinput = -1;
	return retval;
}

static void gaminggear_input_event_write_keycode(int code, int value) {
	struct input_event event;
	int written;

	if (uinput < 0) {
		g_warning(_("Uinput is not initialized"));
		return;
	}

	event.type = EV_KEY;
	event.code = code;
	event.value = value;

	written = write(uinput, (const void*) &event, sizeof(event));
	if (written != sizeof(event))
		g_warning(_("Could not write key event %1$i/%2$i to uinput %3$i: %4$i"), code, value, uinput, written);
}

static void gaminggear_input_event_write_sync(void) {
	struct input_event event;
	int written;

	if (uinput < 0) {
		g_warning(_("Uinput is not initialized"));
		return;
	}

	event.type = EV_SYN;
	event.code = SYN_REPORT;
	event.value = 0;

	written = write(uinput, (const void*) &event, sizeof(event));
	if (written != sizeof(event))
		g_warning(_("Could not write sync event to uinput %i: %i"), uinput, written);
}

/* returns true if state changed */
static gboolean test_hid(int hid, int value) {
	gboolean retval = FALSE;

	if (value == GAMINGGEAR_INPUT_EVENT_VALUE_PRESS) {
		if (active_hids[hid] == G_MAXUINT8) {
			g_warning(_("Uinput: Hid code %i was pressed too many times"), hid);
			return FALSE;
		}

		if (active_hids[hid] == 0)
			retval = TRUE;

		++active_hids[hid];
	} else {
		if (active_hids[hid] == 0) {
			g_warning(_("Uinput: Hid code %i was not pressed before"), hid);
			return FALSE;
		}

		--active_hids[hid];

		if (active_hids[hid] == 0)
			retval = TRUE;
	}

	return retval;
}

void gaminggear_input_event_write_keyboard(int hid, int value) {
	if (test_hid(hid, value)) {
		gaminggear_input_event_write_keycode(gaminggear_hid_to_kbd_keycode(hid), value);
		gaminggear_input_event_write_sync();
	}
}

void gaminggear_input_event_write_keyboard_multi(int *hids, gsize length, int value) {
	gsize i;
	gboolean any = FALSE;

	for (i = 0; i < length; ++i) {
		if (test_hid(hids[i], value)) {
			gaminggear_input_event_write_keycode(gaminggear_hid_to_kbd_keycode(hids[i]), value);
			any = TRUE;
		}
	}

	if (any)
		gaminggear_input_event_write_sync();
}

void gaminggear_input_event_write_button(int hid, int value) {
	if (test_hid(hid, value)) {
		gaminggear_input_event_write_keycode(gaminggear_hid_to_btn_keycode(hid), value);
		gaminggear_input_event_write_sync();
	}
}

void gaminggear_input_event_write_multimedia(int hid, int value) {
	gaminggear_input_event_write_keycode(gaminggear_consumer_page_to_kbd_keycode(hid), value);
	gaminggear_input_event_write_sync();
}

void gaminggear_input_event_write_multimedia_single(int hid) {
	gaminggear_input_event_write_multimedia(hid, GAMINGGEAR_INPUT_EVENT_VALUE_PRESS);
	gaminggear_input_event_write_sync();
	gaminggear_input_event_write_multimedia(hid, GAMINGGEAR_INPUT_EVENT_VALUE_RELEASE);
	gaminggear_input_event_write_sync();
}
