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

#include "macros_converter_steelseries_engine3.h"
#include "i18n-lib.h"
#include <sqlite3.h>
#include <string.h>
#include <stdlib.h>

typedef enum {
	STEELSERIES_MACRO_EVENT_TYPE_MOUSE = 0,
	STEELSERIES_MACRO_EVENT_TYPE_KEY = 2,
	STEELSERIES_MACRO_EVENT_TYPE_WAIT = 4,
} SteelseriesMacroEventType;

typedef enum {
	STEELSERIES_MACRO_EVENT_EXTRA_DATA_PRESS = 1,
	STEELSERIES_MACRO_EVENT_EXTRA_DATA_RELEASE = 0,
} SteelseriesMacroEventExtraData;

static guint get_uint(GRegex *regex, gchar const *event) {
	GMatchInfo *match_info;
	gchar *string;
	guint value;

	g_regex_match(regex, event, 0, &match_info);

	string = g_match_info_fetch(match_info, 1);
	value = (guint)strtoul(string, NULL, 10);

	g_free(string);
	g_match_info_free(match_info);
	return value;
}

static guint steelseries_mousebutton_to_gaminggear_macro_keystroke_key(guint from) {
	return from - 1 + GAMINGGEAR_MACRO_KEYSTROKE_KEY_BUTTON_LEFT;
}

static GaminggearMacroKeystrokes *steelseries_events_to_gaminggear_macro_keystrokes(gchar const *string) {
	GaminggearMacroKeystrokes *keystrokes;
	gchar **events;
	gchar **event;
	guint index;
	guint type;
	GRegex *type_regex;
	GRegex *code_regex;
	GRegex *extra_regex;

	keystrokes = gaminggear_macro_keystrokes_new();

	type_regex = g_regex_new("\"type\":([^,}]+)", 0, 0, NULL);
	code_regex = g_regex_new("\"code\":([^,}]+)", 0, 0, NULL);
	extra_regex = g_regex_new("\"extraData\":([^,}]+)", 0, 0, NULL);

	events = g_strsplit(string, "},{", 0);

	event = events;
	index = 0;

	while (*event) {
		type = get_uint(type_regex, *event);
		switch (type) {
		case STEELSERIES_MACRO_EVENT_TYPE_MOUSE:
			keystrokes->keystrokes[index].key = steelseries_mousebutton_to_gaminggear_macro_keystroke_key(get_uint(code_regex, *event));
			keystrokes->keystrokes[index].action = (get_uint(extra_regex, *event) == STEELSERIES_MACRO_EVENT_EXTRA_DATA_PRESS ?
					GAMINGGEAR_MACRO_KEYSTROKE_ACTION_PRESS : GAMINGGEAR_MACRO_KEYSTROKE_ACTION_RELEASE);
			++index;
			break;
		case STEELSERIES_MACRO_EVENT_TYPE_KEY:
			keystrokes->keystrokes[index].key = get_uint(code_regex, *event);
			keystrokes->keystrokes[index].action = (get_uint(extra_regex, *event) == STEELSERIES_MACRO_EVENT_EXTRA_DATA_PRESS ?
					GAMINGGEAR_MACRO_KEYSTROKE_ACTION_PRESS : GAMINGGEAR_MACRO_KEYSTROKE_ACTION_RELEASE);
			++index;
			break;
		case STEELSERIES_MACRO_EVENT_TYPE_WAIT:
			if (index == 0)
				g_warning(_("Ignoring wait as first event type"));
			else
				gaminggear_macro_keystroke_set_period(&keystrokes->keystrokes[index - 1], get_uint(extra_regex, *event));
			break;
		default:
			g_warning(_("Ignoring unknown event type %u"), type);
			break;
		}

		++event;
	}
	g_strfreev(events);

	g_regex_unref(type_regex);
	g_regex_unref(code_regex);
	g_regex_unref(extra_regex);

	gaminggear_macro_keystrokes_set_count(keystrokes, index);
	return keystrokes;
}

static int callback(void *arg0, int argc, char **argv, char **col_name) {
	GaminggearMacros *gaminggear_macros;
	GaminggearMacro *gaminggear_macro;
	GaminggearMacroKeystrokes *gaminggear_macro_keystrokes;
	int i;
	char const *name = NULL;
	char const *events = NULL;

	gaminggear_macros = (GaminggearMacros *)arg0;

	for (i = 0; i < argc; ++i) {
		if (strcmp(col_name[i], "name") == 0) {
			name = argv[i];
		} else if (strcmp(col_name[i], "events") == 0) {
			events = argv[i];
		}
	}

	if (!name || !events)
		return 0;

	gaminggear_macro_keystrokes = steelseries_events_to_gaminggear_macro_keystrokes(events);
	gaminggear_macro = gaminggear_macro_new("SteelseriesEngine3", name, gaminggear_macro_keystrokes);
	gaminggear_macros_set(gaminggear_macros, gaminggear_macro);
	gaminggear_macro_free(gaminggear_macro);
	gaminggear_macro_keystrokes_free(gaminggear_macro_keystrokes);

	return 0;
}

GaminggearMacros *macros_conversions_steelseries_engine3_import(gchar const *filename, GError **error) {
	GaminggearMacros *gaminggear_macros;
	int retval;
	sqlite3 *database;
	char *message = NULL;

	gaminggear_macros = gaminggear_macros_new();

	retval = sqlite3_open(filename, &database);
	if (retval) {
		g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_FAILED, _("Sqlite3 could not open database: %s"), filename);
		sqlite3_close(database);
		return NULL;
	}

	retval = sqlite3_exec(database, "SELECT name, events FROM macros;", callback, gaminggear_macros, &message);
	if (retval != SQLITE_OK) {
		g_set_error(error, G_FILE_ERROR_FAILED, G_FILE_ERROR_FAILED, _("Sqlite3 could not evaluate statement: %s"), message);
		sqlite3_free(message);
	}

	sqlite3_close(database);

	return gaminggear_macros;
}

static GaminggearMacrosConverter converter = {
	.import = macros_conversions_steelseries_engine3_import,
	.export = NULL,
	.file_extension = "db",
	.pattern = "*.db",
	.name = N_("SteelSeries Engine 3 database"),
};

GaminggearMacrosConverter const *macros_converter_steelseries_engine3(void) {
	return &converter;
}
