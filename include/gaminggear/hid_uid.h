#ifndef __HID_UID_H__
#define __HID_UID_H__

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

 /*! \file gaminggear/hid_uid.h
  *  \brief HID usage id definitions.
  */

typedef enum {
	HID_UID_KB_A = 0x04,
	HID_UID_KB_B = 0x05,
	HID_UID_KB_C = 0x06,
	HID_UID_KB_D = 0x07,
	HID_UID_KB_E = 0x08,
	HID_UID_KB_F = 0x09,
	HID_UID_KB_G = 0x0a,
	HID_UID_KB_H = 0x0b,
	HID_UID_KB_I = 0x0c,
	HID_UID_KB_J = 0x0d,
	HID_UID_KB_K = 0x0e,
	HID_UID_KB_L = 0x0f,
	HID_UID_KB_M = 0x10,
	HID_UID_KB_N = 0x11,
	HID_UID_KB_O = 0x12,
	HID_UID_KB_P = 0x13,
	HID_UID_KB_Q = 0x14,
	HID_UID_KB_R = 0x15,
	HID_UID_KB_S = 0x16,
	HID_UID_KB_T = 0x17,
	HID_UID_KB_U = 0x18,
	HID_UID_KB_V = 0x19,
	HID_UID_KB_W = 0x1a,
	HID_UID_KB_X = 0x1b,
	HID_UID_KB_Y = 0x1c,
	HID_UID_KB_Z = 0x1d,
	HID_UID_KB_1 = 0x1e,
	HID_UID_KB_2 = 0x1f,
	HID_UID_KB_3 = 0x20,
	HID_UID_KB_4 = 0x21,
	HID_UID_KB_5 = 0x22,
	HID_UID_KB_6 = 0x23,
	HID_UID_KB_7 = 0x24,
	HID_UID_KB_8 = 0x25,
	HID_UID_KB_9 = 0x26,
	HID_UID_KB_0 = 0x27,
	HID_UID_KB_ENTER = 0x28,
	HID_UID_KB_ESCAPE = 0x29,
	HID_UID_KB_BACKSPACE = 0x2a,
	HID_UID_KB_TAB = 0x2b,
	HID_UID_KB_SPACE = 0x2c,

	HID_UID_KB_CAPS_LOCK = 0x39,
	HID_UID_KB_F1 = 0x3a,
	HID_UID_KB_F2 = 0x3b,
	HID_UID_KB_F3 = 0x3c,
	HID_UID_KB_F4 = 0x3d,
	HID_UID_KB_F5 = 0x3e,
	HID_UID_KB_F6 = 0x3f,
	HID_UID_KB_F7 = 0x40,
	HID_UID_KB_F8 = 0x41,
	HID_UID_KB_F9 = 0x42,
	HID_UID_KB_F10 = 0x43,
	HID_UID_KB_F11 = 0x44,
	HID_UID_KB_F12 = 0x45,
	HID_UID_KB_PRINT_SCREEN = 0x46,
	HID_UID_KB_SCROLL_LOCK = 0x47,
	HID_UID_KB_PAUSE = 0x48,
	HID_UID_KB_INSERT = 0x49,
	HID_UID_KB_HOME = 0x4a,
	HID_UID_KB_PAGE_UP = 0x4b,
	HID_UID_KB_DELETE = 0x4c,
	HID_UID_KB_END = 0x4d,
	HID_UID_KB_PAGE_DOWN = 0x4e,
	HID_UID_KB_RIGHT_ARROW = 0x4f,
	HID_UID_KB_LEFT_ARROW = 0x50,
	HID_UID_KB_DOWN_ARROW = 0x51,
	HID_UID_KB_UP_ARROW = 0x52,
	HID_UID_KP_NUM_LOCK = 0x53,
	HID_UID_KP_DIV = 0x54,
	HID_UID_KP_MUL = 0x55,
	HID_UID_KP_MINUS = 0x56,
	HID_UID_KP_PLUS = 0x57,
	HID_UID_KP_ENTER = 0x58,
	HID_UID_KP_1 = 0x59,
	HID_UID_KP_2 = 0x5a,
	HID_UID_KP_3 = 0x5b,
	HID_UID_KP_4 = 0x5c,
	HID_UID_KP_5 = 0x5d,
	HID_UID_KP_6 = 0x5e,
	HID_UID_KP_7 = 0x5f,
	HID_UID_KP_8 = 0x60,
	HID_UID_KP_9 = 0x61,
	HID_UID_KP_0 = 0x62,
	HID_UID_KP_DELETE = 0x63,

	HID_UID_KB_APPLICATION = 0x65,

	HID_UID_KB_LEFT_CONTROL = 0xe0,
	HID_UID_KB_LEFT_SHIFT = 0xe1,
	HID_UID_KB_LEFT_ALT = 0xe2,
	HID_UID_KB_LEFT_GUI = 0xe3,
	HID_UID_KB_RIGHT_CONTROL = 0xe4,
	HID_UID_KB_RIGHT_SHIFT = 0xe5,
	HID_UID_KB_RIGHT_ALT = 0xe6,
	HID_UID_KB_RIGHT_GUI = 0xe7,
} HidUsageIdKeyboard;

typedef enum {
	HID_UID_CP_NEXTSONG = 0xb5,
	HID_UID_CP_PREVIOUSSONG = 0xb6,
	HID_UID_CP_STOP = 0xb7,

	HID_UID_CP_PLAYPAUSE = 0xcd,

	HID_UID_CP_MUTE = 0xe2,

	HID_UID_CP_VOLUMEUP = 0xe9,
	HID_UID_CP_VOLUMEDOWN = 0xea,

	HID_UID_AL_CONSUMER_CONTROL_CONFIG = 0x183,

	HID_UID_AL_EMAIL = 0x18a,

	HID_UID_AL_CALCULATOR = 0x192,

	HID_UID_AL_LOCAL_MACHINE_BROWSER = 0x194,

	HID_UID_AC_SEARCH = 0x221,

	HID_UID_AC_HOME = 0x223,
	HID_UID_AC_BACK = 0x224,
	HID_UID_AC_FORWARD = 0x225,
	HID_UID_AC_STOP = 0x226,
	HID_UID_AC_REFRESH = 0x227,
} HidUsageIdConsumer;

#endif
