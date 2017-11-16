#ifndef __GAMINGGEAR_MACRO_THREADS_H__
#define __GAMINGGEAR_MACRO_THREADS_H__

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

/*! \file gaminggear/macro_threads.h
 *  \brief Macro threads.
 */

#include <gaminggear/macro.h>

G_BEGIN_DECLS

typedef struct _GaminggearMacroThreads GaminggearMacroThreads;

/*! \brief Create array of macro threads.
 *  \param size Mostly count of physical buttons that are allowed to play macros.
 *  \retval array That has to be freed with gaminggear_macro_threads_free().
 *  \since 1.0
 */
GaminggearMacroThreads *gaminggear_macro_threads_new(guint size);

/*! \brief Free array of macro threads.
 *  \param macro_threads
 *  \since 1.0
 */
void gaminggear_macro_threads_free(GaminggearMacroThreads *macro_threads);

/*! \brief Play macro in a thread.
 *
 *  If a macro is still playing, execution of that running macro is stopped
 *  instead of execution of new macro.
 *
 *  \since 1.0
 */
void gaminggear_play_macro_threaded(GaminggearMacroThreads *threads, guint index, GaminggearMacro const *macro);

G_END_DECLS

#endif
