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

#include "gaminggear_dscale.h"

#define GAMINGGEAR_DSCALE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GAMINGGEAR_DSCALE_TYPE, GaminggearDscaleClass))
#define IS_GAMINGGEAR_DSCALE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GAMINGGEAR_DSCALE_TYPE))
#define GAMINGGEAR_DSCALE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), GAMINGGEAR_DSCALE_TYPE, GaminggearDscalePrivate))

typedef struct _GaminggearDscaleClass GaminggearDscaleClass;

struct _GaminggearDscaleClass {
	GtkWidgetClass parent_class;
	void(*min_changed)(GaminggearDscale *scale);
	void(*max_changed)(GaminggearDscale *scale);
	void(*lower_changed)(GaminggearDscale *scale);
	void(*upper_changed)(GaminggearDscale *scale);
};

typedef enum {
	CAPTURED_NONE,
	CAPTURED_LOWER,
	CAPTURED_UPPER,
} captured;

struct _GaminggearDscalePrivate {
	gdouble min, max, lower, upper;

	/*
	 * If TRUE min or max stay fixed
	 * else they are expanded if needed
	 */
	gboolean fix_min, fix_max;

	guint gripping_width;
	guint slowdown;
	captured actual_capture;
	GdkCursor *cursor_double_arrow;
};

enum {
	PROP_0,
	PROP_MIN,
	PROP_MAX,
	PROP_LOWER,
	PROP_UPPER,
	PROP_FIX_MIN,
	PROP_FIX_MAX,
	PROP_GRIPPING_WIDTH,
	PROP_SLOWDOWN,
};

G_DEFINE_TYPE(GaminggearDscale, gaminggear_dscale, GTK_TYPE_WIDGET);

GtkWidget *gaminggear_dscale_new(void) {
	return GTK_WIDGET(g_object_new(GAMINGGEAR_DSCALE_TYPE, NULL));
}

static void emit_signals(GtkWidget *widget, gboolean emit_min, gboolean emit_lower, gboolean emit_upper, gboolean emit_max) {
	if (emit_min)
		g_signal_emit_by_name((gpointer)widget, "min-changed");
	if (emit_lower)
		g_signal_emit_by_name((gpointer)widget, "lower-changed");
	if (emit_upper)
		g_signal_emit_by_name((gpointer)widget, "upper-changed");
	if (emit_max)
		g_signal_emit_by_name((gpointer)widget, "max-changed");
}

static glong value_to_pixel(GtkWidget *widget, gdouble value) {
	GaminggearDscale *scale;
	GaminggearDscalePrivate *priv;
	GtkAllocation allocation;

	scale = GAMINGGEAR_DSCALE(widget);
	priv = scale->priv;
	gtk_widget_get_allocation(widget, &allocation);
	return (gdouble)allocation.width * (value - priv->min) / (priv->max - priv->min);
}

// TODO prevent overflow (long back to float and compare with pixel)
static gdouble pixel_to_value(GtkWidget *widget, glong pixel) {
	GaminggearDscale *scale;
	GaminggearDscalePrivate *priv;
	GtkAllocation allocation;

	scale = GAMINGGEAR_DSCALE(widget);
	priv = scale->priv;
	gtk_widget_get_allocation(widget, &allocation);
	return priv->min + (gdouble)pixel * (priv->max - priv->min) / (gdouble)allocation.width;
}

static void draw_range(GtkWidget *widget) {
	GaminggearDscale *scale;
	GaminggearDscalePrivate *priv;
	glong start, lower_pixel, upper_pixel, width;
	GtkStateType state;
	GtkAllocation allocation;
	GdkWindow *window;
	cairo_t *cr;

	window = gtk_widget_get_window(widget);

	if (!GDK_IS_DRAWABLE(window))
		return;

	scale = GAMINGGEAR_DSCALE(widget);
	priv = scale->priv;
	gtk_widget_get_allocation(widget, &allocation);

	gdk_window_clear_area(window, 0, 0, allocation.width, allocation.height);

	if (priv->max != priv->min) {

		lower_pixel = value_to_pixel(widget, priv->lower);
		upper_pixel = value_to_pixel(widget, priv->upper);

		/* draw minimum a small line that is always visible*/
		start = MIN(lower_pixel, allocation.width - 1);
		width = MAX(upper_pixel - lower_pixel, 1);

		state = gtk_widget_get_state(widget);
		
		cr = gdk_cairo_create(window);
		cairo_set_line_width(cr, 1.0);
		cairo_set_line_cap(cr, CAIRO_LINE_CAP_SQUARE);
		gdk_cairo_set_source_color(cr, &gtk_widget_get_style(widget)->fg[state]);
		cairo_rectangle(cr, start, 0, width, allocation.height);
		cairo_fill(cr);
		cairo_destroy(cr);
	}
}

void gaminggear_dscale_set_min(GaminggearDscale *scale, gdouble new_value) {
	GaminggearDscalePrivate *priv = scale->priv;
	gboolean emit_min = FALSE;
	gboolean emit_lower = FALSE;
	gboolean emit_upper = FALSE;
	gboolean emit_max = FALSE;

	if (new_value != priv->min) {
		priv->min = new_value;
		emit_min = TRUE;
	}

	if (new_value > priv->lower) {
		priv->lower = new_value;
		emit_lower = TRUE;
	}

	if (new_value > priv->upper) {
		priv->upper = new_value;
		emit_upper = TRUE;
	}

	if (new_value > priv->max) {
		priv->max = new_value;
		emit_max = TRUE;
	}

	emit_signals(GTK_WIDGET(scale), emit_min, emit_lower, emit_upper, emit_max);

	draw_range(GTK_WIDGET(scale));
}

gdouble gaminggear_dscale_get_min(GaminggearDscale *scale) {
	return scale->priv->min;
}

void gaminggear_dscale_set_lower(GaminggearDscale *scale, gdouble new_value) {
	GaminggearDscalePrivate *priv = scale->priv;
	gboolean emit_min = FALSE;
	gboolean emit_lower = FALSE;
	gboolean emit_upper = FALSE;
	gboolean emit_max = FALSE;

	if (new_value < priv->min) {
		priv->min = new_value;
		emit_min = TRUE;
	}

	if (new_value != priv->lower) {
		priv->lower = new_value;
		emit_lower = TRUE;
	}

	if (new_value > priv->upper) {
		priv->upper = new_value;
		emit_upper = TRUE;
	}

	if (new_value > priv->max) {
		priv->max = new_value;
		emit_max = TRUE;
	}

	emit_signals(GTK_WIDGET(scale), emit_min, emit_lower, emit_upper, emit_max);

	draw_range(GTK_WIDGET(scale));
}

gdouble gaminggear_dscale_get_lower(GaminggearDscale *scale) {
	return scale->priv->lower;
}

void gaminggear_dscale_set_upper(GaminggearDscale *scale, gdouble new_value) {
	GaminggearDscalePrivate *priv = scale->priv;
	gboolean emit_min = FALSE;
	gboolean emit_lower = FALSE;
	gboolean emit_upper = FALSE;
	gboolean emit_max = FALSE;

	if (new_value < priv->min) {
		priv->min = new_value;
		emit_min = TRUE;
	}

	if (new_value < priv->lower) {
		priv->lower = new_value;
		emit_lower = TRUE;
	}

	if (new_value != priv->upper) {
		priv->upper = new_value;
		emit_upper = TRUE;
	}

	if (new_value > priv->max) {
		priv->max = new_value;
		emit_max = TRUE;
	}

	emit_signals(GTK_WIDGET(scale), emit_min, emit_lower, emit_upper, emit_max);

	draw_range(GTK_WIDGET(scale));
}

gdouble gaminggear_dscale_get_upper(GaminggearDscale *scale) {
	return scale->priv->upper;
}

void gaminggear_dscale_set_max(GaminggearDscale *scale, gdouble new_value) {
	GaminggearDscalePrivate *priv = scale->priv;
	gboolean emit_min = FALSE;
	gboolean emit_lower = FALSE;
	gboolean emit_upper = FALSE;
	gboolean emit_max = FALSE;

	if (new_value < priv->min) {
		priv->min = new_value;
		emit_min = TRUE;
	}

	if (new_value < priv->lower) {
		priv->lower = new_value;
		emit_lower = TRUE;
	}

	if (new_value < priv->upper) {
		priv->upper = new_value;
		emit_upper = TRUE;
	}

	if (new_value != priv->max) {
		priv->max = new_value;
		emit_max = TRUE;
	}

	emit_signals(GTK_WIDGET(scale), emit_min, emit_lower, emit_upper, emit_max);

	draw_range(GTK_WIDGET(scale));
}

gdouble gaminggear_dscale_get_max(GaminggearDscale *scale) {
	return scale->priv->max;
}

void gaminggear_dscale_set_fix_min(GaminggearDscale *scale, gboolean new_value) {
	scale->priv->fix_min = new_value;
}

gboolean gaminggear_dscale_get_fix_min(GaminggearDscale *scale) {
	return scale->priv->fix_min;
}

void gaminggear_dscale_set_fix_max(GaminggearDscale *scale, gboolean new_value) {
	scale->priv->fix_max = new_value;
}

gboolean gaminggear_dscale_get_fix_max(GaminggearDscale *scale) {
	return scale->priv->fix_max;
}

void gaminggear_dscale_set_gripping_width(GaminggearDscale *scale, guint new_value) {
	scale->priv->gripping_width = new_value;
}

guint gaminggear_dscale_get_gripping_width(GaminggearDscale *scale) {
	return scale->priv->gripping_width;
}

void gaminggear_dscale_set_slowdown(GaminggearDscale *scale, guint new_value) {
	scale->priv->slowdown = new_value;
}

guint gaminggear_dscale_get_slowdown(GaminggearDscale *scale) {
	return scale->priv->slowdown;
}

static void gaminggear_dscale_set_property(GObject *object, guint prop_id, GValue const *value, GParamSpec *pspec) {
	GaminggearDscale *scale = GAMINGGEAR_DSCALE(object);
	switch(prop_id) {
	case PROP_MIN:
		gaminggear_dscale_set_min(scale, g_value_get_double(value));
		break;
	case PROP_MAX:
		gaminggear_dscale_set_max(scale, g_value_get_double(value));
		break;
	case PROP_LOWER:
		gaminggear_dscale_set_lower(scale, g_value_get_double(value));
		break;
	case PROP_UPPER:
		gaminggear_dscale_set_upper(scale, g_value_get_double(value));
		break;
	case PROP_FIX_MIN:
		gaminggear_dscale_set_fix_min(scale, g_value_get_boolean(value));
		break;
	case PROP_FIX_MAX:
		gaminggear_dscale_set_fix_max(scale, g_value_get_boolean(value));
		break;
	case PROP_GRIPPING_WIDTH:
		gaminggear_dscale_set_gripping_width(scale, g_value_get_uint(value));
		break;
	case PROP_SLOWDOWN:
		gaminggear_dscale_set_slowdown(scale, g_value_get_uint(value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static void gaminggear_dscale_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec) {
	GaminggearDscale *scale = GAMINGGEAR_DSCALE(object);
	GaminggearDscalePrivate *priv = scale->priv;

	switch(prop_id) {
	case PROP_MIN:
		g_value_set_double(value, priv->min);
		break;
	case PROP_MAX:
		g_value_set_double(value, priv->max);
		break;
	case PROP_LOWER:
		g_value_set_double(value, priv->lower);
		break;
	case PROP_UPPER:
		g_value_set_double(value, priv->upper);
		break;
	case PROP_FIX_MIN:
		g_value_set_boolean(value, priv->fix_min);
		break;
	case PROP_FIX_MAX:
		g_value_set_boolean(value, priv->fix_max);
		break;
	case PROP_GRIPPING_WIDTH:
		g_value_set_uint(value, priv->gripping_width);
		break;
	case PROP_SLOWDOWN:
		g_value_set_uint(value, priv->slowdown);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static void gaminggear_dscale_realize(GtkWidget *widget)  {
	GaminggearDscale *scale;
	GdkWindowAttr attributes;
	gint attr_mask;
	GtkAllocation allocation;
	GdkWindow *window;

	g_return_if_fail(widget != NULL);
	g_return_if_fail(IS_GAMINGGEAR_DSCALE(widget));

	gtk_widget_set_realized(widget, TRUE);
	scale = GAMINGGEAR_DSCALE(widget);
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
	gdk_window_set_user_data(window, scale);

	gtk_widget_style_attach(widget);
	gtk_style_set_background(gtk_widget_get_style(widget), window, GTK_STATE_NORMAL);
	gdk_window_show(window);
}

static gint gaminggear_dscale_expose(GtkWidget *widget, GdkEventExpose *event) {
	g_return_val_if_fail(widget != NULL || event != NULL, FALSE);
	g_return_val_if_fail(IS_GAMINGGEAR_DSCALE(widget), FALSE);

	if (event->count > 0)
		return TRUE;

	draw_range(widget);

	return TRUE;
}

// TODO height
static void gaminggear_dscale_size_request(GtkWidget *widget, GtkRequisition *requisition) {
	PangoFontDescription *fd;
	GtkStyle *style;

	g_return_if_fail(widget != NULL || requisition != NULL);
	g_return_if_fail(IS_GAMINGGEAR_DSCALE(widget));

	requisition->width = 100;

	style = gtk_widget_get_style(widget);
	fd = style->font_desc;
	requisition->height = pango_font_description_get_size(fd) / PANGO_SCALE * 2;
}

static void gaminggear_dscale_size_allocate(GtkWidget *widget, GtkAllocation *allocation) {
	g_return_if_fail(widget != NULL || allocation != NULL);
	g_return_if_fail(IS_GAMINGGEAR_DSCALE(widget));

	gtk_widget_set_allocation(widget, allocation);

	if (gtk_widget_get_realized(widget))
		gdk_window_move_resize(gtk_widget_get_window(widget), allocation->x, allocation->y, allocation->width, allocation->height);
}

static captured is_in_capture_position(GtkWidget *widget, glong x) {
	GaminggearDscale *scale = GAMINGGEAR_DSCALE(widget);
	GaminggearDscalePrivate *priv = scale->priv;
	glong dist_lower, dist_upper;
	glong lower, upper;

	/* compare pixel positions */
	lower =value_to_pixel(widget, priv->lower);
	upper = value_to_pixel(widget, priv->upper);
	dist_lower = x - lower;
	dist_upper = x - upper;

	if (ABS(dist_upper) <= (glong)priv->gripping_width && ABS(dist_upper) < ABS(dist_lower))
		return CAPTURED_UPPER;

	if (ABS(dist_lower) <= (glong)priv->gripping_width && ABS(dist_lower) < ABS(dist_upper))
		return CAPTURED_LOWER;

	if (ABS(dist_upper) <= (glong)priv->gripping_width && dist_upper >= 0)
		return CAPTURED_UPPER;

	if (ABS(dist_lower) <= (glong)priv->gripping_width && dist_lower < 0)
		return CAPTURED_LOWER;

	return CAPTURED_NONE;
}

static gboolean button_press_callback(GtkWidget *widget, GdkEventButton *event, gpointer dummy) {
	GaminggearDscale *scale = GAMINGGEAR_DSCALE(widget);
	GaminggearDscalePrivate *priv = scale->priv;

	if (!gtk_widget_is_sensitive(widget)) {
		priv->actual_capture = CAPTURED_NONE;
		return FALSE;
	}

	priv->actual_capture = is_in_capture_position(widget, event->x);

	/* draw immediately to indicate activation */
	draw_range(widget);

	return FALSE;
}

static gboolean button_release_callback(GtkWidget *widget, GdkEventButton *event, gpointer dummy) {
	GaminggearDscale *scale = GAMINGGEAR_DSCALE(widget);

	scale->priv->actual_capture = CAPTURED_NONE;

	/* draw a last time to indicate release */
	draw_range(widget);

	return FALSE;
}

static void set_cursor(GtkWidget *widget, glong x) {
	GaminggearDscale *scale = GAMINGGEAR_DSCALE(widget);

	if (is_in_capture_position(widget, x) != CAPTURED_NONE)
		gdk_window_set_cursor(gtk_widget_get_window(widget), scale->priv->cursor_double_arrow);
	else
		gdk_window_set_cursor(gtk_widget_get_window(widget), NULL);
}

static gboolean motion_callback(GtkWidget *widget, GdkEventMotion *event, gpointer dummy) {
	GaminggearDscale *scale = GAMINGGEAR_DSCALE(widget);
	GaminggearDscalePrivate *priv = scale->priv;

	gdouble actual, new_lower, new_upper;
	gboolean emit_min = FALSE;
	gboolean emit_max = FALSE;
	gboolean emit_lower = FALSE;
	gboolean emit_upper = FALSE;

	set_cursor(widget, event->x);

	if (priv->actual_capture == CAPTURED_NONE)
		return FALSE;

	actual = pixel_to_value(widget, event->x);

	if (actual < priv->min) {
		if (!priv->fix_min) {
			priv->min -= (priv->min - actual) / priv->slowdown;
			emit_min = TRUE;
		} else
			actual = priv->min;
	}

	if (actual > priv->max) {
		if (!priv->fix_max) {
			priv->max += (actual - priv->max) / priv->slowdown;
			emit_max = TRUE;
		} else
			actual = priv->max;
	}

	if (priv->actual_capture == CAPTURED_LOWER) {
		new_lower = MIN(actual, priv->max);
		new_upper = MIN(MAX(actual, priv->upper), priv->max);
	} else { /* CAPTURED_UPPER */
		new_upper = MAX(actual, priv->min);
		new_lower = MAX(MIN(actual, priv->lower), priv->min);
	}

	if (new_lower != priv->lower) {
		priv->lower = new_lower;
		emit_lower = TRUE;
	}

	if (new_upper != priv->upper) {
		priv->upper = new_upper;
		emit_upper = TRUE;
	}

	/* emit events after all new values are set */
	emit_signals(widget, emit_min, emit_lower, emit_upper, emit_max);

	draw_range(widget);

	return FALSE;
}

static void gaminggear_dscale_init(GaminggearDscale *scale) {
	GaminggearDscalePrivate *priv = GAMINGGEAR_DSCALE_GET_PRIVATE(scale);

	scale->priv = priv;

	priv->min = 0.0;
	priv->max = 0.0;
	priv->lower = 0.0;
	priv->upper = 0.0;
	priv->fix_min = TRUE;
	priv->fix_max = TRUE;
	priv->gripping_width = 5;
	priv->slowdown = 5;
	priv->actual_capture = CAPTURED_NONE;

	// TODO how to unref?
	priv->cursor_double_arrow = gdk_cursor_new(GDK_SB_H_DOUBLE_ARROW);

	gtk_widget_add_events(GTK_WIDGET(scale), GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK);
	g_signal_connect(G_OBJECT(scale), "button-press-event", G_CALLBACK(button_press_callback), NULL);
	g_signal_connect(G_OBJECT(scale), "button-release-event", G_CALLBACK(button_release_callback), NULL);
	g_signal_connect(G_OBJECT(scale), "motion-notify-event", G_CALLBACK(motion_callback), NULL);
}

static void gaminggear_dscale_class_init(GaminggearDscaleClass *klass) {
	GObjectClass *gobject_class;
	GtkWidgetClass *widget_class;

	gobject_class = (GObjectClass*)klass;
	widget_class = (GtkWidgetClass*)klass;

	gobject_class->set_property = gaminggear_dscale_set_property;
	gobject_class->get_property = gaminggear_dscale_get_property;

	widget_class->realize = gaminggear_dscale_realize;
	widget_class->expose_event = gaminggear_dscale_expose;
	widget_class->size_request = gaminggear_dscale_size_request;
	widget_class->size_allocate = gaminggear_dscale_size_allocate;

	g_type_class_add_private(klass, sizeof(GaminggearDscalePrivate));

	g_signal_new("min-changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			G_STRUCT_OFFSET(GaminggearDscaleClass, min_changed),
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

	g_signal_new("max-changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			G_STRUCT_OFFSET(GaminggearDscaleClass, max_changed),
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

	g_signal_new("lower-changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			G_STRUCT_OFFSET(GaminggearDscaleClass, lower_changed),
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

	g_signal_new("upper-changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			G_STRUCT_OFFSET(GaminggearDscaleClass, upper_changed),
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

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

	g_object_class_install_property(gobject_class, PROP_LOWER,
			g_param_spec_double("lower",
					"DScale lower scale value",
					"The lower scale value",
					-G_MAXDOUBLE, G_MAXDOUBLE, 0.0,
					G_PARAM_READWRITE));

	g_object_class_install_property(gobject_class, PROP_UPPER,
			g_param_spec_double("upper",
					"DScale upper scale value",
					"The upper scale value",
					-G_MAXDOUBLE, G_MAXDOUBLE, 0.0,
					G_PARAM_READWRITE));

	g_object_class_install_property(gobject_class, PROP_FIX_MIN,
			g_param_spec_boolean("fix-min",
					"Fixed minimal value",
					"Don't change minimal value",
					TRUE,
					G_PARAM_READWRITE));

	g_object_class_install_property(gobject_class, PROP_FIX_MAX,
			g_param_spec_boolean("fix-max",
					"Fixed maximal value",
					"Don't change maximal value",
					TRUE,
					G_PARAM_READWRITE));

	g_object_class_install_property(gobject_class, PROP_GRIPPING_WIDTH,
			g_param_spec_uint("gripping-width",
					"Gripping width",
					"Pixels a click can be away from a boundary to capture it",
					0, G_MAXUINT, 5,
					G_PARAM_READWRITE));

	g_object_class_install_property(gobject_class, PROP_SLOWDOWN,
			g_param_spec_uint("slowdown",
					"Slowdown of range extension",
					"How slow range extends when moving captured boundary outside of widget",
					0, G_MAXUINT, 5,
					G_PARAM_READWRITE));
}
