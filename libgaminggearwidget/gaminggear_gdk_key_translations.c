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

#include "gaminggear/gdk_key_translations.h"
#include "gaminggear/key_translations.h"
#include "gaminggear/macro.h"
#include "gaminggear/hid_uid.h"
#include "gaminggear_helper.h"
#include "i18n-lib.h"

static guint gaminggear_xkeycode_to_keyval(guint keycode) {
	GdkKeymap *keymap;
	GdkKeymapKey key = {keycode, 0, 0};
	guint keyval;

	keymap = gdk_keymap_get_default();
	keyval = gdk_keymap_lookup_key(keymap, &key);
	if (keyval == 0)
		g_warning(_("Xkeycode 0x%04x has no corresponding keyval in keymap"), keycode);
	return keyval;
}

static gchar *gaminggear_keyval_to_keyname(guint keyval) {
	guint32 unicode;

	unicode = gdk_keyval_to_unicode(keyval);

	if (g_unichar_isgraph(unicode))
		return g_gaminggear_unichar_to_utf8(unicode);
	else
		return g_strdup(gdk_keyval_name(keyval));
}

gchar *gaminggear_xkeycode_to_keyname(guint keycode) {
	return gaminggear_keyval_to_keyname(gaminggear_xkeycode_to_keyval(keycode));
}

gchar *gaminggear_hid_to_keyname(guint8 usage_id) {
	gchar *string = NULL;

	switch (usage_id) {
	case GAMINGGEAR_MACRO_KEYSTROKE_KEY_BUTTON_LEFT:
		string = g_strdup(_("Button left"));
		break;
	case GAMINGGEAR_MACRO_KEYSTROKE_KEY_BUTTON_RIGHT:
		string = g_strdup(_("Button right"));
		break;
	case GAMINGGEAR_MACRO_KEYSTROKE_KEY_BUTTON_MIDDLE:
		string = g_strdup(_("Button middle"));
		break;
	default:
		if (usage_id >= GAMINGGEAR_MACRO_KEYSTROKE_KEY_BUTTON_LEFT)
			string = g_strdup_printf(_("Button %u"), usage_id - GAMINGGEAR_MACRO_KEYSTROKE_KEY_BUTTON_LEFT + 1);
		else
			string = gaminggear_xkeycode_to_keyname(gaminggear_hid_to_xkeycode(usage_id));
	}
	return string;
}

gchar *gaminggear_hid_to_button_name(guint8 usage_id) {
	gchar *text = NULL;

	switch(usage_id) {
	case HID_UID_KB_ENTER:
		text = "\xe2\x86\xb5";
		break;
	case HID_UID_KB_ESCAPE:
		text = "ESC";
		break;
	case HID_UID_KB_BACKSPACE:
		text = "\xe2\x9f\xb5";
		break;
	case HID_UID_KB_TAB:
		text = "\xe2\x86\xb9";
		break;
	case HID_UID_KB_SPACE:
		text = "Space";
		break;
	case HID_UID_KB_CAPS_LOCK:
		text = "\xe2\x87\xa9";
		break;
	case HID_UID_KB_SCROLL_LOCK:
		text = "Scroll\nLock";
		break;
	case HID_UID_KB_PAGE_UP:
		text = "Pg\xe2\x86\x91";
		break;
	case HID_UID_KB_PAGE_DOWN:
		text = "Pg\xe2\x86\x93";
		break;
	case HID_UID_KB_RIGHT_ARROW:
		text = "\xe2\x86\x92";
		break;
	case HID_UID_KB_LEFT_ARROW:
		text = "\xe2\x86\x90";
		break;
	case HID_UID_KB_DOWN_ARROW:
		text = "\xe2\x86\x93";
		break;
	case HID_UID_KB_UP_ARROW:
		text = "\xe2\x86\x91";
		break;
	case HID_UID_KP_NUM_LOCK:
		text = "Num\nLock";
		break;
	case HID_UID_KP_DIV:
		text = "/\nKP";
		break;
	case HID_UID_KP_MUL:
		text = "*\nKP";
		break;
	case HID_UID_KP_MINUS:
		text = "-\nKP";
		break;
	case HID_UID_KP_PLUS:
		text = "+\nKP";
		break;
	case HID_UID_KP_ENTER:
		text = "Enter";
		break;
	case HID_UID_KP_1:
		text = "1\nKP";
		break;
	case HID_UID_KP_2:
		text = "2\nKP";
		break;
	case HID_UID_KP_3:
		text = "3\nKP";
		break;
	case HID_UID_KP_4:
		text = "4\nKP";
		break;
	case HID_UID_KP_5:
		text = "5\nKP";
		break;
	case HID_UID_KP_6:
		text = "6\nKP";
		break;
	case HID_UID_KP_7:
		text = "7\nKP";
		break;
	case HID_UID_KP_8:
		text = "8\nKP";
		break;
	case HID_UID_KP_9:
		text = "9\nKP";
		break;
	case HID_UID_KP_0:
		text = "0\nKP";
		break;
	case HID_UID_KP_DELETE:
		text = "Delete\nKP";
		break;
	case HID_UID_KB_LEFT_CONTROL:
		text = "Ctrl\nL";
		break;
	case HID_UID_KB_LEFT_SHIFT:
		text = "\xe2\x87\xa7\nL";
		break;
	case HID_UID_KB_LEFT_ALT:
		text = "Alt\nL";
		break;
	case HID_UID_KB_LEFT_GUI:
		text = "GUI\nL";
		break;
	case HID_UID_KB_RIGHT_CONTROL:
		text = "Ctrl\nR";
		break;
	case HID_UID_KB_RIGHT_SHIFT:
		text = "\xe2\x87\xa7\nR";
		break;
	case HID_UID_KB_RIGHT_ALT:
		text = "Alt\nR";
		break;
	case HID_UID_KB_RIGHT_GUI:
		text = "GUI\nR";
		break;
	default:
		break;
	}

	if (text)
		return g_strdup(text);
	else
		return gaminggear_hid_to_keyname(usage_id);
}

gchar *gaminggear_hid_to_name(guint8 usage_id) {
	gchar *text = NULL;

	switch(usage_id) {
	case HID_UID_KB_SPACE:
		text = "Space";
		break;
	case HID_UID_KB_CAPS_LOCK:
		text = "Caps lock";
		break;
	case HID_UID_KB_SCROLL_LOCK:
		text = "Scroll lock";
		break;
	case HID_UID_KB_PAGE_UP:
		text = "Page up";
		break;
	case HID_UID_KB_PAGE_DOWN:
		text = "Page down";
		break;
	case HID_UID_KP_NUM_LOCK:
		text = "Num lock";
		break;
	case HID_UID_KP_DIV:
		text = "Keypad /";
		break;
	case HID_UID_KP_MUL:
		text = "Keypad *";
		break;
	case HID_UID_KP_MINUS:
		text = "Keypad -";
		break;
	case HID_UID_KP_PLUS:
		text = "Keypad +";
		break;
	case HID_UID_KP_ENTER:
		text = "Keypad Enter";
		break;
	case HID_UID_KP_1:
		text = "Keypad 1";
		break;
	case HID_UID_KP_2:
		text = "Keypad 2";
		break;
	case HID_UID_KP_3:
		text = "Keypad 3";
		break;
	case HID_UID_KP_4:
		text = "Keypad 4";
		break;
	case HID_UID_KP_5:
		text = "Keypad 5";
		break;
	case HID_UID_KP_6:
		text = "Keypad 6";
		break;
	case HID_UID_KP_7:
		text = "Keypad 7";
		break;
	case HID_UID_KP_8:
		text = "Keypad 8";
		break;
	case HID_UID_KP_9:
		text = "Keypad 9";
		break;
	case HID_UID_KP_0:
		text = "Keypad 0";
		break;
	case HID_UID_KB_LEFT_CONTROL:
		text = "Left control";
		break;
	case HID_UID_KB_LEFT_SHIFT:
		text = "Left shift";
		break;
	case HID_UID_KB_LEFT_ALT:
		text = "Left alt";
		break;
	case HID_UID_KB_LEFT_GUI:
		text = "Left gui";
		break;
	case HID_UID_KB_RIGHT_CONTROL:
		text = "Right control";
		break;
	case HID_UID_KB_RIGHT_SHIFT:
		text = "Right shift";
		break;
	case HID_UID_KB_RIGHT_ALT:
		text = "Right alt";
		break;
	case HID_UID_KB_RIGHT_GUI:
		text = "Right gui";
		break;
	default:
		break;
	}

	if (text)
		return g_strdup(text);
	else
		return gaminggear_hid_to_keyname(usage_id);
}
