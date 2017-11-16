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
#include "gaminggear/hid_uid.h"
#include "gaminggear/macro.h"
#include "config.h"
#include "i18n-lib.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <linux/version.h>
#include <linux/uhid.h>

enum {
	KEYBOARD_KEY_NUM = 30,
	MOUSE_BUTTON_NUM = 5,
	MOUSE_REPORT_ID = 1,
	MULTIMEDIA_REPORT_ID = 2,
};

/*
 * QUOTE decoding of report descriptors was done with
 * http://eleccelerator.com/usbdescreqparser/
 * Copyright (c) 2013 Frank Zhao
 */

/* without led output report defined, shift does not work */
static guint8 const keyboard_descriptor[] = { /* Taken from Ryos MK Pro */
	0x05, 0x01,                    // Usage Page (Generic Desktop Ctrls)
	0x09, 0x06,                    // Usage (Keyboard)
	0xA1, 0x01,                    // Collection (Application)
	0x05, 0x08,                    //   Usage Page (LEDs)
	0x19, 0x01,                    //   Usage Minimum (Num Lock)
	0x29, 0x03,                    //   Usage Maximum (Scroll Lock)
	0x15, 0x00,                    //   Logical Minimum (0)
	0x25, 0x01,                    //   Logical Maximum (1)
	0x75, 0x01,                    //   Report Size (1)
	0x95, 0x03,                    //   Report Count (3)
	0x91, 0x02,                    //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
	0x95, 0x05,                    //   Report Count (5)
	0x75, 0x01,                    //   Report Size (1)
	0x91, 0x01,                    //   Output (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
	0x05, 0x07,                    //   Usage Page (Kbrd/Keypad)
	0x19, HID_UID_KB_LEFT_CONTROL, //   Usage Minimum (0xE0)
	0x29, HID_UID_KB_RIGHT_GUI,    //   Usage Maximum (0xE7)
	0x95, 0x08,                    //   Report Count (8)
	0x81, 0x02,                    //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x75, 0x08,                    //   Report Size (8)
	0x95, 0x01,                    //   Report Count (1)
	0x81, 0x01,                    //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x19, 0x00,                    //   Usage Minimum (0x00)
	0x29, 0x91,                    //   Usage Maximum (0x91)
	0x15, 0x00,                    //   Logical Minimum (0)
	0x26, 0xFF, 0x00,              //   Logical Maximum (255)
	0x75, 0x08,                    //   Report Size (8)
	0x95, KEYBOARD_KEY_NUM,        //   Report Count (30)
	0x81, 0x00,                    //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0xC0,                          // End Collection
};

static guint8 const mouse_descriptor[] = { /* Taken from Tyon */
	0x05, 0x01,                    // Usage Page (Generic Desktop Ctrls)
	0x09, 0x02,                    // Usage (Mouse)
	0xA1, 0x01,                    // Collection (Application)
	0x85, MOUSE_REPORT_ID,         //   Report ID (1)
	0x09, 0x01,                    //   Usage (Pointer)
	0xA1, 0x00,                    //   Collection (Physical)
	0x05, 0x09,                    //     Usage Page (Button)
	0x19, 0x01,                    //     Usage Minimum (0x01)
	0x29, MOUSE_BUTTON_NUM,        //     Usage Maximum (0x05)
	0x15, 0x00,                    //     Logical Minimum (0)
	0x25, 0x01,                    //     Logical Maximum (1)
	0x95, MOUSE_BUTTON_NUM,        //     Report Count (5)
	0x75, 0x01,                    //     Report Size (1)
	0x81, 0x02,                    //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x95, 0x01,                    //     Report Count (1)
	0x75, 8 - MOUSE_BUTTON_NUM,    //     Report Size (3)
	0x81, 0x01,                    //     Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x05, 0x01,                    //     Usage Page (Generic Desktop Ctrls)
	0x09, 0x30,                    //     Usage (X)
	0x09, 0x31,                    //     Usage (Y)
	0x16, 0x00, 0x80,              //     Logical Minimum (32768)
	0x26, 0xFF, 0x7F,              //     Logical Maximum (32767)
	0x95, 0x02,                    //     Report Count (2)
	0x75, 0x10,                    //     Report Size (16)
	0x81, 0x06,                    //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
	0x09, 0x38,                    //     Usage (Wheel)
	0x15, 0x81,                    //     Logical Minimum (129)
	0x25, 0x7F,                    //     Logical Maximum (127)
	0x95, 0x01,                    //     Report Count (1)
	0x75, 0x08,                    //     Report Size (8)
	0x81, 0x06,                    //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
	0x05, 0x0C,                    //     Usage Page (Consumer)
	0x0A, 0x38, 0x02,              //     Usage (AC Pan)
	0x81, 0x06,                    //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
	0xC0,                          //   End Collection
	0xC0,                          // End Collection
};

static guint8 const multimedia_descriptor[] =  { /* Taken from Ryos MK */
	0x05, 0x0C,                    // Usage Page (Consumer)
	0x09, 0x01,                    // Usage (Consumer Control)
	0xA1, 0x01,                    // Collection (Application)
	0x85, MULTIMEDIA_REPORT_ID,    //   Report ID (2)
	0x19, 0x00,                    //   Usage Minimum (Unassigned)
	0x2A, 0x3C, 0x02,              //   Usage Maximum (AC Format)
	0x15, 0x00,                    //   Logical Minimum (0)
	0x26, 0x3C, 0x02,              //   Logical Maximum (572)
	0x95, 0x01,                    //   Report Count (1)
	0x75, 0x10,                    //   Report Size (16)
	0x81, 0x00,                    //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0xC0,                          // End Collection
};

static struct {
	guint8 special;
	guint8 pad;
	guint8 keys[KEYBOARD_KEY_NUM];
} __attribute__ ((packed)) keyboard_event;
static guint keyboard_event_next_key_index;

static struct {
	guint8 report_id;
	guint8 buttons;
	guint8 unused[6];
} __attribute__ ((packed)) mouse_event;

static struct {
	guint8 report_id;
	guint16 value;
} __attribute__ ((packed)) multimedia_event;

/* It's intentional to have one device per report descriptor */
typedef struct {
	int fd;
	gchar const * const identifier;
	guint8 const * const report_descriptor;
	size_t const report_descriptor_size;
	guint8 const * const event;
	size_t const event_size;
} uhid_device;

static uhid_device keyboard  = {
	-1,
	"GaminggearUhidKeyboard",
	keyboard_descriptor,
	sizeof(keyboard_descriptor),
	(guint8 const *)&keyboard_event,
	sizeof(keyboard_event),
};

static uhid_device mouse = {
	-1,
	"GaminggearUhidMouse",
	mouse_descriptor,
	sizeof(mouse_descriptor),
	(guint8 const *)&mouse_event,
	sizeof(mouse_event),
};

static uhid_device multimedia = {
	-1,
	"GaminggearUhidMultimedia",
	multimedia_descriptor,
	sizeof(multimedia_descriptor),
	(guint8 const *)&multimedia_event,
	sizeof(multimedia_event),
};

static guint8 active_keyboard_hids[256];
static guint8 active_mouse_buttons[MOUSE_BUTTON_NUM];

static gboolean uhid_write(uhid_device const *device, struct uhid_event const *event, GError **error) {
	ssize_t ret;

	ret = write(device->fd, event, sizeof(*event));
	if (ret < 0) {
		g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errno), _("Could not write to uhid %i: %s"), device->fd, g_strerror(errno));
		return FALSE;
	} else if (ret != sizeof(*event)) {
		g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_FAILED, _("Could not write to uhid %i: partial write (%zi instead of %zu)"), device->fd, ret, sizeof(*event));
		return FALSE;
	}
	
	return TRUE;
}

static gchar *data_to_string(guchar const *data, gsize length) {
	gchar *buffer;
	gsize i;
	guint n;

	if (length == 0)
		return g_strdup("");

	buffer = (gchar *)g_malloc(length * 3);

	n = sprintf(buffer, "%02x", data[0]);
	for (i = 1; i < length; ++i)
		n += sprintf(buffer + n, " %02x", data[i]);

	return buffer;
}

static void debug(uhid_device const *device) {
	gchar *data_string;
	struct timeval time;

	gettimeofday(&time, NULL);
	data_string = data_to_string(device->event, device->event_size);

	g_debug("%llu.%06llu IN  %s  %s",
			(unsigned long long)time.tv_sec, (unsigned long long)time.tv_usec,
			device->identifier, data_string);

	g_free(data_string);
}

static gboolean send_event(uhid_device const *device) {
	struct uhid_event event;
	GError *local_error = NULL;
	gboolean retval;

	debug(device);
	
	memset(&event, 0, sizeof(event));

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,15,0)
	event.type = UHID_INPUT2;
	event.u.input2.size = device->event_size;
	memcpy(event.u.input2.data, device->event, MIN(UHID_DATA_MAX, device->event_size));
#else
	event.type = UHID_INPUT;
	event.u.input.size = device->event_size;
	memcpy(event.u.input.data, device->event, MIN(UHID_DATA_MAX, device->event_size));
#endif

	retval = uhid_write(device, &event, &local_error);
	if (!retval) {
		g_warning(_("Could not write event to uhid %i: %s"), device->fd, local_error->message);
		g_clear_error(&local_error);
	}
	return retval;
}

static gboolean init(uhid_device *device, guint vendor_id, guint product_id, GError **error) {
	struct uhid_event event;
	
	device->fd = open(UHID_DIR "/uhid", O_RDWR | O_CLOEXEC);
	if (device->fd < 0) {
		g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errno), _("Could not open uhid: %s"), g_strerror(errno));
		return FALSE;
	}
	g_debug(_("Uhid %1$s has file descriptor %2$i"), device->identifier, device->fd);
	
	memset(&event, 0, sizeof(event));
	memset(&active_keyboard_hids, 0, sizeof(active_keyboard_hids));
	memset(&active_mouse_buttons, 0, sizeof(active_mouse_buttons));

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,15,0)
	event.type = UHID_CREATE2;
	snprintf((gchar *)event.u.create2.name, 128, "%s", device->identifier);
	memcpy(event.u.create2.rd_data, device->report_descriptor, MIN(HID_MAX_DESCRIPTOR_SIZE, device->report_descriptor_size));
	event.u.create2.rd_size = device->report_descriptor_size;
	event.u.create2.bus = BUS_USB;
	event.u.create2.vendor = vendor_id;
	event.u.create2.product = product_id;
	event.u.create2.version = 1;
	event.u.create2.country = 0;
#else
	event.type = UHID_CREATE;
	snprintf((gchar *)event.u.create.name, 128, "%s", device->identifier);
	event.u.create.rd_data = device->report_descriptor;
	event.u.create.rd_size = device->report_descriptor_size;
	event.u.create.bus = BUS_USB;
	event.u.create.vendor = vendor_id;
	event.u.create.product = product_id;
	event.u.create.version = 1;
	event.u.create.country = 0;
#endif

	if (!uhid_write(device, &event, error)) {
		close(device->fd);
		device->fd = -1;
		return FALSE;
	}
	
	return TRUE;
}

static gboolean deinit(uhid_device *device, GError **error) {
	struct uhid_event event;
	gboolean retval;
	
	if (device->fd < 0)
		return TRUE;
	
	memset(&event, 0, sizeof(event));
	event.type = UHID_DESTROY;
	retval = uhid_write(device, &event, error);
	
	close(device->fd);
	device->fd = -1;
	return retval;
}

gboolean gaminggear_input_event_init(guint vendor_id, guint product_id, GError **error) {
	memset(&keyboard_event, 0, sizeof(keyboard_event));
	if (!init(&keyboard, vendor_id, product_id, error))
		return FALSE;
	keyboard_event_next_key_index = 0;

	memset(&mouse_event, 0, sizeof(mouse_event));
	mouse_event.report_id = MOUSE_REPORT_ID;
	if (!init(&mouse, vendor_id, product_id, error))
		return FALSE;

	memset(&multimedia_event, 0, sizeof(multimedia_event));
	multimedia_event.report_id = MULTIMEDIA_REPORT_ID;
	if (!init(&multimedia, vendor_id, product_id, error))
		return FALSE;

	return TRUE;
}

gboolean gaminggear_input_event_deinit(GError **error) {
	if (!deinit(&multimedia, error))
		return FALSE;
	if (!deinit(&mouse, error))
		return FALSE;
	if (!deinit(&keyboard, error))
		return FALSE;
	return TRUE;
}

static gboolean set_bit(guint8 *byte, guint bit, gboolean new_value) {
	gboolean old_value = (*byte & (1 << bit)) ? TRUE : FALSE;
	if (new_value)
		*byte |= 1 << bit;
	else
		*byte &= ~(1 << bit);
	return old_value == new_value;
}

/* returns TRUE if event is modified and should be sent */
static gboolean keyboard_add_key(int hid) {
	if (hid >= HID_UID_KB_LEFT_CONTROL && hid <= HID_UID_KB_RIGHT_GUI) {
		if (set_bit(&keyboard_event.special, hid - HID_UID_KB_LEFT_CONTROL, TRUE)) {
			g_warning(_("Uhid keyboard: Hid code %i had same state"), hid);
			return FALSE;
		}
		return TRUE;
	}

	if (keyboard_event_next_key_index == KEYBOARD_KEY_NUM) {
		g_warning(_("Uhid keyboard: Too many keys pressed at the same time. Could not press %i"), hid);
		return FALSE;
	}

	keyboard_event.keys[keyboard_event_next_key_index] = hid;
	++keyboard_event_next_key_index;
	return TRUE;
}

/* returns TRUE if event is modified and should be sent */
static gboolean keyboard_remove_key(int hid) {
	guint i;
	gint found;

	if (hid >= HID_UID_KB_LEFT_CONTROL && hid <= HID_UID_KB_RIGHT_GUI) {
		if (set_bit(&keyboard_event.special, hid - HID_UID_KB_LEFT_CONTROL, FALSE)) {
			g_warning(_("Uhid keyboard: Hid code %i had same state"), hid);
			return FALSE;
		}
		return TRUE;
	}

	if (keyboard_event_next_key_index == 0) {
		g_warning(_("Uhid keyboard: No key to remove"));
		return FALSE;
	}

	found = -1;
	for (i = 0; i < keyboard_event_next_key_index; ++i) {
		if (keyboard_event.keys[i] == hid) { /* found element to delete */
			found = i;
			break;
		}
	}

	if (found == -1) {
		g_warning(_("Uhid keyboard: Hid code %i was not pressed before"), hid);
		return FALSE;
	}

	/* shifting positions to left */
	for (i = found; i < keyboard_event_next_key_index; ++i) {
		if (i == keyboard_event_next_key_index - 1)
			keyboard_event.keys[i] = 0;
		else
			keyboard_event.keys[i] = keyboard_event.keys[i + 1];
	}

	--keyboard_event_next_key_index;
	return TRUE;
}

/* returns TRUE if event is modified and should be sent */
static gboolean keyboard_handle_event(int hid, int value) {
	gboolean retval = FALSE;

	if (value == GAMINGGEAR_INPUT_EVENT_VALUE_PRESS) {
		if (active_keyboard_hids[hid] == G_MAXUINT8) {
			g_warning(_("Uhid keyboard: Hid code %i was pressed too many times"), hid);
			return FALSE;
		}

		if (active_keyboard_hids[hid] == 0)
			retval = keyboard_add_key(hid);

		++active_keyboard_hids[hid];
	} else {
		if (active_keyboard_hids[hid] == 0) {
			g_warning(_("Uhid keyboard: Hid code %i was not pressed before"), hid);
			return FALSE;
		}

		--active_keyboard_hids[hid];

		if (active_keyboard_hids[hid] == 0)
			retval = keyboard_remove_key(hid);
	}

	return retval;
}

void gaminggear_input_event_write_keyboard(int hid, int value) {
	if (keyboard_handle_event(hid, value))
		send_event(&keyboard);
}

void gaminggear_input_event_write_keyboard_multi(int *hids, gsize length, int value) {
	gsize i;
	gboolean changed = FALSE;

	for (i = 0; i < length; ++i) {
		if (keyboard_handle_event(hids[i], value))
			changed = TRUE;
	}

	if (changed)
		send_event(&keyboard);
}

void gaminggear_input_event_write_button(int hid, int value) {
	guint bit = hid - GAMINGGEAR_MACRO_KEYSTROKE_KEY_BUTTON_LEFT;
	gboolean changed = FALSE;

	if (bit >= MOUSE_BUTTON_NUM) {
		g_warning(_("Uhid mouse: button %i not supported"), bit + 1);
		return;
	}

	if (value == GAMINGGEAR_INPUT_EVENT_VALUE_PRESS) {
		if (active_mouse_buttons[bit] == G_MAXUINT8) {
			g_warning(_("Uhid mouse: button %i was pressed too many times"), bit + 1);
			return;
		}

		if (active_mouse_buttons[bit] == 0) {
			if (set_bit(&mouse_event.buttons, bit, TRUE))
				g_warning(_("Uhid mouse: button %i had same state"), bit + 1);
			else
				changed = TRUE;
		}

		++active_mouse_buttons[bit];
	} else {
		if (active_mouse_buttons[bit] == 0) {
			g_warning(_("Uhid mouse: button %i was not pressed before"), bit + 1);
			return;
		}

		--active_mouse_buttons[bit];

		if (active_mouse_buttons[bit] == 0) {
			if (set_bit(&mouse_event.buttons, bit, FALSE))
				g_warning(_("Uhid mouse: button %i had same state"), bit + 1);
			else
				changed = TRUE;
		}
	}

	if (changed)
		send_event(&mouse);
}

void gaminggear_input_event_write_multimedia(int hid, int value) {
	if (value == GAMINGGEAR_INPUT_EVENT_VALUE_PRESS) {
		if (multimedia_event.value != 0)
			g_warning(_("Uhid multimedia: already other event %i active"), multimedia_event.value);
		multimedia_event.value = GUINT16_TO_LE(hid);
	} else {
		if (multimedia_event.value == 0)
			g_warning(_("Uhid multimedia: no event active"));
		else if (multimedia_event.value != hid)
			g_warning(_("Uhid multimedia: already other event %i active"), multimedia_event.value);
		multimedia_event.value = GUINT16_TO_LE(0);
	}
	send_event(&multimedia);
}

void gaminggear_input_event_write_multimedia_single(int hid) {
	gaminggear_input_event_write_multimedia(hid, GAMINGGEAR_INPUT_EVENT_VALUE_PRESS);
	gaminggear_input_event_write_multimedia(hid, GAMINGGEAR_INPUT_EVENT_VALUE_RELEASE);
}
