#ifndef __GAMINGEAR_DSCALE_H__
#define __GAMINGEAR_DSCALE_H__

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

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GAMINGGEAR_DSCALE_TYPE (gaminggear_dscale_get_type())
#define GAMINGGEAR_DSCALE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GAMINGGEAR_DSCALE_TYPE, GaminggearDscale))
#define IS_GAMINGGEAR_DSCALE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GAMINGGEAR_DSCALE_TYPE))

typedef struct _GaminggearDscale GaminggearDscale;
typedef struct _GaminggearDscalePrivate GaminggearDscalePrivate;

struct _GaminggearDscale {
	GtkWidget widget;
	GaminggearDscalePrivate *priv;
};

GType gaminggear_dscale_get_type(void);
GtkWidget *gaminggear_dscale_new(void);

void gaminggear_dscale_set_min(GaminggearDscale *scale, gdouble new_value);
gdouble gaminggear_dscale_get_min(GaminggearDscale *scale);

void gaminggear_dscale_set_lower(GaminggearDscale *scale, gdouble new_value);
gdouble gaminggear_dscale_get_lower(GaminggearDscale *scale);

void gaminggear_dscale_set_upper(GaminggearDscale *scale, gdouble new_value);
gdouble gaminggear_dscale_get_upper(GaminggearDscale *scale);

void gaminggear_dscale_set_max(GaminggearDscale *scale, gdouble new_value);
gdouble gaminggear_dscale_get_max(GaminggearDscale *scale);

void gaminggear_dscale_set_fix_min(GaminggearDscale *scale, gboolean new_value);
gboolean gaminggear_dscale_get_fix_min(GaminggearDscale *scale);

void gaminggear_dscale_set_fix_max(GaminggearDscale *scale, gboolean new_value);
gboolean gaminggear_dscale_get_fix_max(GaminggearDscale *scale);

void gaminggear_dscale_set_gripping_width(GaminggearDscale *scale, guint new_value);
guint gaminggear_dscale_get_gripping_width(GaminggearDscale *scale);

void gaminggear_dscale_set_slowdown(GaminggearDscale *scale, guint new_value);
guint gaminggear_dscale_get_slowdown(GaminggearDscale *scale);

G_END_DECLS

#endif
