#ifndef __GAMINGGEAR_MACRO_EDITOR_ADVANCED_H__
#define __GAMINGGEAR_MACRO_EDITOR_ADVANCED_H__

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

#include "gaminggear_macro_editor_interface.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GAMINGGEAR_MACRO_EDITOR_ADVANCED_TYPE (gaminggear_macro_editor_advanced_get_type())
#define GAMINGGEAR_MACRO_EDITOR_ADVANCED(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GAMINGGEAR_MACRO_EDITOR_ADVANCED_TYPE, GaminggearMacroEditorAdvanced))
#define IS_GAMINGGEAR_MACRO_EDITOR_ADVANCED(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GAMINGGEAR_MACRO_EDITOR_ADVANCED_TYPE))

typedef struct _GaminggearMacroEditorAdvanced GaminggearMacroEditorAdvanced;
typedef struct _GaminggearMacroEditorAdvancedPrivate GaminggearMacroEditorAdvancedPrivate;

struct _GaminggearMacroEditorAdvanced {
	GtkScrolledWindow window;
	GaminggearMacroEditorAdvancedPrivate *priv;
};

GType gaminggear_macro_editor_advanced_get_type(void);
GtkWidget *gaminggear_macro_editor_advanced_new(void);

G_END_DECLS

#endif
