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

enum {
	MODIFIED,
	EMPTY_CHANGED,
	LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL] = { 0 };

gchar const *gaminggear_macro_editor_interface_name(GaminggearMacroEditorInterface *self) {
	return GAMINGGEAR_MACRO_EDITOR_INTERFACE_GET_INTERFACE(self)->name(self);
}

void gaminggear_macro_editor_interface_set_keystrokes(GaminggearMacroEditorInterface *self, GaminggearMacroKeystrokes const *keystrokes) {
	GAMINGGEAR_MACRO_EDITOR_INTERFACE_GET_INTERFACE(self)->set_keystrokes(self, keystrokes);
}

GaminggearMacroKeystrokes *gaminggear_macro_editor_interface_get_keystrokes(GaminggearMacroEditorInterface *self) {
	return GAMINGGEAR_MACRO_EDITOR_INTERFACE_GET_INTERFACE(self)->get_keystrokes(self);
}

void gaminggear_macro_editor_interface_clear(GaminggearMacroEditorInterface *self) {
	GAMINGGEAR_MACRO_EDITOR_INTERFACE_GET_INTERFACE(self)->clear(self);
}

gboolean gaminggear_macro_editor_interface_empty(GaminggearMacroEditorInterface *self) {
	return GAMINGGEAR_MACRO_EDITOR_INTERFACE_GET_INTERFACE(self)->empty(self);
}

void gaminggear_macro_editor_interface_add_keystroke(GaminggearMacroEditorInterface *self, guint key, guint action, glong rel_time) {
	GAMINGGEAR_MACRO_EDITOR_INTERFACE_GET_INTERFACE(self)->add_keystroke(self, key, action, rel_time);
}

gboolean gaminggear_macro_editor_interface_get_modified(GaminggearMacroEditorInterface *self) {
	return GAMINGGEAR_MACRO_EDITOR_INTERFACE_GET_INTERFACE(self)->get_modified(self);
}

void gaminggear_macro_editor_interface_set_modified(GaminggearMacroEditorInterface *self, gboolean new_state) {
	GAMINGGEAR_MACRO_EDITOR_INTERFACE_GET_INTERFACE(self)->set_modified(self, new_state);
}

void gaminggear_macro_editor_interface_modified(GaminggearMacroEditorInterface *self) {
	g_signal_emit((gpointer)self, signals[MODIFIED], 0);
}

void gaminggear_macro_editor_interface_empty_changed(GaminggearMacroEditorInterface *self) {
	g_signal_emit((gpointer)self, signals[EMPTY_CHANGED], 0);
}

G_DEFINE_INTERFACE(GaminggearMacroEditorInterface, gaminggear_macro_editor_interface, G_TYPE_OBJECT);

static void gaminggear_macro_editor_interface_default_init(GaminggearMacroEditorInterfaceInterface *iface) {
	signals[MODIFIED] = g_signal_new("modified",
			GAMINGGEAR_MACRO_EDITOR_INTERFACE_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			G_STRUCT_OFFSET(GaminggearMacroEditorInterfaceInterface, modified),
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

	signals[EMPTY_CHANGED] = g_signal_new("empty-changed",
			GAMINGGEAR_MACRO_EDITOR_INTERFACE_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			G_STRUCT_OFFSET(GaminggearMacroEditorInterfaceInterface, empty_changed),
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}
