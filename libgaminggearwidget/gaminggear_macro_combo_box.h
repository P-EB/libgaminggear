#ifndef __GAMINGGEAR_MACRO_COMBO_BOX_H__
#define __GAMINGGEAR_MACRO_COMBO_BOX_H__

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

#include "gaminggear_macros.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

GtkWidget *gaminggear_macro_combo_box_new(void);
void gaminggear_macro_combo_box_set_macros(GtkComboBox *combo_box, GaminggearMacros *macros);

GaminggearMacro *gaminggear_macro_combo_box_get_value(GtkComboBox *combo);

G_END_DECLS

#endif
