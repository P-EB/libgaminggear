#ifndef __GAMINGGEAR_FX_PLUGIN_H__
#define __GAMINGGEAR_FX_PLUGIN_H__

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

#include <gaminggear/gfx.h>

#ifdef  __cplusplus
extern "C" {
#endif

#define GAMINGGEAR_FX_PLUGIN_API __attribute__((visibility("default")))

typedef struct _GaminggearFxPlugin GaminggearFxPlugin;

struct _GaminggearFxPlugin {
	void *private;
	GfxResult (*get_device_description)(GaminggearFxPlugin *self, char * const device_description,
			unsigned int const device_description_size, GfxDevtype * const device_type);
	GfxResult (*get_num_lights)(GaminggearFxPlugin *self, unsigned int * const num_lights);
	GfxResult (*get_light_description)(GaminggearFxPlugin *self, unsigned int const light_index,
			char * const light_description, unsigned int const light_description_size);
	GfxResult (*get_light_position)(GaminggearFxPlugin *self, unsigned int const light_index,
			GfxPosition * const light_position);
	GfxResult (*get_light_color)(GaminggearFxPlugin *self, unsigned int const light_index,
			uint32_t * const color);
	GfxResult (*set_light_color)(GaminggearFxPlugin *self, unsigned int const light_index,
			uint32_t const color);
	GfxResult (*update)(GaminggearFxPlugin *self);
	void (*finalize)(GaminggearFxPlugin *self);
};

GaminggearFxPlugin *gaminggear_fx_plugin_new(void);
#define GAMINGGEAR_FX_SYMBOL_PLUGIN_NEW "gaminggear_fx_plugin_new"
typedef GaminggearFxPlugin *(*GAMINGGEAR_FX_PLUGIN_NEW)(void);

#ifdef  __cplusplus
}
#endif

#endif
