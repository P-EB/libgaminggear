#ifndef __GAMINGGEAR_MACRO_EDITOR_DIALOG_MENU_BAR_H__
#define __GAMINGGEAR_MACRO_EDITOR_DIALOG_MENU_BAR_H__

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

#include "gaminggear/gaminggear_macro_editor_dialog.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

GtkWidget *gaminggear_macro_editor_dialog_menu_bar_new(GaminggearMacroEditorDialog *dialog);

G_END_DECLS

#endif
