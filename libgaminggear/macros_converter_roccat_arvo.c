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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libgaminggear. If not, see <http://www.gnu.org/licenses/>.
 */

#include "macros_converter_roccat_arvo.h"
#include "macros_converter_roccat_helper.h"
#include "gaminggear_helper.h"
#include "i18n-lib.h"
#include <string.h>

typedef struct _ArvoOriginalMacroKeystroke ArvoOriginalMacroKeystroke;
typedef struct _ArvoOriginalMacro ArvoOriginalMacro;

#define ARVO_ORIGINAL_MACRO_MAIN_GROUP_NAME "Main"
#define ARVO_ORIGINAL_MACRO_COUNT_NAME "Count"

enum {
	ARVO_ORIGINAL_MACRO_KEYSTROKES_NUM = 512,
	ARVO_ORIGINAL_MACRO_NAME_LENGTH = 52,
};

struct _ArvoOriginalMacroKeystroke {
	guint8 key;
	guint8 action;
	guint16 period;
} __attribute__ ((packed));

typedef enum {
	ARVO_ORIGINAL_MACRO_KEYSTROKE_ACTION_PRESS = 0,
	ARVO_ORIGINAL_MACRO_KEYSTROKE_ACTION_RELEASE = 1,
} ArvoOriginalMacroKeystrokeAction;

struct _ArvoOriginalMacro {
	guint8 macro_name[ARVO_ORIGINAL_MACRO_NAME_LENGTH];
	guint32 unused;
	guint32 count;
	ArvoOriginalMacroKeystroke keystrokes[ARVO_ORIGINAL_MACRO_KEYSTROKES_NUM];
	guint8 checksum;
} __attribute__ ((packed));

static inline guint16 arvo_original_macro_keystroke_get_period(ArvoOriginalMacroKeystroke const *keystroke) {
	return GUINT16_FROM_LE(keystroke->period);
}

static inline void arvo_original_macro_keystroke_set_period(ArvoOriginalMacroKeystroke *keystroke, guint16 new_value) {
	keystroke->period = GUINT16_TO_LE(new_value);
}

static inline guint32 arvo_original_macro_get_count(ArvoOriginalMacro const *macro) {
	return GUINT32_FROM_LE(macro->count);
}

static inline void arvo_original_macro_set_count(ArvoOriginalMacro *macro, guint32 new_value) {
	macro->count = GUINT32_TO_LE(new_value);
}

static GaminggearMacroKeystrokes *arvo_original_macro_to_gaminggear_macro_keystrokes(ArvoOriginalMacro const *arvo_original_macro) {
	GaminggearMacroKeystrokes *result = g_malloc0(sizeof(GaminggearMacroKeystrokes));
	guint32 count;
	guint i;

	count = arvo_original_macro_get_count(arvo_original_macro);
	g_assert(count <= GAMINGGEAR_MACRO_KEYSTROKES_NUM);
	gaminggear_macro_keystrokes_set_count(result, count);

	for (i = 0; i < count; ++i) {
		result->keystrokes[i].key = arvo_original_macro->keystrokes[i].key;
		switch (arvo_original_macro->keystrokes[i].action) {
		case ARVO_ORIGINAL_MACRO_KEYSTROKE_ACTION_PRESS:
			result->keystrokes[i].action = GAMINGGEAR_MACRO_KEYSTROKE_ACTION_PRESS;
			break;
		case ARVO_ORIGINAL_MACRO_KEYSTROKE_ACTION_RELEASE:
			result->keystrokes[i].action = GAMINGGEAR_MACRO_KEYSTROKE_ACTION_RELEASE;
			break;
		}
		gaminggear_macro_keystroke_set_period(&result->keystrokes[i], arvo_original_macro_keystroke_get_period(&arvo_original_macro->keystrokes[i]));
	}

	return result;
}

static void gaminggear_macro_import_iterate_keys(GaminggearMacros *gaminggear_macros,
		GKeyFile *other_macro_file,
		gchar const *group_name,
		gboolean (*eval_key)(GaminggearMacros *, GKeyFile *, gchar const *, gchar const *, GError **)) {
	gchar **key_names, **key_name;
	GError *error = NULL;

	key_names = g_key_file_get_keys(other_macro_file, group_name, NULL, &error);
	if (!key_names)
		g_error(_("Could not find group %s: %s"), group_name, error->message);

	key_name = key_names;
	while (*key_name) {
		if (!eval_key(gaminggear_macros, other_macro_file, group_name, *key_name, &error)) {
			g_critical(_("Could not find key %s/%s: %s"), group_name, *key_name, error->message);
			g_clear_error(&error);
		}
		++key_name;
	}
	g_strfreev(key_names);
}

static void gaminggear_macro_import_iterate_groups(GaminggearMacros *gaminggear_macros,
		GKeyFile *other_macro_file,
		void (*eval_group)(GaminggearMacros *, GKeyFile *, gchar const *)) {
	gchar **groups, **group;
	groups = g_key_file_get_groups(other_macro_file, NULL);
	group = groups;
	while (*group) {
		eval_group(gaminggear_macros, other_macro_file, *group);
		++group;
	}
	g_strfreev(groups);
}

static gboolean eval_arvo_key(GaminggearMacros *gaminggear_macros, GKeyFile *arvo_original_macro_file, gchar const *group_name, gchar const *key_name, GError **error) {
	ArvoOriginalMacro *arvo_original_macro;
	GaminggearMacroKeystrokes *gaminggear_macro_keystrokes;
	GaminggearMacro *gaminggear_macro;

	if (strcmp(key_name, ARVO_ORIGINAL_MACRO_COUNT_NAME)) {
		arvo_original_macro = g_gaminggear_key_file_get_binary(arvo_original_macro_file, group_name, key_name, sizeof(ArvoOriginalMacro), error);
		if (!arvo_original_macro)
			return FALSE;

		gaminggear_macro_keystrokes = arvo_original_macro_to_gaminggear_macro_keystrokes(arvo_original_macro);
		gaminggear_macro = gaminggear_macro_new(group_name, (gchar const *)arvo_original_macro->macro_name, gaminggear_macro_keystrokes);
		gaminggear_macros_set(gaminggear_macros, gaminggear_macro);

		gaminggear_macro_free(gaminggear_macro);
		gaminggear_macro_keystrokes_free(gaminggear_macro_keystrokes);
		g_free(arvo_original_macro);
	}
	return TRUE;
}

static void eval_arvo_group(GaminggearMacros *gaminggear_macros, GKeyFile *arvo_original_macro_file, gchar const *group_name) {
	if (strcmp(group_name, ARVO_ORIGINAL_MACRO_MAIN_GROUP_NAME))
		gaminggear_macro_import_iterate_keys(gaminggear_macros, arvo_original_macro_file, group_name, eval_arvo_key);
}

static gboolean is_arvo_file(GKeyFile *key_file) {
	if (g_key_file_has_group(key_file, ARVO_ORIGINAL_MACRO_MAIN_GROUP_NAME))
		return TRUE;
	return FALSE;
}

static guint8 arvo_original_macro_calc_checksum(ArvoOriginalMacro const *macro) {
	return ROCCAT_BYTESUM_PARTIALLY(macro, ArvoOriginalMacro, macro_name[0], checksum);
}

static void arvo_original_macro_set_checksum(ArvoOriginalMacro *macro) {
	macro->checksum = arvo_original_macro_calc_checksum(macro);
}

static void arvo_original_macro_set_macro_name(ArvoOriginalMacro *macro, gchar const *new_name) {
	g_strlcpy((gchar *)(macro->macro_name), new_name, ARVO_ORIGINAL_MACRO_NAME_LENGTH);
}

static void gaminggear_macro_keystroke_to_arvo_original_macro_keystroke(GaminggearMacroKeystroke const *from, ArvoOriginalMacroKeystroke *to) {
	to->key = from->key;
	arvo_original_macro_keystroke_set_period(to, gaminggear_macro_keystroke_get_period(from));
	if (from->action == GAMINGGEAR_MACRO_KEYSTROKE_ACTION_PRESS)
		to->action = ARVO_ORIGINAL_MACRO_KEYSTROKE_ACTION_PRESS;
	else
		to->action = ARVO_ORIGINAL_MACRO_KEYSTROKE_ACTION_RELEASE;
}

static ArvoOriginalMacro *gaminggear_macro_to_arvo_original_macro(GaminggearMacro const *gaminggear_macro) {
	ArvoOriginalMacro *result;
	guint i, count;

	count = gaminggear_macro_keystrokes_get_count(&gaminggear_macro->keystrokes);
	if (count > ARVO_ORIGINAL_MACRO_KEYSTROKES_NUM) {
		g_warning(_("Macro %s/%s is too long to convert"), gaminggear_macro->macroset, gaminggear_macro->macro);
		return NULL;
	}

	result = g_malloc0(sizeof(ArvoOriginalMacro));
	arvo_original_macro_set_macro_name(result, gaminggear_macro->macro);
	arvo_original_macro_set_count(result, count);

	for (i = 0; i < count; ++i) {
		gaminggear_macro_keystroke_to_arvo_original_macro_keystroke(&gaminggear_macro->keystrokes.keystrokes[i], &result->keystrokes[i]);
	}

	arvo_original_macro_set_checksum(result);

	return result;
}

GaminggearMacros *macros_conversions_roccat_arvo_import(gchar const *filename, GError **error) {
	GaminggearMacros *gaminggear_macros;
	GKeyFile *arvo_original_macro_file;

	arvo_original_macro_file = g_key_file_new();
	gaminggear_macros = gaminggear_macros_new();

	if (!g_key_file_load_from_file(arvo_original_macro_file, filename, G_KEY_FILE_NONE, error))
		goto free;

	if (!is_arvo_file(arvo_original_macro_file)) {
		g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_INVAL, _("%1$s is no %2$s macro file"), filename, "Roccat Arvo");
		goto free;
	}

	gaminggear_macro_import_iterate_groups(gaminggear_macros, arvo_original_macro_file, eval_arvo_group);

	g_key_file_free(arvo_original_macro_file);

	return gaminggear_macros;
free:
	g_key_file_free(arvo_original_macro_file);
	gaminggear_macros_free(gaminggear_macros);
	return NULL;
}

gboolean macros_conversions_roccat_arvo_export(gchar const *filename, GaminggearMacros *gaminggear_macros, GError **error) {
	GKeyFile *arvo_original_macro_file = g_key_file_new();
	GaminggearMacro *gaminggear_macro;
	ArvoOriginalMacro *arvo_original_macro;
	gchar *data, **groups, **group, **keys, **key;
	gsize length;
	guint macroset_counter = 0;
	guint macro_counter = 0;
	gchar *intkey;

	groups = gaminggear_macros_get_macrosets(gaminggear_macros, NULL);
	for (group = groups; *group; ++group) {
		keys = gaminggear_macros_get_macros(gaminggear_macros, *group, NULL, error);
		if (*error)
			goto free_groups;

		intkey = g_strdup_printf("%i", macroset_counter);
		g_key_file_set_string(arvo_original_macro_file, ARVO_ORIGINAL_MACRO_MAIN_GROUP_NAME, intkey, *group);
		g_free(intkey);

		for (key = keys; *key; ++key) {
			gaminggear_macro = gaminggear_macros_get(gaminggear_macros, *group, *key, error);
			if (!gaminggear_macro)
				goto free_keys;

			arvo_original_macro = gaminggear_macro_to_arvo_original_macro(gaminggear_macro);
			gaminggear_macro_free(gaminggear_macro);
			if (!arvo_original_macro)
				break;

			intkey = g_strdup_printf("%i", macro_counter);
			g_gaminggear_key_file_set_binary(arvo_original_macro_file, *group, intkey, arvo_original_macro, sizeof(ArvoOriginalMacro));
			g_free(intkey);
			g_free(arvo_original_macro);
			++macro_counter;
		}
		g_key_file_set_integer(arvo_original_macro_file, *group, ARVO_ORIGINAL_MACRO_COUNT_NAME, macro_counter);
		g_strfreev(keys);
		++macroset_counter;
		macro_counter = 0;
	}
	g_strfreev(groups);
	g_key_file_set_integer(arvo_original_macro_file, ARVO_ORIGINAL_MACRO_MAIN_GROUP_NAME, ARVO_ORIGINAL_MACRO_COUNT_NAME, macroset_counter);

	data = g_key_file_to_data(arvo_original_macro_file, &length, error);
	g_key_file_free(arvo_original_macro_file);
	if (*error)
		goto exit;

	g_file_set_contents(filename, data, length, error);
	g_free(data);
	if (*error)
		goto exit;

	return TRUE;
free_keys:
	g_strfreev(keys);
free_groups:
	g_strfreev(groups);
	g_key_file_free(arvo_original_macro_file);
exit:
	return FALSE;
}

static GaminggearMacrosConverter converter = {
	.import = macros_conversions_roccat_arvo_import,
	.export = macros_conversions_roccat_arvo_export,
	.file_extension = "ini",
	.pattern = "*.ini",
	.name = N_("Roccat Arvo macro files"),
};

GaminggearMacrosConverter const *macros_converter_roccat_arvo(void) {
	return &converter;
}
