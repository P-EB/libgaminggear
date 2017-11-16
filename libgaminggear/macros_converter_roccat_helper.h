#ifndef __MACROS_CONVERTER_ROCCAT_HELPER_H__
#define __MACROS_CONVERTER_ROCCAT_HELPER_H__

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

#include <glib.h>

static inline gulong roccat_calc_bytesum(gconstpointer data, gsize size) {
	guchar const *array = (guchar const *)data;
	gsize i;
	gulong result = 0;

	for (i = 0; i < size; ++i)
		result += array[i];

	return result;
}

#define ROCCAT_BYTESUM_PARTIALLY(ptr, struct_type, first_member, last_member) \
	roccat_calc_bytesum((gchar const *)ptr + G_STRUCT_OFFSET(struct_type, first_member), \
		G_STRUCT_OFFSET(struct_type, last_member) - G_STRUCT_OFFSET(struct_type, first_member))

G_END_DECLS

#endif
