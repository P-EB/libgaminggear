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

#include "gaminggear/macro_threads.h"
#include "gaminggear_macro_thread.h"
#include "gaminggear_helper.h"
#include <unistd.h>

struct _GaminggearMacroThreads {
	GaminggearMacroThread **threads;
	guint size;
};

static void gaminggear_macro_threads_unref_thread(GaminggearMacroThreads *macro_threads, guint index) {
	g_assert(index < macro_threads->size);
	
	g_clear_object(&macro_threads->threads[index]);
}

GaminggearMacroThreads *gaminggear_macro_threads_new(guint size) {
	GaminggearMacroThreads *result;

	result = g_malloc0(sizeof(GaminggearMacroThreads));

	result->size = size;
	result->threads = g_malloc0(size * sizeof(GaminggearMacroThread*));

	return result;
}

void gaminggear_macro_threads_free(GaminggearMacroThreads *macro_threads) {
	guint i;

	for (i = 0; i < macro_threads->size; ++i) {
		if (macro_threads->threads[i]) {
			/* TODO cancel yes or no? */
			gaminggear_macro_thread_set_cancelled(macro_threads->threads[i]);
			gaminggear_macro_threads_unref_thread(macro_threads, i);
		}
	}

	g_free(macro_threads->threads);
	g_free(macro_threads);
}

void gaminggear_play_macro_threaded(GaminggearMacroThreads *macro_threads, guint index, GaminggearMacro const *macro) {
	g_assert(index < macro_threads->size);

	/* if there is a macro still playing, just stop it */
	if (macro_threads->threads[index] && gaminggear_macro_thread_get_running(macro_threads->threads[index])) {
		gaminggear_macro_thread_set_cancelled(macro_threads->threads[index]);
		
		/* allow for early finalization */
		gaminggear_macro_threads_unref_thread(macro_threads, index);
		return;
	}
	
	if (macro_threads->threads[index])
		gaminggear_macro_threads_unref_thread(macro_threads, index);
	
	macro_threads->threads[index] = gaminggear_macro_thread_new(macro);

	/* IDEA invent uncancellable, multiexec macro option
	 * call gaminggear_macro_threads_unref_thread immediatly
	 */
}
