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

#include "gaminggear/gaminggear_xy_scales.h"
#include "gaminggear/gaminggear_hscale.h"

#define GAMINGGEAR_XY_SCALES_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), GAMINGGEAR_XY_SCALES_TYPE, GaminggearXyScalesPrivate))

struct _GaminggearXyScalesPrivate {
	GtkToggleButton *linked;
	GtkImage *icon;
	GaminggearHScale *x_scale;
	GaminggearHScale *y_scale;
	gulong x_scale_handler;
	gulong y_scale_handler;
	gulong linked_handler;
	gdouble min, max, step;
};

enum {
	PROP_0,
	PROP_MIN,
	PROP_MAX,
	PROP_STEP,
};

G_DEFINE_TYPE(GaminggearXyScales, gaminggear_xy_scales, GTK_TYPE_TABLE);

static void x_to_y(GaminggearXyScales *scales) {
	GaminggearXyScalesPrivate *priv = scales->priv;

	g_signal_handler_block(G_OBJECT(priv->y_scale), priv->y_scale_handler);
	gaminggear_hscale_set_value(priv->y_scale, gaminggear_hscale_get_value(priv->x_scale));
	g_signal_handler_unblock(G_OBJECT(priv->y_scale), priv->y_scale_handler);
}

static void y_to_x(GaminggearXyScales *scales) {
	GaminggearXyScalesPrivate *priv = scales->priv;

	g_signal_handler_block(G_OBJECT(priv->x_scale), priv->x_scale_handler);
	gaminggear_hscale_set_value(priv->x_scale, gaminggear_hscale_get_value(priv->y_scale));
	g_signal_handler_unblock(G_OBJECT(priv->x_scale), priv->x_scale_handler);
}

void gaminggear_xy_scales_set_digits(GaminggearXyScales *scales, guint digits) {
	gaminggear_hscale_set_digits(scales->priv->x_scale, digits);
	gaminggear_hscale_set_digits(scales->priv->y_scale, digits);
}

gdouble gaminggear_xy_scales_get_x_value(GaminggearXyScales *scales) {
	return gaminggear_hscale_get_value(scales->priv->x_scale);
}

void gaminggear_xy_scales_set_x_value(GaminggearXyScales *scales, gdouble value) {
	gaminggear_hscale_set_value(scales->priv->x_scale, value);
}

gdouble gaminggear_xy_scales_get_y_value(GaminggearXyScales *scales) {
	return gaminggear_hscale_get_value(scales->priv->y_scale);
}

void gaminggear_xy_scales_set_y_value(GaminggearXyScales *scales, gdouble value) {
	gaminggear_hscale_set_value(scales->priv->y_scale, value);
}

gboolean gaminggear_xy_scales_get_linked(GaminggearXyScales *scales) {
	return gtk_toggle_button_get_active(scales->priv->linked);
}

static void update_link_icon(GaminggearXyScales *scales) {
	GaminggearXyScalesPrivate *priv = scales->priv;
	if (gtk_toggle_button_get_active(priv->linked))
		gtk_image_set_from_icon_name(GTK_IMAGE(priv->icon), "gaminggear-link", GTK_ICON_SIZE_BUTTON);
	else
		gtk_image_set_from_icon_name(GTK_IMAGE(priv->icon), "gaminggear-link_break", GTK_ICON_SIZE_BUTTON);
}

void gaminggear_xy_scales_set_linked(GaminggearXyScales *scales, gboolean value) {
	GaminggearXyScalesPrivate *priv = scales->priv;
	gtk_toggle_button_set_active(priv->linked, value);
	update_link_icon(scales);
	if (value)
		x_to_y(scales);
}

static GaminggearHScale *scale_new(gdouble min, gdouble max, gdouble step) {
	GaminggearHScale *scale;
	gdouble i;

	scale = GAMINGGEAR_HSCALE(gaminggear_hscale_new_with_range(min, max, step));

	for (i = min; i <= max; i += step)
		gaminggear_hscale_add_mark(scale, i, GTK_POS_BOTTOM, NULL);
	return scale;
}

static void x_value_changed_cb(GaminggearHScale *scale, gpointer user_data) {
	GaminggearXyScales *scales = GAMINGGEAR_XY_SCALES(user_data);

	if (gaminggear_xy_scales_get_linked(scales))
		x_to_y(scales);
}

static void y_value_changed_cb(GaminggearHScale *scale, gpointer user_data) {
	GaminggearXyScales *scales = GAMINGGEAR_XY_SCALES(user_data);

	if (gaminggear_xy_scales_get_linked(scales))
		y_to_x(scales);
}

static void link_toggled_cb(GtkToggleButton *button, gpointer user_data) {
	GaminggearXyScales *scales = GAMINGGEAR_XY_SCALES(user_data);

	if (gtk_toggle_button_get_active(button))
		x_to_y(scales);
	update_link_icon(scales);
}

static GObject *gaminggear_xy_scales_constructor(GType gtype, guint n_properties, GObjectConstructParam *properties) {
	GaminggearXyScalesPrivate *priv;
	GaminggearXyScales *scales;
	GObject *obj;

	obj = G_OBJECT_CLASS(gaminggear_xy_scales_parent_class)->constructor(gtype, n_properties, properties);
	scales = GAMINGGEAR_XY_SCALES(obj);
	priv = scales->priv;

	priv->x_scale = scale_new(priv->min, priv->max, priv->step);
	priv->y_scale = scale_new(priv->min, priv->max, priv->step);
	priv->linked = GTK_TOGGLE_BUTTON(gtk_toggle_button_new());
	priv->icon = GTK_IMAGE(gtk_image_new_from_pixbuf(NULL));

	gtk_container_add(GTK_CONTAINER(priv->linked), GTK_WIDGET(priv->icon));

	gtk_table_attach(GTK_TABLE(scales), gtk_label_new("X"), 0, 1, 0, 1, GTK_SHRINK, GTK_EXPAND, 0, 0);
	gtk_table_attach(GTK_TABLE(scales), gtk_label_new("Y"), 0, 1, 1, 2, GTK_SHRINK, GTK_EXPAND, 0, 0);
	gtk_table_attach(GTK_TABLE(scales), GTK_WIDGET(priv->x_scale), 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);
	gtk_table_attach(GTK_TABLE(scales), GTK_WIDGET(priv->y_scale), 1, 2, 1, 2, GTK_EXPAND | GTK_FILL, GTK_EXPAND, 0, 0);
	gtk_table_attach(GTK_TABLE(scales), GTK_WIDGET(priv->linked), 2, 3, 0, 2, GTK_SHRINK, GTK_EXPAND, 0, 0);

	priv->x_scale_handler = g_signal_connect(G_OBJECT(priv->x_scale), "value-changed", G_CALLBACK(x_value_changed_cb), scales);
	priv->y_scale_handler = g_signal_connect(G_OBJECT(priv->y_scale), "value-changed", G_CALLBACK(y_value_changed_cb), scales);
	priv->linked_handler = g_signal_connect(G_OBJECT(priv->linked), "toggled", G_CALLBACK(link_toggled_cb), scales);

	update_link_icon(scales);

	return obj;
}

GtkWidget *gaminggear_xy_scales_new(gdouble min, gdouble max, gdouble step) {
	GaminggearXyScales *scales;

	scales = g_object_new(GAMINGGEAR_XY_SCALES_TYPE,
			"min", min,
			"max", max,
			"step", step,
			"homogeneous", FALSE,
			"n-columns", 3,
			"n-rows", 2,
			NULL);

	return GTK_WIDGET(scales);
}

static void gaminggear_xy_scales_init(GaminggearXyScales *scales) {
	scales->priv = GAMINGGEAR_XY_SCALES_GET_PRIVATE(scales);
}

static void gaminggear_xy_scales_set_property(GObject *object, guint prop_id, GValue const *value, GParamSpec *pspec) {
	GaminggearXyScalesPrivate *priv = GAMINGGEAR_XY_SCALES(object)->priv;
	switch(prop_id) {
	case PROP_MIN:
		priv->min = g_value_get_double(value);
		break;
	case PROP_MAX:
		priv->max = g_value_get_double(value);
		break;
	case PROP_STEP:
		priv->step = g_value_get_double(value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static void gaminggear_xy_scales_class_init(GaminggearXyScalesClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = (GObjectClass *)klass;

	gobject_class->constructor = gaminggear_xy_scales_constructor;
	gobject_class->set_property = gaminggear_xy_scales_set_property;

	g_type_class_add_private(klass, sizeof(GaminggearXyScalesPrivate));

	g_object_class_install_property(gobject_class, PROP_MIN,
			g_param_spec_double("min",
					"minimum value",
					"The minimum value",
					-G_MAXDOUBLE, G_MAXDOUBLE, 0.0,
					G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));

	g_object_class_install_property(gobject_class, PROP_MAX,
			g_param_spec_double("max",
					"maximum value",
					"The maximum value",
					-G_MAXDOUBLE, G_MAXDOUBLE, 0.0,
					G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));

	g_object_class_install_property(gobject_class, PROP_STEP,
			g_param_spec_double("step",
					"step",
					"Step",
					G_MINDOUBLE, G_MAXDOUBLE, 1.0,
					G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}
