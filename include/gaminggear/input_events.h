#ifndef __GAMINGGEAR_INPUT_EVENTS_H__
#define __GAMINGGEAR_INPUT_EVENTS_H__

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

/*! \file gaminggear/input_events.h
 *  \brief Event device access
 */

#include <glib.h>

G_BEGIN_DECLS

typedef enum {
	GAMINGGEAR_INPUT_EVENT_VALUE_PRESS = 1,
	GAMINGGEAR_INPUT_EVENT_VALUE_RELEASE = 0,
} GaminggearInputEventValue;

/*! \brief Initialize input event support of libgaminggear.
 *  \param vendor_id The vendor id the device should have.
 *  \param product_id The product id the device should have.
 *  \param error Return location for error or \c NULL.
 *  \retval condition \c TRUE on success, \c FALSE else.
 *  \since 1.0
 */
gboolean gaminggear_input_event_init(guint vendor_id, guint product_id, GError **error);

/*! \brief Deinitialize input event support of libgaminggear.
 *  \param error Return location for error or \c NULL.
 *  \retval condition \c TRUE on success, \c FALSE else.
 *  \since 1.0
 */
gboolean gaminggear_input_event_deinit(GError **error);

/*! \brief Write a keyboard event.
 *  \param hid usage id of keyboard page.
 *  \param value GaminggearInputEventValue.
 *  \since 1.0
 */
void gaminggear_input_event_write_keyboard(int hid, int value);

/*! \brief Write multiple keyboard events.
 *  \param hids of hid usage ids of keyboard page.
 *  \param length of array.
 *  \param value GaminggearInputEventValue.
 *  \since 1.0
 */
void gaminggear_input_event_write_keyboard_multi(int *hids, gsize length, int value);

/*! \brief Write a mouse button event.
 *  \param hid mouse button encoded in hid usage id (see GaminggearMacroKeystrokeKey).
 *  \param value GaminggearInputEventValue.
 *  \since 1.0
 */
void gaminggear_input_event_write_button(int hid, int value);

/*! \brief Write a multimedia event.
 *  \param hid usage id of consumer page.
 *  \param value GaminggearInputEventValue.
 *  \since 1.0
 */
void gaminggear_input_event_write_multimedia(int hid, int value);

/*! \brief Write a full multimedia event (press/release).
 *  \param hid usage id of consumer page.
 *  \since 1.0
 */
void gaminggear_input_event_write_multimedia_single(int hid);

G_END_DECLS

#endif
