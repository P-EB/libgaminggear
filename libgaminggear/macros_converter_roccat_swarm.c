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

#include "macros_converter_roccat_swarm.h"
#include "i18n-lib.h"
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

enum {
	SWARM_MACRO_NAME_LENGTH = 32,
	SWARM_MACRO_KEYSTROKES_NUM = 480,
};

typedef struct {
	guint8 key;
	guint8 action;
	guint16 period;
} __attribute__ ((packed)) SwarmKeystroke;

enum {
	SWARM_KEYSTROKE_ACTION_PRESS = 1,
	SWARM_KEYSTROKE_ACTION_RELEASE = 2,
};

static inline guint16 swarm_keystroke_get_period(SwarmKeystroke const *keystroke) {
	return GUINT16_FROM_LE(keystroke->period);
}

static inline void swarm_keystroke_set_period(SwarmKeystroke *keystroke, guint16 new_period) {
	keystroke->period = GUINT16_TO_LE(new_period);
}

typedef struct {
	guint8 loop;
	guint8 name[SWARM_MACRO_NAME_LENGTH];
	guint16 count;
	SwarmKeystroke keystrokes[SWARM_MACRO_KEYSTROKES_NUM];
} __attribute__ ((packed)) SwarmMacro;

static inline guint16 swarm_macro_get_count(SwarmMacro const *macro) {
	return GUINT16_FROM_LE(macro->count);
}

static inline void swarm_macro_set_count(SwarmMacro *macro, guint16 new_count) {
	macro->count = GUINT16_TO_LE(new_count);
}

static void swarm_macro_set_name(SwarmMacro *macro, gchar const *new_name) {
	g_strlcpy((gchar *)(macro->name), new_name, SWARM_MACRO_NAME_LENGTH);
}

static gboolean myread(int fd, void *buffer, size_t nbytes, GError **error) {
	int result;

	result = read(fd, buffer, nbytes);
	if (result == 0) {
		g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_IO, _("Trying to read behind file boundaries"));
		return FALSE;
	} else if (result < 0) {
		g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errno), _("Could not read: %s"), g_strerror(errno));
		return FALSE;
	}
	return TRUE;
}

static gboolean read32be(int fd, guint32 *value, GError **error) {
	guint32 buffer;
	int result;

	result = myread(fd, &buffer, sizeof(guint32), error);
	if (!result)
		return FALSE;

	*value = GUINT32_FROM_BE(buffer);

	return TRUE;
}

static gboolean mywrite(int fd, void const *buffer, size_t nbytes, GError **error) {
	if (write(fd, buffer, nbytes) < 0) {
		g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errno), _("Could not write: %s"), g_strerror(errno));
		return FALSE;
	}
	return TRUE;
}

static gboolean write32be(int fd, guint32 value, GError **error) {
	guint32 buffer;
	buffer = GUINT32_TO_BE(value);

	return mywrite(fd, &buffer, sizeof(guint32), error);
}

static void utf16_from_be(gunichar2 *data, guint length) {
	guint i;

	for (i = 0; i < length; ++i)
		data[i] = GUINT16_FROM_BE(data[i]);
}

static void utf16_to_be(gunichar2 *data, guint length) {
	guint i;
	for (i = 0; i < length; ++i)
		data[i] = GUINT16_TO_BE(data[i]);
}

static GaminggearMacroKeystrokes *swarm_macro_to_gaminggear_macro_keystrokes(SwarmMacro const *swarm_macro) {
	GaminggearMacroKeystrokes *result;
	guint count;
	guint i;

	count = MIN(swarm_macro_get_count(swarm_macro), GAMINGGEAR_MACRO_KEYSTROKES_NUM);
	result = gaminggear_macro_keystrokes_new();

	gaminggear_macro_keystrokes_set_count(result, count);
	result->loop = swarm_macro->loop;

	for (i = 0; i < count; ++i) {
		result->keystrokes[i].key = swarm_macro->keystrokes[i].key;
		if (swarm_macro->keystrokes[i].action == SWARM_KEYSTROKE_ACTION_PRESS)
			result->keystrokes[i].action = GAMINGGEAR_MACRO_KEYSTROKE_ACTION_PRESS;
		else
			result->keystrokes[i].action = GAMINGGEAR_MACRO_KEYSTROKE_ACTION_RELEASE;
		gaminggear_macro_keystroke_set_period(&result->keystrokes[i], swarm_keystroke_get_period(&swarm_macro->keystrokes[i]));
	}

	return result;
}

static SwarmMacro *gaminggear_macro_to_swarm_macro(GaminggearMacro const *gaminggear_macro) {
	SwarmMacro *result;
	guint count;
	guint i;

	count = MIN(gaminggear_macro_keystrokes_get_count(&gaminggear_macro->keystrokes), SWARM_MACRO_KEYSTROKES_NUM);
	result = (SwarmMacro *)g_malloc0(sizeof(SwarmMacro));

	swarm_macro_set_count(result, count);
	swarm_macro_set_name(result, gaminggear_macro->macro);
	result->loop = gaminggear_macro->keystrokes.loop;

	for (i = 0; i < count; ++i) {
		result->keystrokes[i].key = gaminggear_macro->keystrokes.keystrokes[i].key;
		if (gaminggear_macro->keystrokes.keystrokes[i].action == GAMINGGEAR_MACRO_KEYSTROKE_ACTION_PRESS)
			result->keystrokes[i].action = SWARM_KEYSTROKE_ACTION_PRESS;
		else
			result->keystrokes[i].action = SWARM_KEYSTROKE_ACTION_RELEASE;
		swarm_keystroke_set_period(&result->keystrokes[i], gaminggear_macro_keystroke_get_period(&gaminggear_macro->keystrokes.keystrokes[i]));
	}

	return result;
}

gboolean macros_conversions_roccat_swarm_export(gchar const *filename, GaminggearMacros *gaminggear_macros, GError **error) {
	GaminggearMacro *gaminggear_macro;
	SwarmMacro *swarm_macro;
	gchar **macrosets, **macroset, **macros, **macro;
	gsize macroset_count, macro_count;
	gunichar2 *macroset_name;
	glong size;
	int fd;

	fd = open(filename, O_WRONLY | O_CREAT, 0666);
	if (fd < 0) {
		g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errno), _("Could not open %2$s macro file %1$s: %3$s"), filename, "Roccat Swarm", g_strerror(errno));
		return FALSE;
	}

	macrosets = gaminggear_macros_get_macrosets(gaminggear_macros, &macroset_count);

	if (!write32be(fd, macroset_count, error))
		goto free_macrosets;

	for (macroset = macrosets; *macroset; ++macroset) {
		macroset_name = g_utf8_to_utf16(*macroset, -1, NULL, &size, NULL);
		utf16_to_be(macroset_name, size);
		if (!write32be(fd, size * 2, error))
			goto free_macrosets;
		if (!mywrite(fd, macroset_name, size * 2, error))
			goto free_macrosets;

		g_free(macroset_name);

		macros = gaminggear_macros_get_macros(gaminggear_macros, *macroset, &macro_count, error);
		if (*error)
			goto free_macrosets;

		if (!write32be(fd, macro_count, error))
			goto free_macros;

		for (macro = macros; *macro; ++macro) {
			gaminggear_macro = gaminggear_macros_get(gaminggear_macros, *macroset, *macro, error);
			if (!gaminggear_macro)
				goto free_macros;

			swarm_macro = gaminggear_macro_to_swarm_macro(gaminggear_macro);
			gaminggear_macro_free(gaminggear_macro);

			if (!write32be(fd, sizeof(SwarmMacro), error))
				goto free_macros;

			if (!mywrite(fd, swarm_macro, sizeof(SwarmMacro), error))
				goto free_macros;

			g_free(swarm_macro);
		}
		g_strfreev(macros);
	}
	g_strfreev(macrosets);

	close(fd);
	return TRUE;
free_macros:
	g_strfreev(macros);
free_macrosets:
	g_strfreev(macrosets);
	close(fd);
	return FALSE;
}

GaminggearMacros *macros_conversions_roccat_swarm_import(gchar const *filename, GError **error) {
	GaminggearMacros *gaminggear_macros;
	GaminggearMacroKeystrokes *gaminggear_macro_keystrokes;
	GaminggearMacro *gaminggear_macro;
	guint32 macroset_count, macroset_index;
	guint32 macro_count, macro_index;
	guint32 size;
	SwarmMacro swarm_macro;
	int fd;
	gunichar2 *raw_macroset_name;
	gchar *macroset_name;

	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errno), _("Could not open %2$s macro file %1$s: %3$s"), filename, "Roccat Swarm", g_strerror(errno));
		return FALSE;
	}

	gaminggear_macros = gaminggear_macros_new();

	if (!read32be(fd, &macroset_count, error)) goto exit;
	for (macroset_index = 0; macroset_index < macroset_count; ++macroset_index) {
		if (!read32be(fd, &size, error)) goto exit;
		raw_macroset_name = (gunichar2 *)g_malloc(size);
		if (!myread(fd, raw_macroset_name, size, error)) goto exit;
		utf16_from_be(raw_macroset_name, size / 2);
		macroset_name = g_utf16_to_utf8(raw_macroset_name, size / 2, NULL, NULL, NULL);
		g_free(raw_macroset_name);

		if (!read32be(fd, &macro_count, error)) goto exit;
		for (macro_index = 0; macro_index < macro_count; ++macro_index) {
			if (!read32be(fd, &size, error)) goto exit;
			if (size != sizeof(SwarmMacro)) {
				g_warning("%u != %zu", size, sizeof(SwarmMacro));
				// FIXME consequence
			}
			if (!myread(fd, &swarm_macro, size, error)) goto exit;

			gaminggear_macro_keystrokes = swarm_macro_to_gaminggear_macro_keystrokes(&swarm_macro);
			gaminggear_macro = gaminggear_macro_new(macroset_name, (gchar const *)swarm_macro.name, gaminggear_macro_keystrokes);
			gaminggear_macros_set(gaminggear_macros, gaminggear_macro);
			gaminggear_macro_free(gaminggear_macro);
			gaminggear_macro_keystrokes_free(gaminggear_macro_keystrokes);
		}
		g_free(macroset_name);
	}

exit:
	close(fd);
	return gaminggear_macros;
}

static GaminggearMacrosConverter converter = {
	.import = macros_conversions_roccat_swarm_import,
	.export = macros_conversions_roccat_swarm_export,
	.file_extension = "dat",
	.pattern = "*.dat",
	.name = N_("Roccat Swarm macro files"),
};

GaminggearMacrosConverter const *macros_converter_roccat_swarm(void) {
	return &converter;
}
