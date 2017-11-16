#ifndef __GAMINGGEAR_MACROS_CONVERTER_H__
#define __GAMINGGEAR_MACROS_CONVERTER_H__

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

#include "gaminggear_macros.h"

G_BEGIN_DECLS

typedef struct _GaminggearMacrosConverter GaminggearMacrosConverter;

struct _GaminggearMacrosConverter {
	GaminggearMacros *(*import)(gchar const *filename, GError **error);
	gboolean (*export)(gchar const *filename, GaminggearMacros *gaminggear_macros, GError **error);
	gchar const * const file_extension;
	gchar const * const pattern;
	gchar const * const name;
};

G_END_DECLS

#endif
