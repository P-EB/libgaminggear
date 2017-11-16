#ifndef __GAMINGGEAR_HID_COMBO_BOX_H__
#define __GAMINGGEAR_HID_COMBO_BOX_H__

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

/*! \file gaminggear/gaminggear_hid_combo_box.h
 *  \brief ComboBox with HID usage id selection.
 */

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GAMINGGEAR_HID_COMBO_BOX_TYPE (gaminggear_hid_combo_box_get_type())
#define GAMINGGEAR_HID_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GAMINGGEAR_HID_COMBO_BOX_TYPE, GaminggearHidComboBox))
#define IS_GAMINGGEAR_HID_COMBO_BOX(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GAMINGGEAR_HID_COMBO_BOX_TYPE))

typedef struct _GaminggearHidComboBox GaminggearHidComboBox;

GType gaminggear_hid_combo_box_get_type(void);

/*! \brief Creates new GaminggearHidComboBox.
 *  \retval widget A new GaminggearHidComboBox.
 */
GtkWidget *gaminggear_hid_combo_box_new(void);

/*! \brief Gets current selected value of combo box.
 *  \param hid_combo_box A GaminggearHidComboBox.
 *  \retval value Actual HID usage id.
 */
gint gaminggear_hid_combo_box_get_value(GaminggearHidComboBox *hid_combo_box);

/*! \brief Sets current value of combo box.
 *  \param hid_combo_box A GaminggearHidComboBox.
 *  \param new_value The new HID usage id.
 */
void gaminggear_hid_combo_box_set_value(GaminggearHidComboBox *hid_combo_box, gint new_value);

G_END_DECLS

#endif
