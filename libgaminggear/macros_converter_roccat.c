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

#include "macros_converter_roccat.h"
#include "macros_converter_roccat_helper.h"
#include "gaminggear_helper.h"
#include "i18n-lib.h"
#include <string.h>

#define ROCCAT_ORIGINAL_MACRO_MAIN_GROUP_NAME "SetMacro"
#define ROCCAT_ORIGINAL_MACRO_COUNT_NAME "number"

typedef struct _RoccatOriginalMacroKeystroke RoccatOriginalMacroKeystroke;
typedef struct _RoccatOriginalMacro RoccatOriginalMacro;

enum {
	ROCCAT_ORIGINAL_MACRO_SEQUENCE_KEYSTROKES_NUM = 500,
	ROCCAT_ORIGINAL_MACRO_SEQUENCE_NAME_LENGTH = 52,
};

struct _RoccatOriginalMacroKeystroke {
	guint8 key;
	guint8 action;
	guint16 period;
} __attribute__ ((packed));

typedef enum {
	ROCCAT_ORIGINAL_MACRO_KEYSTROKE_ACTION_PRESS = 1,
	ROCCAT_ORIGINAL_MACRO_KEYSTROKE_ACTION_RELEASE = 2,
} RoccatOriginalMacroKeystrokeAction;

struct _RoccatOriginalMacro {
	guint8 macro_name[ROCCAT_ORIGINAL_MACRO_SEQUENCE_NAME_LENGTH];
	guint32 loop;
	guint32 count;
	RoccatOriginalMacroKeystroke keystrokes[ROCCAT_ORIGINAL_MACRO_SEQUENCE_KEYSTROKES_NUM];
	guint8 checksum;
} __attribute__ ((packed));

static inline guint16 roccat_original_macro_keystroke_get_period(RoccatOriginalMacroKeystroke const *keystroke) {
	return GUINT16_FROM_LE(keystroke->period);
}

static inline void roccat_original_macro_keystroke_set_period(RoccatOriginalMacroKeystroke *keystroke, guint16 new_value) {
	keystroke->period = GUINT16_TO_LE(new_value);
}

static inline guint32 roccat_original_macro_get_count(RoccatOriginalMacro const *macro) {
	return GUINT32_FROM_LE(macro->count);
}

static inline void roccat_original_macro_set_count(RoccatOriginalMacro *macro, guint32 new_value) {
	macro->count = GUINT32_TO_LE(new_value);
}

static inline guint32 roccat_original_macro_get_loop(RoccatOriginalMacro const *macro) {
	return GUINT32_FROM_LE(macro->loop);
}

static inline void roccat_original_macro_set_loop(RoccatOriginalMacro *macro, guint32 new_value) {
	macro->loop = GUINT32_TO_LE(new_value);
}

static guint8 roccat_original_macro_calc_checksum(RoccatOriginalMacro const *macro) {
	return ROCCAT_BYTESUM_PARTIALLY(macro, RoccatOriginalMacro, macro_name[0], checksum);
}

static void roccat_original_macro_set_checksum(RoccatOriginalMacro *macro) {
	macro->checksum = roccat_original_macro_calc_checksum(macro);
}

static void roccat_original_macro_set_macro_name(RoccatOriginalMacro *macro, gchar const *new_name) {
	g_strlcpy((gchar *)(macro->macro_name), new_name, ROCCAT_ORIGINAL_MACRO_SEQUENCE_NAME_LENGTH);
}

static void gaminggear_macro_keystroke_to_roccat_original_macro_keystroke(GaminggearMacroKeystroke const *from, RoccatOriginalMacroKeystroke *to) {
	to->key = from->key;
	roccat_original_macro_keystroke_set_period(to, gaminggear_macro_keystroke_get_period(from));

	if (from->action == GAMINGGEAR_MACRO_KEYSTROKE_ACTION_PRESS)
		to->action = ROCCAT_ORIGINAL_MACRO_KEYSTROKE_ACTION_PRESS;
	else
		to->action = ROCCAT_ORIGINAL_MACRO_KEYSTROKE_ACTION_RELEASE;
}

static RoccatOriginalMacro *gaminggear_macro_to_roccat_original_macro(GaminggearMacro const *gaminggear_macro) {
	RoccatOriginalMacro *result;
	guint i, count;

	count = gaminggear_macro_keystrokes_get_count(&gaminggear_macro->keystrokes);
	if (count > ROCCAT_ORIGINAL_MACRO_SEQUENCE_KEYSTROKES_NUM) {
		g_warning(_("Macro %s/%s is too long to convert"), gaminggear_macro->macroset, gaminggear_macro->macro);
		return NULL;
	}

	result = g_malloc0(sizeof(RoccatOriginalMacro));
	roccat_original_macro_set_macro_name(result, gaminggear_macro->macro);
	roccat_original_macro_set_loop(result, gaminggear_macro->keystrokes.loop);
	roccat_original_macro_set_count(result, count);

	for (i = 0; i < count; ++i)
		gaminggear_macro_keystroke_to_roccat_original_macro_keystroke(&gaminggear_macro->keystrokes.keystrokes[i], &result->keystrokes[i]);

	roccat_original_macro_set_checksum(result);

	return result;
}

gboolean macros_conversions_roccat_export(gchar const *filename, GaminggearMacros *gaminggear_macros, GError **error) {
	GKeyFile *roccat_orig_macros = g_key_file_new();
	GaminggearMacro *gaminggear_macro;
	RoccatOriginalMacro *roccat_original_macro;
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
		g_key_file_set_string(roccat_orig_macros, ROCCAT_ORIGINAL_MACRO_MAIN_GROUP_NAME, intkey, *group);
		g_free(intkey);

		for (key = keys; *key; ++key) {
			gaminggear_macro = gaminggear_macros_get(gaminggear_macros, *group, *key, error);
			if (!gaminggear_macro)
				goto free_keys;

			roccat_original_macro = gaminggear_macro_to_roccat_original_macro(gaminggear_macro);
			gaminggear_macro_free(gaminggear_macro);
			if (!roccat_original_macro)
				break;

			intkey = g_strdup_printf("%i", macro_counter);
			g_gaminggear_key_file_set_binary(roccat_orig_macros, *group, intkey, roccat_original_macro, sizeof(RoccatOriginalMacro));
			g_free(intkey);
			g_free(roccat_original_macro);
			++macro_counter;
		}
		g_key_file_set_integer(roccat_orig_macros, *group, ROCCAT_ORIGINAL_MACRO_COUNT_NAME, macro_counter);
		g_strfreev(keys);
		++macroset_counter;
		macro_counter = 0;
	}
	g_strfreev(groups);
	g_key_file_set_integer(roccat_orig_macros, ROCCAT_ORIGINAL_MACRO_MAIN_GROUP_NAME, ROCCAT_ORIGINAL_MACRO_COUNT_NAME, macroset_counter);

	data = g_key_file_to_data(roccat_orig_macros, &length, error);
	g_key_file_free(roccat_orig_macros);
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
	g_key_file_free(roccat_orig_macros);
exit:
	return FALSE;
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

static GaminggearMacroKeystrokes *roccat_original_macro_to_gaminggear_macro_keystrokes(RoccatOriginalMacro const *roccat_original_macro) {
	GaminggearMacroKeystrokes *result = g_malloc0(sizeof(GaminggearMacroKeystrokes));
	guint32 count;
	guint i;

	count = roccat_original_macro_get_count(roccat_original_macro);
	g_assert(count <= GAMINGGEAR_MACRO_KEYSTROKES_NUM);
	gaminggear_macro_keystrokes_set_count(result, count);
	result->loop = roccat_original_macro_get_loop(roccat_original_macro);

	for (i = 0; i < count; ++i) {
		result->keystrokes[i].key = roccat_original_macro->keystrokes[i].key;
		switch (roccat_original_macro->keystrokes[i].action) {
		case ROCCAT_ORIGINAL_MACRO_KEYSTROKE_ACTION_PRESS:
			result->keystrokes[i].action = GAMINGGEAR_MACRO_KEYSTROKE_ACTION_PRESS;
			break;
		case ROCCAT_ORIGINAL_MACRO_KEYSTROKE_ACTION_RELEASE:
			result->keystrokes[i].action = GAMINGGEAR_MACRO_KEYSTROKE_ACTION_RELEASE;
			break;
		}
		gaminggear_macro_keystroke_set_period(&result->keystrokes[i], roccat_original_macro_keystroke_get_period(&roccat_original_macro->keystrokes[i]));
	}

	return result;
}

static gboolean eval_roccat_original_key(GaminggearMacros *gaminggear_macros, GKeyFile *roccat_original_macro_file, gchar const *group_name, gchar const *key_name, GError **error) {
	RoccatOriginalMacro *roccat_original_macro;
	GaminggearMacroKeystrokes *gaminggear_macro_keystrokes;
	GaminggearMacro *gaminggear_macro;

	if (strcmp(key_name, ROCCAT_ORIGINAL_MACRO_COUNT_NAME)) {
		roccat_original_macro = g_gaminggear_key_file_get_binary(roccat_original_macro_file, group_name, key_name, sizeof(RoccatOriginalMacro), error);
		if (!roccat_original_macro)
			return FALSE;

		gaminggear_macro_keystrokes = roccat_original_macro_to_gaminggear_macro_keystrokes(roccat_original_macro);
		gaminggear_macro = gaminggear_macro_new(group_name, (gchar const *)roccat_original_macro->macro_name, gaminggear_macro_keystrokes);
		gaminggear_macros_set(gaminggear_macros, gaminggear_macro);

		gaminggear_macro_free(gaminggear_macro);
		gaminggear_macro_keystrokes_free(gaminggear_macro_keystrokes);
		g_free(roccat_original_macro);
	}
	return TRUE;
}

static void eval_roccat_original_group(GaminggearMacros *gaminggear_macros, GKeyFile *roccat_original_macro_file, gchar const *group_name) {
	if (strcmp(group_name, ROCCAT_ORIGINAL_MACRO_MAIN_GROUP_NAME))
		gaminggear_macro_import_iterate_keys(gaminggear_macros, roccat_original_macro_file, group_name, eval_roccat_original_key);
}

static gboolean is_roccat_original_file(GKeyFile *key_file) {
	if (g_key_file_has_group(key_file, ROCCAT_ORIGINAL_MACRO_MAIN_GROUP_NAME))
		return TRUE;
	return FALSE;
}

GaminggearMacros *macros_conversions_roccat_import(gchar const *filename, GError **error) {
	GaminggearMacros *gaminggear_macros;
	GKeyFile *roccat_orig_macros;

	roccat_orig_macros = g_key_file_new();
	gaminggear_macros = gaminggear_macros_new();

	if (!g_key_file_load_from_file(roccat_orig_macros, filename, G_KEY_FILE_NONE, error))
		goto free;

	if (!is_roccat_original_file(roccat_orig_macros)) {
		g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_INVAL, _("%1$s is no %2$s macro file"), filename, "Roccat");
		goto free;
	}

	gaminggear_macro_import_iterate_groups(gaminggear_macros, roccat_orig_macros, eval_roccat_original_group);

	g_key_file_free(roccat_orig_macros);

	return gaminggear_macros;
free:
	g_key_file_free(roccat_orig_macros);
	gaminggear_macros_free(gaminggear_macros);
	return NULL;
}

static GaminggearMacrosConverter converter = {
	.import = macros_conversions_roccat_import,
	.export = macros_conversions_roccat_export,
	.file_extension = "dat",
	.pattern = "*.dat",
	.name = N_("Roccat macro files"),
};

GaminggearMacrosConverter const *macros_converter_roccat(void) {
	return &converter;
}
