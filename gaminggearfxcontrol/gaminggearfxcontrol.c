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

#include "gaminggear/gfx.h"
#include "config.h"
#include "i18n.h"
#include <glib.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>

static guint const description_size = 255;

static gchar *parameter_device = NULL;
static gboolean parameter_all = FALSE;
static gboolean parameter_single = FALSE;
static gboolean parameter_reset = FALSE;
static gboolean parameter_color = FALSE;
static gint parameter_light_index = -1;
static gint64 parameter_color1 = 0;

static GOptionEntry entries[] = {
	{ "device", 'd', 0, G_OPTION_ARG_STRING, &parameter_device, N_("use device STRING"), N_("STRING") },
	{ "light", 'l', 0, G_OPTION_ARG_INT, &parameter_light_index, N_("use light index INDEX"), N_("INDEX") },
	{ "all", 'a', 0, G_OPTION_ARG_NONE, &parameter_all, N_("set all colors"), NULL },
	{ "single", 's', 0, G_OPTION_ARG_NONE, &parameter_single, N_("set single color"), NULL },
	{ "reset", 'r', 0, G_OPTION_ARG_NONE, &parameter_reset, N_("reset colors"), NULL },
	{ "color", 'c', 0, G_OPTION_ARG_NONE, &parameter_color, N_("set plain color"), NULL },
	{ "color1", '1', 0, G_OPTION_ARG_INT64, &parameter_color1, N_("COLOR1"), N_("COLOR1") },
	{ NULL }
};

static gboolean post_parse_func(GOptionContext *context, GOptionGroup *group, gpointer data, GError **error) {
	guint actions = 0;

	if (parameter_all) ++actions;
	if (parameter_single) ++actions;
	if (parameter_reset) ++actions;

	if (actions < 1) {
		g_set_error(error, G_OPTION_ERROR, G_OPTION_ERROR_FAILED, _("give one of --all, --single or --reset"));
		return FALSE;
	}

	if (actions > 1) {
		g_set_error(error, G_OPTION_ERROR, G_OPTION_ERROR_FAILED, _("--all, --single and --reset are mutual exclusive"));
		return FALSE;
	}

	if ((parameter_all || parameter_single) && !parameter_color) {
		g_set_error(error, G_OPTION_ERROR, G_OPTION_ERROR_FAILED, _("--all and --single need --color"));
		return FALSE;
	}

	if (parameter_single && (!parameter_device || parameter_light_index == -1)) {
		g_set_error(error, G_OPTION_ERROR, G_OPTION_ERROR_FAILED, _("--single needs --device and --light"));
		return FALSE;
	}

	if (parameter_color && !parameter_color1) {
		g_set_error(error, G_OPTION_ERROR, G_OPTION_ERROR_FAILED, _("--color needs --color1"));
		return FALSE;
	}

	return TRUE;
}

static GOptionContext *commandline_parse(int *argc, char ***argv) {
	GOptionContext *context = NULL;
	GError *error = NULL;

	context = g_option_context_new(_("- control Gaminggear FX"));

	g_option_context_add_main_entries(context, entries, NULL);
	g_option_context_set_translation_domain(context, PROJECT_NAME);

	g_option_group_set_parse_hooks(g_option_context_get_main_group(context), NULL, post_parse_func);

	if (!g_option_context_parse(context, argc, argv, &error)) {
		g_critical(_("Option parsing failed: %s"), error->message);
		exit(EXIT_FAILURE);
	}
	return context;
}

static void commandline_free(GOptionContext *context) {
	g_free(parameter_device);
}

static gint get_device_index(gchar const *parameter) {
	GfxResult gfx_result;
	unsigned int num_devices;
	unsigned int device_index;
	char description[description_size];
	GfxDevtype device_type;

	gfx_result = gfx_get_num_devices(&num_devices);
	if (gfx_result != GFX_SUCCESS) {
		g_warning(_("Could not get devices"));
		return -1;
	}

	for (device_index = 0; device_index < num_devices; ++device_index) {
		gfx_result = gfx_get_device_description(device_index, description, description_size, &device_type);
		if (gfx_result != GFX_SUCCESS) {
			g_warning(_("Could not get device description"));
			return -1;
		}

		if (!strcmp(description, parameter))
			return device_index;
	}
	return -1;
}

int main(int argc, char **argv) {
	GOptionContext *context;
	GfxResult gfx_result;
	int retval = EXIT_FAILURE;
	gint device_index;

	setlocale(LC_ALL, "");
	bindtextdomain(PROJECT_NAME, LOCALEDIR);
	bind_textdomain_codeset(PROJECT_NAME, "UTF-8");
	textdomain(PROJECT_NAME);

	context = commandline_parse(&argc, &argv);

	gfx_result = gfx_initialize();
	if (gfx_result != GFX_SUCCESS) {
		g_warning(_("Could not initialize fx system"));
		goto exit_1;
	}

	if (parameter_single) {
		device_index = get_device_index(parameter_device);
		if (device_index == -1) {
			g_warning(_("Could not find device %s"), parameter_device);
			goto exit_2;
		}
		if (parameter_color) {
			gfx_result = gfx_set_light_color(device_index, parameter_light_index, parameter_color1);
			if (gfx_result != GFX_SUCCESS) {
				g_warning(_("Could not set light %1$i on device %2$s to color1"), parameter_light_index, parameter_device);
				goto exit_2;
			}
		}
	} else if (parameter_all) {
		if (parameter_color) {
			gfx_result = gfx_light(GFX_LOCATION_ALL, parameter_color1);
			if (gfx_result != GFX_SUCCESS) {
				g_warning(_("Could not set all lights to color1"));
				goto exit_2;
			}
		}
	} else if (parameter_reset) {
		gfx_result = gfx_reset();
		if (gfx_result != GFX_SUCCESS) {
			g_warning(_("Could not reset all lights"));
			goto exit_2;
		}
	}

	if (parameter_all || parameter_single || parameter_reset) {
		gfx_result = gfx_update();
		if (gfx_result != GFX_SUCCESS) {
			g_warning(_("Could not update lights"));
			goto exit_2;
		}
	}

	retval = EXIT_SUCCESS;
exit_2:
	gfx_result = gfx_release();
	if (gfx_result != GFX_SUCCESS) {
		g_warning(_("Could not release fx system"));
		retval = EXIT_FAILURE;
		goto exit_1;
	}
exit_1:
	commandline_free(context);
	return retval;
}
