#ifndef __GAMINGGEAR_RULER_H__
#define __GAMINGGEAR_RULER_H__

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

#define GAMINGGEAR_RULER_TYPE (gaminggear_ruler_get_type())
#define GAMINGGEAR_RULER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GAMINGGEAR_RULER_TYPE, GaminggearRuler))
#define GAMINGGEAR_RULER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GAMINGGEAR_RULER_TYPE, GaminggearRulerClass))
#define IS_GAMINGGEAR_RULER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GAMINGGEAR_RULER_TYPE))
#define IS_GAMINGGEAR_RULER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GAMINGGEAR_RULER_TYPE))

typedef struct _GaminggearRuler GaminggearRuler;
typedef struct _GaminggearRulerClass GaminggearRulerClass;
typedef struct _GaminggearRulerPrivate GaminggearRulerPrivate;

struct _GaminggearRuler {
	GtkWidget widget;
	GaminggearRulerPrivate *priv;
};

struct _GaminggearRulerClass {
	GtkWidgetClass parent_class;
};

GType gaminggear_ruler_get_type(void);
GtkWidget *gaminggear_ruler_new(void);

void gaminggear_ruler_set_min(GaminggearRuler *ruler, gdouble new_value);
gdouble gaminggear_ruler_get_min(GaminggearRuler *ruler);

void gaminggear_ruler_set_max(GaminggearRuler *ruler, gdouble new_value);
gdouble gaminggear_ruler_get_max(GaminggearRuler *ruler);

void gaminggear_ruler_set_interval(GaminggearRuler *ruler, gdouble new_value);
gdouble gaminggear_ruler_get_interval(GaminggearRuler *ruler);

G_END_DECLS

#endif
