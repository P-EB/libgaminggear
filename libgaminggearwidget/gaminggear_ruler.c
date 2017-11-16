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

#include "gaminggear_ruler.h"
#include <glib.h>
#include <math.h>

#define GAMINGGEAR_RULER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), GAMINGGEAR_RULER_TYPE, GaminggearRulerPrivate))

struct _GaminggearRulerPrivate {
	gdouble min, max;
	gdouble interval;
};

enum {
	PROP_0,
	PROP_MIN,
	PROP_MAX,
	PROP_INTERVAL,
};

G_DEFINE_TYPE(GaminggearRuler, gaminggear_ruler, GTK_TYPE_WIDGET);

GtkWidget *gaminggear_ruler_new(void) {
	return GTK_WIDGET(g_object_new(GAMINGGEAR_RULER_TYPE, NULL));
}

static gdouble value_to_pixel(GtkWidget *widget, gdouble value) {
	GaminggearRuler *ruler;
	GaminggearRulerPrivate *priv;
	GtkAllocation allocation;

	ruler = GAMINGGEAR_RULER(widget);
	priv = ruler->priv;
	gtk_widget_get_allocation(widget, &allocation);
	/* cairo needs values to be exactly between two integers to draw sharp one pixel lines */
	return floor((gdouble)allocation.width * (value - priv->min) / (priv->max - priv->min)) + 0.5;
}

static void draw_ruler(GtkWidget *widget) {
	GaminggearRuler *ruler;
	GaminggearRulerPrivate *priv;
	gdouble tick;
	gdouble pixel;
	GtkAllocation allocation;
	GdkWindow *window;
	cairo_t *cr;

	window = gtk_widget_get_window(widget);

	if (!GDK_IS_DRAWABLE(window))
		return;

	ruler = GAMINGGEAR_RULER(widget);
	priv = ruler->priv;
	gtk_widget_get_allocation(widget, &allocation);

	gdk_window_clear_area(window, 0, 0, allocation.width, allocation.height);

	if (priv->max != priv->min && priv->interval > 0.0) {
		cr = gdk_cairo_create(window);
		cairo_set_line_width(cr, 1.0);
		cairo_set_line_cap(cr, CAIRO_LINE_CAP_SQUARE);
		gdk_cairo_set_source_color(cr, &gtk_widget_get_style(widget)->fg[GTK_STATE_NORMAL]);
		for (tick = priv->min; tick <= priv->max; tick += priv->interval) {
			pixel = value_to_pixel(widget, tick);
			cairo_move_to(cr, pixel, 0);
			cairo_line_to(cr, pixel, allocation.height);
			cairo_stroke(cr);
		}
		cairo_destroy(cr);
	}
}

void gaminggear_ruler_set_min(GaminggearRuler *ruler, gdouble new_value) {
	GaminggearRulerPrivate *priv = ruler->priv;

	priv->min = new_value;

	if (new_value > priv->max)
		priv->max = new_value;

	draw_ruler(GTK_WIDGET(ruler));
}

gdouble gaminggear_ruler_get_min(GaminggearRuler *ruler) {
	return ruler->priv->min;
}

void gaminggear_ruler_set_max(GaminggearRuler *ruler, gdouble new_value) {
	GaminggearRulerPrivate *priv = ruler->priv;

	priv->max = new_value;

	if (new_value < priv->min)
		priv->min = new_value;

	draw_ruler(GTK_WIDGET(ruler));
}

gdouble gaminggear_ruler_get_max(GaminggearRuler *ruler) {
	return ruler->priv->max;
}

void gaminggear_ruler_set_interval(GaminggearRuler *ruler, gdouble new_value) {
	ruler->priv->interval = new_value;
	draw_ruler(GTK_WIDGET(ruler));
}

gdouble gaminggear_ruler_get_interval(GaminggearRuler *ruler) {
	return ruler->priv->interval;
}

static void gaminggear_ruler_set_property(GObject *object, guint prop_id, GValue const *value, GParamSpec *pspec) {
	GaminggearRuler *ruler = GAMINGGEAR_RULER(object);
	switch(prop_id) {
	case PROP_MIN:
		gaminggear_ruler_set_min(ruler, g_value_get_double(value));
		break;
	case PROP_MAX:
		gaminggear_ruler_set_max(ruler, g_value_get_double(value));
		break;
	case PROP_INTERVAL:
		gaminggear_ruler_set_interval(ruler, g_value_get_double(value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static void gaminggear_ruler_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec) {
	GaminggearRuler *ruler = GAMINGGEAR_RULER(object);
	GaminggearRulerPrivate *priv = ruler->priv;

	switch(prop_id) {
	case PROP_MIN:
		g_value_set_double(value, priv->min);
		break;
	case PROP_MAX:
		g_value_set_double(value, priv->max);
		break;
	case PROP_INTERVAL:
		g_value_set_double(value, priv->interval);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static void gaminggear_ruler_realize(GtkWidget *widget)  {
	GaminggearRuler *ruler;
	GdkWindowAttr attributes;
	gint attr_mask;
	GtkAllocation allocation;
	GdkWindow *window;

	g_return_if_fail(widget != NULL);
	g_return_if_fail(IS_GAMINGGEAR_RULER(widget));

	gtk_widget_set_realized(widget, TRUE);
	ruler = GAMINGGEAR_RULER(widget);
	gtk_widget_get_allocation(widget, &allocation);

	attributes.x = allocation.x;
	attributes.y = allocation.y;
	attributes.width = allocation.width;
	attributes.height = allocation.height;
	attributes.wclass = GDK_INPUT_OUTPUT;
	attributes.window_type = GDK_WINDOW_CHILD;
	attributes.event_mask = gtk_widget_get_events(widget);
	attributes.event_mask |= (GDK_EXPOSURE_MASK);
	attributes.visual = gtk_widget_get_visual(widget);
	attributes.colormap = gtk_widget_get_colormap(widget);

	attr_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
	window = gdk_window_new(gtk_widget_get_parent_window(widget), &attributes, attr_mask);
	gtk_widget_set_window(widget, window);
	gdk_window_set_user_data(window, ruler);

	gtk_widget_style_attach(widget);
	gtk_style_set_background(gtk_widget_get_style(widget), window, GTK_STATE_NORMAL);
	gdk_window_show(window);
}

static gint gaminggear_ruler_expose(GtkWidget *widget, GdkEventExpose *event) {
	g_return_val_if_fail(widget != NULL || event != NULL, FALSE);
	g_return_val_if_fail(IS_GAMINGGEAR_RULER(widget), FALSE);

	if (event->count > 0)
		return TRUE;

	draw_ruler(widget);

	return TRUE;
}

// TODO height
static void gaminggear_ruler_size_request(GtkWidget *widget, GtkRequisition *requisition) {
	PangoFontDescription *fd;
	GtkStyle *style;

	g_return_if_fail(widget != NULL || requisition != NULL);
	g_return_if_fail(IS_GAMINGGEAR_RULER(widget));

	requisition->width = 100;

	style = gtk_widget_get_style(widget);
	fd = style->font_desc;
	requisition->height = pango_font_description_get_size(fd) / PANGO_SCALE * 2;
}

static void gaminggear_ruler_size_allocate(GtkWidget *widget, GtkAllocation *allocation) {
	g_return_if_fail(widget != NULL || allocation != NULL);
	g_return_if_fail(IS_GAMINGGEAR_RULER(widget));

	gtk_widget_set_allocation(widget, allocation);

	if (gtk_widget_get_realized(widget))
		gdk_window_move_resize(gtk_widget_get_window(widget), allocation->x, allocation->y, allocation->width, allocation->height);
}

static void gaminggear_ruler_init(GaminggearRuler *ruler) {
	GaminggearRulerPrivate *priv = GAMINGGEAR_RULER_GET_PRIVATE(ruler);

	ruler->priv = priv;

	priv->min = 0.0;
	priv->max = 0.0;
	priv->interval = 1.0;
}

static void gaminggear_ruler_class_init(GaminggearRulerClass *klass) {
	GObjectClass *gobject_class;
	GtkWidgetClass *widget_class;

	gobject_class = (GObjectClass*)klass;
	widget_class = (GtkWidgetClass*)klass;

	gobject_class->set_property = gaminggear_ruler_set_property;
	gobject_class->get_property = gaminggear_ruler_get_property;

	widget_class->realize = gaminggear_ruler_realize;
	widget_class->expose_event = gaminggear_ruler_expose;
	widget_class->size_request = gaminggear_ruler_size_request;
	widget_class->size_allocate = gaminggear_ruler_size_allocate;

	g_type_class_add_private(klass, sizeof(GaminggearRulerPrivate));

	g_object_class_install_property(gobject_class, PROP_MIN,
			g_param_spec_double("min",
					"DScale minimum value",
					"The minimum value",
					-G_MAXDOUBLE, G_MAXDOUBLE, 0.0,
					G_PARAM_READWRITE));

	g_object_class_install_property(gobject_class, PROP_MAX,
			g_param_spec_double("max",
					"DScale maximum value",
					"The maximum value",
					-G_MAXDOUBLE, G_MAXDOUBLE, 0.0,
					G_PARAM_READWRITE));

	g_object_class_install_property(gobject_class, PROP_INTERVAL,
			g_param_spec_double("interval",
					"Interval",
					"Interval",
					G_MINDOUBLE, G_MAXDOUBLE, 1.0,
					G_PARAM_READWRITE));

}
