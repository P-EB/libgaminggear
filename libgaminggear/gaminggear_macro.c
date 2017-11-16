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

void gaminggear_macro_set_macroset_name(GaminggearMacro *macro, gchar const *new_name) {
	g_free(macro->macroset);
	macro->macroset = g_strdup(new_name);
}

void gaminggear_macro_set_macro_name(GaminggearMacro *macro, gchar const *new_name) {
	g_free(macro->macro);
	macro->macro = g_strdup(new_name);
}

gchar *gaminggear_macro_get_name_joined(GaminggearMacro const *macro) {
	return g_strjoin("/", macro->macroset, macro->macro, NULL);
}

GaminggearMacroKeystrokes *gaminggear_macro_get_keystrokes(GaminggearMacro *macro) {
	return &macro->keystrokes;
}

gboolean gaminggear_macro_add_keystroke(GaminggearMacro *macro, GaminggearMacroKeystroke const *keystroke) {
	return gaminggear_macro_keystrokes_add(&macro->keystrokes, keystroke);
}

GaminggearMacro *gaminggear_macro_dup(GaminggearMacro const *gaminggear_macro) {
	return gaminggear_macro_new(gaminggear_macro->macroset, gaminggear_macro->macro, &gaminggear_macro->keystrokes);
}

GaminggearMacro *gaminggear_macro_new(gchar const *macroset_name, gchar const *macro_name, GaminggearMacroKeystrokes const *keystrokes) {
	GaminggearMacro *macro;
	macro = g_malloc0(sizeof(GaminggearMacro));
	gaminggear_macro_set_macroset_name(macro, macroset_name);
	gaminggear_macro_set_macro_name(macro, macro_name);

	if (keystrokes)
		gaminggear_macro_keystrokes_copy(keystrokes, &macro->keystrokes);
	else
		gaminggear_macro_keystrokes_init(&macro->keystrokes);

	return macro;
}

void gaminggear_macro_free(GaminggearMacro *macro) {
	if (macro == NULL)
		return;
	g_free(macro->macroset);
	g_free(macro->macro);
	g_free(macro);
}

gboolean gaminggear_macro_has_mouse_keystroke(GaminggearMacro const *gaminggear_macro) {
	return gaminggear_macro_keystrokes_have_mouse_keystroke(&gaminggear_macro->keystrokes);
}
