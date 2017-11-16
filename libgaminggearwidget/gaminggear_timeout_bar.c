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

#include "gaminggear/gaminggear_timeout_bar.h"

#define GAMINGGEAR_TIMEOUT_BAR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GAMINGGEAR_TIMEOUT_BAR_TYPE, GaminggearTimeoutBarClass))
#define IS_GAMINGGEAR_TIMEOUT_BAR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GAMINGGEAR_TIMEOUT_BAR_TYPE))
#define GAMINGGEAR_TIMEOUT_BAR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), GAMINGGEAR_TIMEOUT_BAR_TYPE, GaminggearTimeoutBarPrivate))

typedef struct _GaminggearTimeoutBarClass GaminggearTimeoutBarClass;
typedef struct _GaminggearTimeoutBarPrivate GaminggearTimeoutBarPrivate;

struct _GaminggearTimeoutBar {
	GtkProgressBar parent;
	GaminggearTimeoutBarPrivate *priv;
};

struct _GaminggearTimeoutBarClass {
	GtkProgressBarClass parent_class;
	void(*timeout)(GaminggearTimeoutBar *timeout_bar);
};

struct _GaminggearTimeoutBarPrivate {
	gdouble fraction; /* msecs */
	guint source;
};

G_DEFINE_TYPE(GaminggearTimeoutBar, gaminggear_timeout_bar, GTK_TYPE_PROGRESS_BAR);

static gboolean update_timeout_bar(GaminggearTimeoutBar *timeout_bar) {
	GaminggearTimeoutBarPrivate *priv = timeout_bar->priv;
	gdouble new_value;

	new_value = gtk_progress_bar_get_fraction(GTK_PROGRESS_BAR(timeout_bar)) - priv->fraction;
	if (new_value <= 0.0) {
		g_signal_emit_by_name((gpointer)timeout_bar, "timeout");
		return FALSE;
	} else {
		gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(timeout_bar), new_value);
		return TRUE;
	}
}

GtkWidget *gaminggear_timeout_bar_new(void) {
	return GTK_WIDGET(g_object_new(GAMINGGEAR_TIMEOUT_BAR_TYPE, NULL));
}

void gaminggear_timeout_bar_start(GaminggearTimeoutBar *timeout_bar, guint interval, guint timeout) {
	GaminggearTimeoutBarPrivate *priv = timeout_bar->priv;

	if (priv->source)
		return;

	priv->fraction = 1.0 / (gdouble)timeout / 1000.0 * (gdouble)interval;
	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(timeout_bar), 1.0);

	priv->source = g_timeout_add(interval, (GSourceFunc)update_timeout_bar, timeout_bar);
}

void gaminggear_timeout_bar_stop(GaminggearTimeoutBar *timeout_bar) {
	GaminggearTimeoutBarPrivate *priv = timeout_bar->priv;

	if (!priv->source)
		return;

	g_source_remove(priv->source);
	priv->source = 0;
}

static void gaminggear_timeout_bar_init(GaminggearTimeoutBar *timeout_bar) {
	GaminggearTimeoutBarPrivate *priv = GAMINGGEAR_TIMEOUT_BAR_GET_PRIVATE(timeout_bar);

	timeout_bar->priv = priv;
}

static void gaminggear_timeout_bar_class_init(GaminggearTimeoutBarClass *klass) {
	g_type_class_add_private(klass, sizeof(GaminggearTimeoutBarPrivate));

	g_signal_new("timeout",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			G_STRUCT_OFFSET(GaminggearTimeoutBarClass, timeout),
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}
