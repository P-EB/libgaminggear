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

#include "gaminggear/key_translations.h"
#include "gaminggear/macro.h"
#include "gaminggear/hid_uid.h"
#include "i18n-lib.h"
#include <linux/input.h>

/*
 * QUOTE table data for hid_usage_id to kbd_keycode conversion copied from Linux-3.8
 * drivers/hid/hid-input.c:hid_keyboard[]
 * https://www.kernel.org/
 * GPLv2
 * Copyright (c) 1999-2001 Vojtech Pavlik
 * Copyright (c) 2006-2010 Jiri Kosina
 */
static guchar const table_hid_usage_to_kbd_keycode[256] = {
	/*0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F */
	  0,   0,   0,   0,  30,  48,  46,  32,  18,  33,  34,  35,  23,  36,  37,  38, /* 0 */
	 50,  49,  24,  25,  16,  19,  31,  20,  22,  47,  17,  45,  21,  44,   2,   3, /* 1 */
	  4,   5,   6,   7,   8,   9,  10,  11,  28,   1,  14,  15,  57,  12,  13,  26, /* 2 */
	 27,  43,  43,  39,  40,  41,  51,  52,  53,  58,  59,  60,  61,  62,  63,  64, /* 3 */
	 65,  66,  67,  68,  87,  88,  99,  70, 119, 110, 102, 104, 111, 107, 109, 106, /* 4 */
	105, 108, 103,  69,  98,  55,  74,  78,  96,  79,  80,  81,  75,  76,  77,  71, /* 5 */
	 72,  73,  82,  83,  86, 127, 116, 117, 183, 184, 185, 186, 187, 188, 189, 190, /* 6 */
	191, 192, 193, 194, 134, 138, 130, 132, 128, 129, 131, 137, 133, 135, 136, 113, /* 7 */
	115, 114,   0,   0,   0, 121,   0,  89,  93, 124,  92,  94,  95,   0,   0,   0, /* 8 */
	122, 123,  90,  91,  85,   0,   0,   0,   0,   0,   0,   0, 111,   0,   0,   0, /* 9 */
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, /* A */
	  0,   0,   0,   0,   0,   0, 179, 180,   0,   0,   0,   0,   0,   0,   0,   0, /* B */
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, /* C */
	  0,   0,   0,   0,   0,   0,   0,   0, 111,   0,   0,   0,   0,   0,   0,   0, /* D */
	 29,  42,  56, 125,  97,  54, 100, 126, 164, 166, 165, 163, 161, 115, 114, 113, /* E */
	150, 158, 159, 128, 136, 177, 178, 176, 142, 152, 173, 140,   0,   0,   0,   0  /* F */
};

gboolean gaminggear_hid_is_mouse_button(guint8 usage_id) {
	return (usage_id >= GAMINGGEAR_MACRO_KEYSTROKE_KEY_BUTTON_LEFT) ? TRUE : FALSE;
}

static struct {
	guint8 usage_id;
	guint keycode;
} consumer_page_to_keycode[7] = {
	{HID_UID_CP_NEXTSONG, KEY_NEXTSONG},
	{HID_UID_CP_PREVIOUSSONG, KEY_PREVIOUSSONG},
	{HID_UID_CP_STOP, KEY_STOPCD},
	{HID_UID_CP_PLAYPAUSE, KEY_PLAYPAUSE},
	{HID_UID_CP_MUTE, KEY_MUTE},
	{HID_UID_CP_VOLUMEUP, KEY_VOLUMEUP},
	{HID_UID_CP_VOLUMEDOWN, KEY_VOLUMEDOWN},
};

guint gaminggear_consumer_page_to_kbd_keycode(guint8 usage_id) {
	guint i;

	for (i = 0; i < 7; ++i)
		if (consumer_page_to_keycode[i].usage_id == usage_id)
			return consumer_page_to_keycode[i].keycode;
	return 0;
}

guint gaminggear_hid_to_kbd_keycode(guint8 usage_id) {
	return table_hid_usage_to_kbd_keycode[usage_id];
}

guint gaminggear_hid_to_btn_keycode(guint8 usage_id) {
	if (usage_id >= GAMINGGEAR_MACRO_KEYSTROKE_KEY_BUTTON_LEFT)
		return usage_id - GAMINGGEAR_MACRO_KEYSTROKE_KEY_BUTTON_LEFT + BTN_LEFT;
	else
		return 0;
}

static guint8 gaminggear_kbd_keycode_to_hid(guint kbd_keycode) {
	guint i;

	for (i = 0; i < 256; ++i) {
		if (kbd_keycode == table_hid_usage_to_kbd_keycode[i])
			return i;
	}
	return 0;
}

/*
 * These two functions are valid when using evdev
 * xf86-input-evdev-2.89.0/src/evdev.c:EvdevQueueKbdEvent()
 */
static guint16 gaminggear_kbd_keycode_to_xkeycode(guint kbd_keycode) {
	return kbd_keycode + 8;
}

static guint gaminggear_xkeycode_to_kbd_keycode(guint16 xkeycode) {
	return xkeycode - 8;
}

guint8 gaminggear_xkeycode_to_hid(guint16 xkeycode) {
	guint8 result;
	result = gaminggear_kbd_keycode_to_hid(gaminggear_xkeycode_to_kbd_keycode(xkeycode));
	if (result == 0)
		g_warning(_("Xkeycode 0x%04x has no corresponding HID usage id"), xkeycode);
	return result;
}

guint16 gaminggear_hid_to_xkeycode(guint8 usage_id) {
	guint16 result;
	result = gaminggear_kbd_keycode_to_xkeycode(gaminggear_hid_to_kbd_keycode(usage_id));
	if (result == 0)
		g_warning(_("HID usage id 0x%02x has no corresponding xkeycode"), usage_id);
	return result;
}
