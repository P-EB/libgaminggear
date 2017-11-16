#ifndef __GAMINGGEAR_MACRO_EDITOR_H__
#define __GAMINGGEAR_MACRO_EDITOR_H__

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

#define GAMINGGEAR_MACRO_EDITOR_TYPE (gaminggear_macro_editor_get_type())
#define GAMINGGEAR_MACRO_EDITOR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GAMINGGEAR_MACRO_EDITOR_TYPE, GaminggearMacroEditor))
#define IS_GAMINGGEAR_MACRO_EDITOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GAMINGGEAR_MACRO_EDITOR_TYPE))

typedef struct _GaminggearMacroEditor GaminggearMacroEditor;
typedef struct _GaminggearMacroEditorPrivate GaminggearMacroEditorPrivate;

struct _GaminggearMacroEditor {
	GtkHPaned window;
	GaminggearMacroEditorPrivate *priv;
};

GType gaminggear_macro_editor_get_type(void);
GtkWidget *gaminggear_macro_editor_new(void);

void gaminggear_macro_editor_clear(GaminggearMacroEditor *macro_editor);
void gaminggear_macro_editor_add_macros(GaminggearMacroEditor *macro_editor, GaminggearMacros *macros);
GaminggearMacros *gaminggear_macro_editor_get_macros(GaminggearMacroEditor *macro_editor);

void gaminggear_macro_editor_set_modified(GaminggearMacroEditor *macro_editor, gboolean state);
gboolean gaminggear_macro_editor_get_modified(GaminggearMacroEditor *macro_editor);

G_END_DECLS

#endif
