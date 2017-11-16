#ifndef __GAMINGGEAR_GDK_KEY_TRANSLATIONS_H__
#define __GAMINGGEAR_GDK_KEY_TRANSLATIONS_H__

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

/*! \file gaminggear/gdk_key_translations.h
 *  \brief Keymap dependent key translations.
 */

#include <gdk/gdk.h>

G_BEGIN_DECLS

/*! \brief Converts keycode to gdk keyname.
 *
 *  Needs GDK to be initialized.
 *
 *  \param keycode Hardware keycode.
 *  \retval keyname that has to be freed with \c g_free().
 *  \since 1.0
 */
gchar *gaminggear_xkeycode_to_keyname(guint keycode);

/*! \brief Converts HID usage id to gdk keyname.
 *
 *  Needs GDK to be initialized.
 *  Also handles mouse button names.
 *
 *  \param usage_id HID usage id.
 *  \retval keyname that has to be freed with \c g_free().
 *  \since 1.0
 */
gchar *gaminggear_hid_to_keyname(guint8 usage_id);

/*! \brief Converts HID usage id to HID name.
 *
 *  Needs GDK to be initialized.
 *
 *  \param usage_id HID usage id.
 *  \retval name that has to be freed with \c g_free().
 *  \since 1.0
 */
gchar *gaminggear_hid_to_name(guint8 usage_id);

/*! \brief Converts HID usage id to short HID name.
 *
 *  Needs GDK to be initialized.
 *
 *  Returns a shorter, two line HID name that is suitable for Keyboard buttons.
 *
 *  \param usage_id HID usage id.
 *  \retval name that has to be freed with \c g_free().
 *  \since 1.0
 */
gchar *gaminggear_hid_to_button_name(guint8 usage_id);

G_END_DECLS

#endif
