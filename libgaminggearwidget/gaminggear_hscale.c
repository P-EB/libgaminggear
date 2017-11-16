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

#include "gaminggear/gaminggear_hscale.h"

#define GAMINGGEAR_HSCALE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), GAMINGGEAR_HSCALE_TYPE, GaminggearHScalePrivate))

struct _GaminggearHScalePrivate {
	GtkHScale *scale;
	GtkSpinButton *spin;
	gulong scale_handler;
	gulong spin_handler;
	gdouble min;
	gdouble max;
	gdouble step;
};

enum {
	PROP_0,
	PROP_DIGITS,
	PROP_VALUE,
	PROP_MIN,
	PROP_MAX,
	PROP_STEP,
};

G_DEFINE_TYPE(GaminggearHScale, gaminggear_hscale, GTK_TYPE_HBOX);

static void show_spin(GaminggearHScale *hscale) {
	GaminggearHScalePrivate *priv = hscale->priv;
	if (!gtk_widget_get_parent(GTK_WIDGET(priv->spin)))
		gtk_box_pack_start(GTK_BOX(hscale), GTK_WIDGET(priv->spin), FALSE, FALSE, 0);
}

static void hide_spin(GaminggearHScale *hscale) {
	GaminggearHScalePrivate *priv = hscale->priv;
	if (gtk_widget_get_parent(GTK_WIDGET(priv->spin)))
		gtk_container_remove(GTK_CONTAINER(hscale), GTK_WIDGET(hscale->priv->spin));
}

void gaminggear_hscale_set_draw_spin(GaminggearHScale *hscale, gboolean draw_spin) {
	if (draw_spin)
		show_spin(hscale);
	else
		hide_spin(hscale);
}

guint gaminggear_hscale_get_digits(GaminggearHScale *hscale) {
	return gtk_spin_button_get_digits(hscale->priv->spin);
}

void gaminggear_hscale_set_digits(GaminggearHScale *hscale, guint digits) {
	gtk_spin_button_set_digits(hscale->priv->spin, digits);
}

void gaminggear_hscale_add_mark(GaminggearHScale *hscale, gdouble value, GtkPositionType position, gchar const *markup) {
	gtk_scale_add_mark(GTK_SCALE(hscale->priv->scale), value, position, markup);
}

void gaminggear_hscale_clear_marks(GaminggearHScale *hscale) {
	gtk_scale_clear_marks(GTK_SCALE(hscale->priv->scale));
}

gdouble gaminggear_hscale_get_value(GaminggearHScale *hscale) {
	return gtk_spin_button_get_value(hscale->priv->spin);
}

static gdouble clamp(gdouble value, gdouble range) {
	gboolean negative = (value < 0);
	return (gdouble)(gint)(value / range + (negative ? -0.5 : 0.5)) * range;
}

void gaminggear_hscale_set_value(GaminggearHScale *hscale, gdouble value) {
	GaminggearHScalePrivate *priv = hscale->priv;

	value = clamp(value, priv->step);

	g_signal_handler_block(G_OBJECT(priv->scale), priv->scale_handler);
	gtk_range_set_value(GTK_RANGE(priv->scale), value);
	g_signal_handler_unblock(G_OBJECT(priv->scale), priv->scale_handler);

	g_signal_handler_block(G_OBJECT(priv->spin), priv->spin_handler);
	gtk_spin_button_set_value(priv->spin, value);
	g_signal_handler_unblock(G_OBJECT(priv->spin), priv->spin_handler);

	g_signal_emit_by_name((gpointer)hscale, "value-changed");
}

void gaminggear_hscale_set_range(GaminggearHScale *hscale, gdouble min, gdouble max) {
	GaminggearHScalePrivate *priv = hscale->priv;
	priv->min = min;
	priv->max = max;
	gtk_spin_button_set_range(priv->spin, min, max);
	gtk_range_set_range(GTK_RANGE(priv->scale), min, max);
}

void gaminggear_hscale_set_increment(GaminggearHScale *hscale, gdouble step) {
	GaminggearHScalePrivate *priv = hscale->priv;
	priv->step = step;
	gtk_spin_button_set_increments(priv->spin, step, step);
	gtk_range_set_increments(GTK_RANGE(priv->scale), step, step);
}

GtkWidget *gaminggear_hscale_new_with_range(gdouble min, gdouble max, gdouble step) {
	GaminggearHScale *hscale;

	hscale = g_object_new(GAMINGGEAR_HSCALE_TYPE,
			"min", min,
			"max", max,
			"step", step,
			NULL);

	return GTK_WIDGET(hscale);
}

static void gaminggear_hscale_set_property(GObject *object, guint prop_id, GValue const *value, GParamSpec *pspec) {
	GaminggearHScale *hscale = GAMINGGEAR_HSCALE(object);

	switch(prop_id) {
	case PROP_VALUE:
		gaminggear_hscale_set_value(hscale, g_value_get_double(value));
		break;
	case PROP_DIGITS:
		gaminggear_hscale_set_digits(hscale, g_value_get_uint(value));
		break;
	case PROP_MIN:
		gaminggear_hscale_set_range(hscale, g_value_get_double(value), hscale->priv->max);
		break;
	case PROP_MAX:
		gaminggear_hscale_set_range(hscale, hscale->priv->min, g_value_get_double(value));
		break;
	case PROP_STEP:
		gaminggear_hscale_set_increment(hscale, g_value_get_double(value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	}
}

static void gaminggear_hscale_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec) {
	GaminggearHScale *hscale = GAMINGGEAR_HSCALE(object);

	switch(prop_id) {
	case PROP_VALUE:
		g_value_set_double(value, gaminggear_hscale_get_value(hscale));
		break;
	case PROP_DIGITS:
		g_value_set_uint(value, gaminggear_hscale_get_digits(hscale));
		break;
	case PROP_MIN:
		g_value_set_double(value, hscale->priv->min);
		break;
	case PROP_MAX:
		g_value_set_double(value, hscale->priv->max);
		break;
	case PROP_STEP:
		g_value_set_double(value, hscale->priv->step);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
	}
}

static void gaminggear_hscale_scale_value_changed_cb(GtkRange *range, gpointer user_data) {
	gaminggear_hscale_set_value(GAMINGGEAR_HSCALE(user_data), gtk_range_get_value(range));
}

static void gaminggear_hscale_spin_value_changed_cb(GtkSpinButton *spinbutton, gpointer user_data) {
	gaminggear_hscale_set_value(GAMINGGEAR_HSCALE(user_data), gtk_spin_button_get_value(spinbutton));
}

/*
 * g_object_bin_property() would be a better way to link both together but it's
 * only available since gobject-2.26
 */
static void gaminggear_hscale_init(GaminggearHScale *hscale) {
	GaminggearHScalePrivate *priv = GAMINGGEAR_HSCALE_GET_PRIVATE(hscale);

	hscale->priv = priv;

	priv->min = -G_MAXDOUBLE;
	priv->max = G_MAXDOUBLE;
	priv->step = 1.0;

	priv->scale = GTK_HSCALE(gtk_hscale_new_with_range(priv->min, priv->max, priv->step));
	g_object_ref(G_OBJECT(priv->scale));
	gtk_scale_set_draw_value(GTK_SCALE(priv->scale), FALSE);
	gtk_box_pack_start(GTK_BOX(hscale), GTK_WIDGET(priv->scale), TRUE, TRUE, 0);
	priv->scale_handler = g_signal_connect(G_OBJECT(priv->scale), "value-changed", G_CALLBACK(gaminggear_hscale_scale_value_changed_cb), hscale);

	priv->spin = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(priv->min, priv->max, priv->step));
	g_object_ref(G_OBJECT(priv->spin));
	show_spin(hscale);
	priv->spin_handler = g_signal_connect(G_OBJECT(priv->spin), "value-changed", G_CALLBACK(gaminggear_hscale_spin_value_changed_cb), hscale);
}

static void gaminggear_hscale_finalize(GObject *object) {
	GaminggearHScalePrivate *priv = GAMINGGEAR_HSCALE(object)->priv;

	g_object_unref(G_OBJECT(priv->scale));
	g_object_unref(G_OBJECT(priv->spin));
	G_OBJECT_CLASS(gaminggear_hscale_parent_class)->finalize(object);
}

static void gaminggear_hscale_class_init(GaminggearHScaleClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = (GObjectClass*)klass;

	gobject_class->set_property = gaminggear_hscale_set_property;
	gobject_class->get_property = gaminggear_hscale_get_property;
	gobject_class->finalize = gaminggear_hscale_finalize;

	g_type_class_add_private(klass, sizeof(GaminggearHScalePrivate));

	g_signal_new("value-changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

	g_object_class_install_property(gobject_class, PROP_VALUE,
			g_param_spec_double("value",
					"value",
					"Reads or sets value",
					-G_MAXDOUBLE, G_MAXDOUBLE, 0.0,
					G_PARAM_READWRITE));

	g_object_class_install_property(gobject_class, PROP_DIGITS,
			g_param_spec_uint("digits",
					"digits",
					"Number of decimal places to display",
					0, 20, 0,
					G_PARAM_READWRITE));

	g_object_class_install_property(gobject_class, PROP_MIN,
			g_param_spec_double("min",
					"min",
					"Reads or sets min",
					-G_MAXDOUBLE, G_MAXDOUBLE, -G_MAXDOUBLE,
					G_PARAM_READWRITE));

	g_object_class_install_property(gobject_class, PROP_MAX,
			g_param_spec_double("max",
					"max",
					"Reads or sets max",
					-G_MAXDOUBLE, G_MAXDOUBLE, G_MAXDOUBLE,
					G_PARAM_READWRITE));

	g_object_class_install_property(gobject_class, PROP_STEP,
			g_param_spec_double("step",
					"step",
					"Reads or sets step",
					-G_MAXDOUBLE, G_MAXDOUBLE, 1.0,
					G_PARAM_READWRITE));
}
