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

static unsigned int const description_size = 255;

static gchar const *get_device_type_string(GfxDevtype device_type) {
	switch(device_type) {
	case GFX_DEVTYPE_MOUSE:
		return _("MOUSE");
		break;
	case GFX_DEVTYPE_KEYBOARD:
		return _("KEYBOARD");
		break;
	default:
		return _("INVALID");
		break;
	}
}

int main(int argc, char **argv) {
	GfxResult gfx_result;
	int retval = EXIT_FAILURE;
	char description[description_size];
	unsigned int num_devices;
	unsigned int num_lights;
	unsigned int device_index;
	unsigned int light_index;
	GfxDevtype device_type;
	GfxPosition light_position;
	uint32_t light_color;

	setlocale(LC_ALL, "");
	bindtextdomain(PROJECT_NAME, LOCALEDIR);
	bind_textdomain_codeset(PROJECT_NAME, "UTF-8");
	textdomain(PROJECT_NAME);

	gfx_result = gfx_initialize();
	if (gfx_result != GFX_SUCCESS) {
		g_warning(_("Could not initialize fx system"));
		goto exit_1;
	}

	gfx_result = gfx_get_num_devices(&num_devices);
	if (gfx_result != GFX_SUCCESS) {
		g_warning(_("Could not get devices"));
		goto exit_2;
	}

	for (device_index = 0; device_index < num_devices; ++device_index) {
		g_print("%s[%u]\n", _("Device"), device_index);

		gfx_result = gfx_get_device_description(device_index, description, description_size, &device_type);
		if (gfx_result != GFX_SUCCESS) {
			g_warning(_("Could not get device description"));
			goto exit_2;
		}

		g_print("%*s%-*s%s\n", 2, "", 16, _("Description: "), description);
		g_print("%*s%-*s%i (%s)\n", 2, "", 16, _("Device type: "), device_type, get_device_type_string(device_type));

		gfx_result = gfx_get_num_lights(device_index, &num_lights);
		if (gfx_result != GFX_SUCCESS) {
			g_warning(_("Could not get lights"));
			goto exit_2;
		}

		for (light_index = 0; light_index < num_lights; ++light_index) {
			g_print("%*s%s[%u]\n", 2, "", _("Light"), light_index);

			gfx_result = gfx_get_light_description(device_index, light_index, description, description_size);
			if (gfx_result != GFX_SUCCESS) {
				g_warning(_("Could not get light description"));
				goto exit_2;
			}
			g_print("%*s%-*s%s\n", 4, "", 14, _("Description: "), description);

			gfx_result = gfx_get_light_position(device_index, light_index, &light_position);
			if (gfx_result != GFX_SUCCESS) {
				g_warning(_("Could not get light position"));
				goto exit_2;
			}
			g_print("%*s%-*s%u, %u, %u\n", 4, "", 14, _("Position: "), light_position.x, light_position.y, light_position.z);

			gfx_result = gfx_get_light_color(device_index, light_index, &light_color);
			if (gfx_result != GFX_SUCCESS) {
				g_warning(_("Could not get light color"));
				goto exit_2;
			}
			g_print("%*s%-*s0x%02x, 0x%02x%02x%02x\n", 4, "", 14, _("Color: "),
					gfx_color_get_brightness_raw(light_color),
					gfx_color_get_red_raw(light_color),
					gfx_color_get_green_raw(light_color),
					gfx_color_get_blue_raw(light_color));
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
	return retval;
}
