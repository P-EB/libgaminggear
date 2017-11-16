#ifndef __GAMINGGEAR_MACRO_EDITOR_INTERFACE_H__
#define __GAMINGGEAR_MACRO_EDITOR_INTERFACE_H__

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

#include "gaminggear/macro.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GAMINGGEAR_MACRO_EDITOR_INTERFACE_TYPE (gaminggear_macro_editor_interface_get_type())
#define GAMINGGEAR_MACRO_EDITOR_INTERFACE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GAMINGGEAR_MACRO_EDITOR_INTERFACE_TYPE, GaminggearMacroEditorInterface))
#define IS_GAMINGGEAR_MACRO_EDITOR_INTERFACE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GAMINGGEAR_MACRO_EDITOR_INTERFACE_TYPE))
#define GAMINGGEAR_MACRO_EDITOR_INTERFACE_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE((inst), GAMINGGEAR_MACRO_EDITOR_INTERFACE_TYPE, GaminggearMacroEditorInterfaceInterface))

typedef struct _GaminggearMacroEditorInterface GaminggearMacroEditorInterface;
typedef struct _GaminggearMacroEditorInterfaceInterface GaminggearMacroEditorInterfaceInterface;

struct _GaminggearMacroEditorInterfaceInterface {
	GTypeInterface parent;

	/* signals */

	void (*modified)(GaminggearMacroEditorInterface *self);
	void (*empty_changed)(GaminggearMacroEditorInterface *self);

	/* virtual table */

	gchar const *(*name)(GaminggearMacroEditorInterface *self);

	void (*clear)(GaminggearMacroEditorInterface *self);

	/* should return true if macro has no keystrokes */
	gboolean (*empty)(GaminggearMacroEditorInterface *self);

	/*
	 * times are in milliseconds
	 * rel_time is relative time from last element to this
	 */
	void (*add_keystroke)(GaminggearMacroEditorInterface *self, guint key, guint action, glong rel_time);

	void (*set_keystrokes)(GaminggearMacroEditorInterface *self, GaminggearMacroKeystrokes const *keystrokes);

	GaminggearMacroKeystrokes *(*get_keystrokes)(GaminggearMacroEditorInterface *self);

	gboolean (*get_modified)(GaminggearMacroEditorInterface *self);
	void (*set_modified)(GaminggearMacroEditorInterface *self, gboolean new_state);
};

GType gaminggear_macro_editor_interface_get_type(void);

gchar const *gaminggear_macro_editor_interface_name(GaminggearMacroEditorInterface *self);
void gaminggear_macro_editor_interface_set_keystrokes(GaminggearMacroEditorInterface *self, GaminggearMacroKeystrokes const *keystrokes);
GaminggearMacroKeystrokes *gaminggear_macro_editor_interface_get_keystrokes(GaminggearMacroEditorInterface *self);
void gaminggear_macro_editor_interface_clear(GaminggearMacroEditorInterface *self);
gboolean gaminggear_macro_editor_interface_empty(GaminggearMacroEditorInterface *self);
void gaminggear_macro_editor_interface_add_keystroke(GaminggearMacroEditorInterface *self, guint key, guint action, glong rel_time);
gboolean gaminggear_macro_editor_interface_get_modified(GaminggearMacroEditorInterface *self);
void gaminggear_macro_editor_interface_set_modified(GaminggearMacroEditorInterface *self, gboolean new_state);

void gaminggear_macro_editor_interface_modified(GaminggearMacroEditorInterface *self);
void gaminggear_macro_editor_interface_empty_changed(GaminggearMacroEditorInterface *self);

G_END_DECLS

#endif
