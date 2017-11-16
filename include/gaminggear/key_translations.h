#ifndef __GAMINGGEAR_KEY_TRANSLATIONS_H__
#define __GAMINGGEAR_KEY_TRANSLATIONS_H__

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

/*! \file gaminggear/key_translations.h
 *  \brief Key translations.
 *
 *  Conversions happen between following types:
 *
 *  - HID usage id: the type stored in macros
 *  - keyboard/button keycode: is used by kernel code (evdev events)
 *  - xkeycode: means hardware keycode as defined by \c GdkEventKey
 *
 *  hid <--> kbd_keycode <--> xkeycode --> (keyval) --> keyname
 */

#include <glib.h>

G_BEGIN_DECLS

/*! \brief Determines if HID usage id is a mouse button.
 *
 *  Gaminggear encodes mouse buttons in macro keystroke keys in upper invalid range
 *  of HID usage ids.
 *
 *  \param usage_id HID usage id.
 *  \retval boolean \c TRUE if usage id is a mouse button.
 *  \since 1.0
 */
gboolean gaminggear_hid_is_mouse_button(guint8 usage_id);

/*! \brief Converts HID usage id to button keycode.
 *
 *  Uses the values defined in linux/input.h.
 *
 *  \param usage_id HID usage id.
 *  \retval btn_keycode Button keycode.
 *  \since 1.0
 */
guint gaminggear_hid_to_btn_keycode(guint8 usage_id);

/*! \brief Converts HID usage id to keyboard keycode.
 *
 *  Uses the same table as linux kernel.
 *
 *  \param usage_id HID usage id.
 *  \retval kbd_keycode Keyboard keycode.
 *  \since 1.0
 */
guint gaminggear_hid_to_kbd_keycode(guint8 usage_id);

guint gaminggear_consumer_page_to_kbd_keycode(guint8 usage_id);

/*! \brief Converts xkeycode to HID usage id.
 *  \param xkeycode Hardware keycode.
 *  \retval usage_id HID usage id.
 *  \since 1.0
 */
guint8 gaminggear_xkeycode_to_hid(guint16 xkeycode);

/*! \brief Converts HID usage id to xkeycode.
 *  \param usage_id HID usage id.
 *  \retval xkeycode Hardware keycode.
 *  \since 1.0
 */
guint16 gaminggear_hid_to_xkeycode(guint8 usage_id);

G_END_DECLS

#endif
