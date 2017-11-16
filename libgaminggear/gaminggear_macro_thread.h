#ifndef __GAMINGGEAR_MACRO_THREAD_H__
#define __GAMINGGEAR_MACRO_THREAD_H__

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
#include <glib-object.h>

G_BEGIN_DECLS

#define GAMINGGEAR_MACRO_THREAD_TYPE (gaminggear_macro_thread_get_type())
#define GAMINGGEAR_MACRO_THREAD(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GAMINGGEAR_MACRO_THREAD_TYPE, GaminggearMacroThread))
#define IS_GAMINGGEAR_MACRO_THREAD(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GAMINGGEAR_MACRO_THREAD_TYPE))

typedef struct _GaminggearMacroThread GaminggearMacroThread;
typedef struct _GaminggearMacroThreadPrivate GaminggearMacroThreadPrivate;

struct _GaminggearMacroThread {
	GObject parent;
	GaminggearMacroThreadPrivate *priv;
};

GType gaminggear_macro_thread_get_type(void);
GaminggearMacroThread *gaminggear_macro_thread_new(GaminggearMacro const *macro);

gboolean gaminggear_macro_thread_get_running(GaminggearMacroThread *gaminggear_macro_thread);

void gaminggear_macro_thread_set_cancelled(GaminggearMacroThread *gaminggear_macro_thread);
gboolean gaminggear_macro_thread_get_cancelled(GaminggearMacroThread *gaminggear_macro_thread);

void gaminggear_macro_thread_set_paused(GaminggearMacroThread *gaminggear_macro_thread, gboolean value);
void gaminggear_macro_thread_toggle_paused(GaminggearMacroThread *gaminggear_macro_thread);
gboolean gaminggear_macro_thread_get_paused(GaminggearMacroThread *gaminggear_macro_thread);

G_END_DECLS

#endif
