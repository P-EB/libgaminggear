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
#include "gaminggear_helper.h"
#include <string.h>

typedef struct _GaminggearMacroKeystrokesV1 GaminggearMacroKeystrokesV1;

struct _GaminggearMacroKeystrokesV1 {
	guint16 count;
	GaminggearMacroKeystroke keystrokes[GAMINGGEAR_MACRO_KEYSTROKES_NUM];
};

static GaminggearMacroKeystrokes *gaminggear_macro_keystrokes_v1_to_gaminggear_macro_keystrokes(GaminggearMacroKeystrokesV1 const *v1) {
	GaminggearMacroKeystrokes *result;
	guint i;

	result = gaminggear_macro_keystrokes_new();
	result->count = v1->count;

	for (i = 0; i < GAMINGGEAR_MACRO_KEYSTROKES_NUM; ++i) {
		result->keystrokes[i].key = v1->keystrokes[i].key;
		result->keystrokes[i].action = v1->keystrokes[i].action;
		result->keystrokes[i].period = v1->keystrokes[i].period;
	}
	result->loop = 1;

	return result;
}

GaminggearMacroKeystrokes *gaminggear_key_file_get_macro_keystrokes(GKeyFile *key_file,
		gchar const *macroset_name, gchar const *macro_name,
		GError **error) {
	GaminggearMacroKeystrokesV1 *gaminggear_macro_keystrokes_v1;
	GaminggearMacroKeystrokes *gaminggear_macro_keystrokes;

	gaminggear_macro_keystrokes = g_gaminggear_key_file_get_binary(key_file, macroset_name, macro_name, sizeof(GaminggearMacroKeystrokes), error);
	if (gaminggear_macro_keystrokes)
		return gaminggear_macro_keystrokes;

	if (!g_error_matches(*error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_INVALID_VALUE))
		return NULL;
	g_clear_error(error);

	gaminggear_macro_keystrokes_v1 = g_gaminggear_key_file_get_binary(key_file, macroset_name, macro_name, sizeof(GaminggearMacroKeystrokesV1), error);
	if (gaminggear_macro_keystrokes_v1) {
		gaminggear_macro_keystrokes = gaminggear_macro_keystrokes_v1_to_gaminggear_macro_keystrokes(gaminggear_macro_keystrokes_v1);
		g_free(gaminggear_macro_keystrokes_v1);
		return gaminggear_macro_keystrokes;
	}

	return NULL;
}

gboolean gaminggear_macro_keystrokes_add(GaminggearMacroKeystrokes *keystrokes, GaminggearMacroKeystroke const *keystroke) {
	guint count = gaminggear_macro_keystrokes_get_count(keystrokes);
	/* index = count - 1; new_index = index + 1 */
	guint new_index = count;

	g_assert(keystrokes->count <= GAMINGGEAR_MACRO_KEYSTROKES_NUM);
	g_assert(keystroke->action == GAMINGGEAR_MACRO_KEYSTROKE_ACTION_PRESS || keystroke->action == GAMINGGEAR_MACRO_KEYSTROKE_ACTION_RELEASE);

	if (new_index >= GAMINGGEAR_MACRO_KEYSTROKES_NUM)
		return FALSE;

	gaminggear_macro_keystrokes_set_count(keystrokes, count + 1);
	keystrokes->keystrokes[new_index].action = keystroke->action;
	keystrokes->keystrokes[new_index].key = keystroke->key;
	/* no change in endianess and size */
	keystrokes->keystrokes[new_index].period = keystroke->period;

	return TRUE;
}

void gaminggear_macro_keystrokes_init(GaminggearMacroKeystrokes *keystrokes) {
	memset(keystrokes, 0, sizeof(GaminggearMacroKeystrokes));
	keystrokes->loop = 1;
}

GaminggearMacroKeystrokes *gaminggear_macro_keystrokes_new(void) {
	GaminggearMacroKeystrokes *keystrokes;
	keystrokes = g_malloc0(sizeof(GaminggearMacroKeystrokes));
	gaminggear_macro_keystrokes_init(keystrokes);
	return keystrokes;
}

void gaminggear_macro_keystrokes_free(GaminggearMacroKeystrokes *keystrokes) {
	g_free(keystrokes);
}

void gaminggear_macro_keystrokes_copy(GaminggearMacroKeystrokes const *from, GaminggearMacroKeystrokes *to) {
	memcpy(to, from, sizeof(GaminggearMacroKeystrokes));
}

GaminggearMacroKeystrokes *gaminggear_macro_keystrokes_dup(GaminggearMacroKeystrokes const *old) {
	return g_memdup(old, sizeof(GaminggearMacroKeystrokes));
}

gboolean gaminggear_macro_keystrokes_have_mouse_keystroke(GaminggearMacroKeystrokes const *keystrokes) {
	guint count = gaminggear_macro_keystrokes_get_count(keystrokes);
	guint i;

	for (i = 0; i < count; ++i) {
		if (keystrokes->keystrokes[i].key >= GAMINGGEAR_MACRO_KEYSTROKE_KEY_BUTTON_LEFT &&
				keystrokes->keystrokes[i].key <= GAMINGGEAR_MACRO_KEYSTROKE_KEY_BUTTON_MIDDLE)
			return TRUE;
	}

	return FALSE;
}
