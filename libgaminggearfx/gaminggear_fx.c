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

#include "config.h"
#include "gaminggear/gfx.h"
#include "gaminggear/gaminggear_fx_plugin.h"
#include "gaminggear/threads.h"
#include <gmodule.h>
#include <glib.h>
#include <glib-object.h>

typedef struct _PluginListData PluginListData;
typedef struct _Work Work;

struct _PluginListData {
	GModule *module;
	GaminggearFxPlugin *plugin;
};

struct _Work {
	guint type;
	unsigned int device_index;
	unsigned int light_index;
	uint32_t color1;
	uint32_t location_mask;
};

typedef enum {
	WORK_TYPE_UPDATE,
	WORK_TYPE_RESET,
	WORK_TYPE_ALL,
	WORK_TYPE_SINGLE,
} WorkType;

GaminggearMutex work_list_mutex;
GaminggearCond new_work_condition;
GaminggearCond work_list_empty_condition;
GQueue *work_list;
GThread *thread;
gboolean cancelled;

GaminggearRWLock plugins_lock;
GSList *plugins;
gboolean initialized;

static void do_single(unsigned int const device_index, unsigned int const light_index, uint32_t const color) {
	GaminggearFxPlugin *plugin;

	plugin = ((PluginListData *)g_slist_nth_data(plugins, device_index))->plugin;
	if (!plugin)
		return;

	plugin->set_light_color(plugin, light_index, color);
	return;
}

static void do_all(uint32_t const location_mask, uint32_t const color) {
	unsigned int num_devices;
	unsigned int num_lights;
	unsigned int device_index;
	unsigned int light_index;

	if (gfx_get_num_devices(&num_devices) != GFX_SUCCESS)
		return;

	for (device_index = 0; device_index < num_devices; ++device_index) {
		if (gfx_get_num_lights(device_index, &num_lights) != GFX_SUCCESS)
			return;

		for (light_index = 0; light_index < num_lights; ++light_index) {
			if (TRUE) // FIXME conditional
				do_single(device_index, light_index, color);
		}
	}
}

static void do_update(void) {
	GaminggearFxPlugin *plugin;
	GSList *iterator;

	for (iterator = plugins; iterator; iterator = g_slist_next(iterator)) {
		plugin = ((PluginListData *)iterator->data)->plugin;
		if (plugin->update(plugin) != GFX_SUCCESS)
			return;
	}
}

static void do_reset(void) {
	do_all(GFX_LOCATION_ALL, 0);
}

static gpointer worker(gpointer data) {
	Work *work;

	while (TRUE) {
		gaminggear_mutex_lock(&work_list_mutex);
		while ((work = g_queue_pop_head(work_list)) == NULL) {
			gaminggear_cond_signal(&work_list_empty_condition);
			if (cancelled) {
				gaminggear_mutex_unlock(&work_list_mutex);
				return NULL;
			} else
				gaminggear_cond_wait(&new_work_condition, &work_list_mutex);
		}
		gaminggear_mutex_unlock(&work_list_mutex);

		gaminggear_rw_lock_reader_lock(&plugins_lock);

		switch (work->type) {
		case WORK_TYPE_UPDATE:
			g_debug("Update");
			do_update();
			break;
		case WORK_TYPE_RESET:
			g_debug("Reset");
			do_reset();
			break;
		case WORK_TYPE_ALL:
			g_debug("All[0x%08x] = 0x%08x", work->location_mask, work->color1);
			do_all(work->location_mask, work->color1);
			break;
		case WORK_TYPE_SINGLE:
			g_debug("Single[%u][%u] = 0x%08x", work->device_index, work->light_index, work->color1);
			do_single(work->device_index, work->light_index, work->color1);
			break;
		default:
			break;
		}

		gaminggear_rw_lock_reader_unlock(&plugins_lock);

		g_free(work);
	}

	return NULL;
}

static PluginListData *plugin_list_data_new(gchar const * const filename) {
	GAMINGGEAR_FX_PLUGIN_NEW gaminggear_fx_plugin_new;
	GaminggearFxPlugin *plugin;
	PluginListData *list_data;
	GModule *module;

	module = g_module_open(filename, G_MODULE_BIND_LAZY);
	if (!module)
		goto exit_1;

	if (!g_module_symbol(module, GAMINGGEAR_FX_SYMBOL_PLUGIN_NEW, (gpointer *)&gaminggear_fx_plugin_new))
		goto exit_2;

	plugin = gaminggear_fx_plugin_new();
	if (!plugin)
		goto exit_2;

	list_data = g_malloc0(sizeof(PluginListData));
	list_data->module = module;
	list_data->plugin = plugin;
	return list_data;

exit_2:
	g_module_close(module);
exit_1:
	return NULL;
}

GfxResult gfx_initialize(void) {
	GError *local_error = NULL;
	GDir *dir;
	GPatternSpec *pattern;
	gchar const *name;
	gchar *abs_name;
	PluginListData *list_data;

	gaminggear_rw_lock_writer_lock(&plugins_lock);

	if (initialized) {
		gaminggear_rw_lock_writer_unlock(&plugins_lock);
		return GFX_FAILURE;
	}

	if (!g_module_supported()) {
		gaminggear_rw_lock_writer_unlock(&plugins_lock);
		return GFX_FAILURE;
	}

	dir = g_dir_open(GAMINGGEAR_FX_PLUGIN_DIR, 0, &local_error);
	if (!dir) {
		g_warning("Error opening directory '%s': %s", GAMINGGEAR_FX_PLUGIN_DIR, local_error->message);
		g_clear_error(&local_error);
		gaminggear_rw_lock_writer_unlock(&plugins_lock);
		return GFX_FAILURE;
	}

	pattern = g_pattern_spec_new("lib*gfxplugin.so");

	while ((name = g_dir_read_name(dir))) {
		if (g_pattern_match_string(pattern, name)) {
			g_debug("Trying plugin %s", name);
			abs_name = g_build_filename(GAMINGGEAR_FX_PLUGIN_DIR, name, NULL);
			list_data = plugin_list_data_new(abs_name);
			g_free(abs_name);
			if (list_data) {
				g_debug("Using plugin %s", name);
				plugins = g_slist_prepend(plugins, list_data);
			}
		}
	}

	g_pattern_spec_free(pattern);
	g_dir_close(dir);

	initialized = TRUE;

	if (!plugins) {
		gaminggear_rw_lock_writer_unlock(&plugins_lock);
		return GFX_ERROR_NODEVS;
	}

	gaminggear_rw_lock_writer_unlock(&plugins_lock);
	return GFX_SUCCESS;
}

static void plugin_list_data_free(gpointer data, gpointer user_data) {
	PluginListData *list_data = (PluginListData *)data;
	list_data->plugin->finalize(list_data->plugin);
	g_module_close(list_data->module);
	g_free(list_data);
}

GfxResult gfx_release(void) {
	gaminggear_mutex_lock(&work_list_mutex);
	while (!g_queue_is_empty(work_list))
		gaminggear_cond_wait(&work_list_empty_condition, &work_list_mutex);

	gaminggear_rw_lock_writer_lock(&plugins_lock);

	if (!initialized) {
		gaminggear_rw_lock_writer_unlock(&plugins_lock);
		gaminggear_mutex_unlock(&work_list_mutex);
		return GFX_ERROR_NOINIT;
	}

	g_slist_foreach(plugins, plugin_list_data_free, NULL);
	g_slist_free(plugins);

	plugins = NULL;
	initialized = FALSE;

	gaminggear_rw_lock_writer_unlock(&plugins_lock);
	gaminggear_mutex_unlock(&work_list_mutex);

	return GFX_SUCCESS;
}

GfxResult gfx_get_num_devices(unsigned int * const num_devices) {
	gaminggear_rw_lock_reader_lock(&plugins_lock);

	if (!initialized) {
		gaminggear_rw_lock_reader_unlock(&plugins_lock);
		return GFX_ERROR_NOINIT;
	}

	if (!plugins) {
		gaminggear_rw_lock_reader_unlock(&plugins_lock);
		return GFX_ERROR_NODEVS;
	}

	*num_devices = g_slist_length(plugins);

	gaminggear_rw_lock_reader_unlock(&plugins_lock);
	return GFX_SUCCESS;
}

GfxResult gfx_get_device_description(unsigned int const device_index, char * const device_description,
		unsigned int const device_description_size, GfxDevtype * const device_type) {
	GaminggearFxPlugin *plugin;
	GfxResult result;

	gaminggear_rw_lock_reader_lock(&plugins_lock);

	if (!initialized) {
		gaminggear_rw_lock_reader_unlock(&plugins_lock);
		return GFX_ERROR_NOINIT;
	}

	plugin = ((PluginListData *)g_slist_nth_data(plugins, device_index))->plugin;

	if (!plugin) {
		gaminggear_rw_lock_reader_unlock(&plugins_lock);
		return GFX_ERROR_NODEVS;
	}

	result = plugin->get_device_description(plugin, device_description, device_description_size, device_type);

	gaminggear_rw_lock_reader_unlock(&plugins_lock);
	return result;
}

GfxResult gfx_get_num_lights(unsigned int const device_index, unsigned int *const num_lights) {
	GaminggearFxPlugin *plugin;
	GfxResult result;

	gaminggear_rw_lock_reader_lock(&plugins_lock);

	if (!initialized) {
		gaminggear_rw_lock_reader_unlock(&plugins_lock);
		return GFX_ERROR_NOINIT;
	}

	plugin = ((PluginListData *)g_slist_nth_data(plugins, device_index))->plugin;

	if (!plugin) {
		gaminggear_rw_lock_reader_unlock(&plugins_lock);
		return GFX_ERROR_NODEVS;
	}

	result = plugin->get_num_lights(plugin, num_lights);

	gaminggear_rw_lock_reader_unlock(&plugins_lock);
	return result;
}

GfxResult gfx_get_light_description(unsigned int const device_index, unsigned int const light_index,
		char * const light_description, unsigned int const light_description_size) {
	GaminggearFxPlugin *plugin;
	GfxResult result;

	gaminggear_rw_lock_reader_lock(&plugins_lock);

	if (!initialized) {
		gaminggear_rw_lock_reader_unlock(&plugins_lock);
		return GFX_ERROR_NOINIT;
	}

	plugin = ((PluginListData *)g_slist_nth_data(plugins, device_index))->plugin;

	if (!plugin) {
		gaminggear_rw_lock_reader_unlock(&plugins_lock);
		return GFX_ERROR_NODEVS;
	}

	result = plugin->get_light_description(plugin, light_index, light_description, light_description_size);

	gaminggear_rw_lock_reader_unlock(&plugins_lock);
	return result;
}

GfxResult gfx_get_light_position(unsigned int const device_index, unsigned int const light_index,
		GfxPosition * const light_position) {
	GaminggearFxPlugin *plugin;
	GfxResult result;

	gaminggear_rw_lock_reader_lock(&plugins_lock);

	if (!initialized) {
		gaminggear_rw_lock_reader_unlock(&plugins_lock);
		return GFX_ERROR_NOINIT;
	}

	plugin = ((PluginListData *)g_slist_nth_data(plugins, device_index))->plugin;

	if (!plugin) {
		gaminggear_rw_lock_reader_unlock(&plugins_lock);
		return GFX_ERROR_NODEVS;
	}

	result = plugin->get_light_position(plugin, light_index, light_position);

	gaminggear_rw_lock_reader_unlock(&plugins_lock);
	return result;
}

GfxResult gfx_get_light_color(unsigned int const device_index, unsigned int const light_index, uint32_t * const color) {
	GaminggearFxPlugin *plugin;
	GfxResult result;

	gaminggear_rw_lock_reader_lock(&plugins_lock);

	if (!initialized) {
		gaminggear_rw_lock_reader_unlock(&plugins_lock);
		return GFX_ERROR_NOINIT;
	}

	plugin = ((PluginListData *)g_slist_nth_data(plugins, device_index))->plugin;

	if (!plugin) {
		gaminggear_rw_lock_reader_unlock(&plugins_lock);
		return GFX_ERROR_NODEVS;
	}

	result = plugin->get_light_color(plugin, light_index, color);

	gaminggear_rw_lock_reader_unlock(&plugins_lock);
	return result;
}

GfxResult gfx_set_light_color(unsigned int const device_index, unsigned int const light_index, uint32_t const color) {
	Work *work;

	gaminggear_rw_lock_reader_lock(&plugins_lock);

	if (!initialized) {
		gaminggear_rw_lock_reader_unlock(&plugins_lock);
		return GFX_ERROR_NOINIT;
	}

	if (!plugins) {
		gaminggear_rw_lock_reader_unlock(&plugins_lock);
		return GFX_ERROR_NODEVS;
	}

	work = (Work *)g_malloc(sizeof(Work));
	work->type = WORK_TYPE_SINGLE;
	work->device_index = device_index;
	work->light_index = light_index;
	work->color1 = color;
	gaminggear_mutex_lock(&work_list_mutex);
	g_queue_push_tail(work_list, work);
	gaminggear_cond_signal(&new_work_condition);
	gaminggear_mutex_unlock(&work_list_mutex);

	gaminggear_rw_lock_reader_unlock(&plugins_lock);
	return GFX_SUCCESS;
}

GfxResult gfx_light(unsigned int const location_mask, uint32_t const color) {
	Work *work;

	gaminggear_rw_lock_reader_lock(&plugins_lock);

	if (!initialized) {
		gaminggear_rw_lock_reader_unlock(&plugins_lock);
		return GFX_ERROR_NOINIT;
	}

	if (!plugins) {
		gaminggear_rw_lock_reader_unlock(&plugins_lock);
		return GFX_ERROR_NODEVS;
	}

	work = (Work *)g_malloc(sizeof(Work));
	work->type = WORK_TYPE_ALL;
	work->location_mask = location_mask;
	work->color1 = color;
	gaminggear_mutex_lock(&work_list_mutex);
	g_queue_push_tail(work_list, work);
	gaminggear_cond_signal(&new_work_condition);
	gaminggear_mutex_unlock(&work_list_mutex);

	gaminggear_rw_lock_reader_unlock(&plugins_lock);
	return GFX_SUCCESS;
}

GfxResult gfx_reset(void) {
	Work *work;

	gaminggear_rw_lock_reader_lock(&plugins_lock);

	if (!initialized) {
		gaminggear_rw_lock_reader_unlock(&plugins_lock);
		return GFX_ERROR_NOINIT;
	}

	if (!plugins) {
		gaminggear_rw_lock_reader_unlock(&plugins_lock);
		return GFX_ERROR_NODEVS;
	}

	work = (Work *)g_malloc(sizeof(Work));
	work->type = WORK_TYPE_RESET;
	gaminggear_mutex_lock(&work_list_mutex);
	g_queue_push_tail(work_list, work);
	gaminggear_cond_signal(&new_work_condition);
	gaminggear_mutex_unlock(&work_list_mutex);

	gaminggear_rw_lock_reader_unlock(&plugins_lock);
	return GFX_SUCCESS;
}

GfxResult gfx_update(void) {
	Work *work;

	gaminggear_rw_lock_reader_lock(&plugins_lock);

	if (!initialized) {
		gaminggear_rw_lock_reader_unlock(&plugins_lock);
		return GFX_ERROR_NOINIT;
	}

	if (!plugins) {
		gaminggear_rw_lock_reader_unlock(&plugins_lock);
		return GFX_ERROR_NODEVS;
	}

	work = (Work *)g_malloc(sizeof(Work));
	work->type = WORK_TYPE_UPDATE;
	gaminggear_mutex_lock(&work_list_mutex);
	g_queue_push_tail(work_list, work);
	gaminggear_cond_signal(&new_work_condition);
	gaminggear_mutex_unlock(&work_list_mutex);

	gaminggear_rw_lock_reader_unlock(&plugins_lock);
	return GFX_SUCCESS;
}

static __attribute__((constructor)) void constructor(void) {
#if !(GLIB_CHECK_VERSION(2, 36, 0))
	g_type_init();
#endif

	gaminggear_mutex_init(&work_list_mutex);
	gaminggear_cond_init(&new_work_condition);
	gaminggear_cond_init(&work_list_empty_condition);
	work_list = g_queue_new();
	gaminggear_rw_lock_init(&plugins_lock);
	plugins = NULL;
	initialized = FALSE;
	cancelled = FALSE;
	thread = gaminggear_thread_try_new(worker, NULL, NULL);
}

static __attribute__((destructor)) void destructor(void) {
	gaminggear_mutex_lock(&work_list_mutex);
	cancelled = TRUE;
	gaminggear_cond_signal(&new_work_condition);
	gaminggear_mutex_unlock(&work_list_mutex);
	g_thread_join(thread);

	gaminggear_mutex_clear(&work_list_mutex);
	gaminggear_cond_clear(&new_work_condition);
	gaminggear_cond_clear(&work_list_empty_condition);
	gaminggear_rw_lock_clear(&plugins_lock);
}
