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

#include "gaminggear/gaminggear_timeout_dialog.h"
#include "gaminggear/gaminggear_timeout_bar.h"

#define GAMINGGEAR_TIMEOUT_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GAMINGGEAR_TIMEOUT_DIALOG_TYPE, GaminggearTimeoutDialogClass))
#define IS_GAMINGGEAR_TIMEOUT_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GAMINGGEAR_TIMEOUT_DIALOG_TYPE))
#define GAMINGGEAR_TIMEOUT_DIALOG_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), GAMINGGEAR_TIMEOUT_DIALOG_TYPE, GaminggearTimeoutDialogPrivate))

typedef struct _GaminggearTimeoutDialogClass GaminggearTimeoutDialogClass;

struct _GaminggearTimeoutDialogClass {
	GtkDialogClass parent_class;
};

struct _GaminggearTimeoutDialogPrivate {
	GtkLabel *text_label;
	GaminggearTimeoutBar *timeout_bar;
};

G_DEFINE_TYPE(GaminggearTimeoutDialog, gaminggear_timeout_dialog, GTK_TYPE_DIALOG);

static guint const interval = 200; /* msecs */

gboolean gaminggear_timeout_dialog(GtkWindow *parent, gchar const *title, gchar const *text, guint timeout) {
	GaminggearTimeoutDialog *timeout_dialog;
	gboolean retval;

	timeout_dialog = GAMINGGEAR_TIMEOUT_DIALOG(gaminggear_timeout_dialog_new(parent, title, text));
	retval = gaminggear_timeout_dialog_run(timeout_dialog, timeout);
	gtk_widget_destroy(GTK_WIDGET(timeout_dialog));

	return retval;
}

gboolean gaminggear_timeout_dialog_run(GaminggearTimeoutDialog *timeout_dialog, guint timeout) {
	GaminggearTimeoutDialogPrivate *priv = timeout_dialog->priv;
	gboolean retval;

	gaminggear_timeout_bar_start(priv->timeout_bar, interval, timeout);
	retval = gtk_dialog_run(GTK_DIALOG(timeout_dialog)) == GTK_RESPONSE_ACCEPT ? TRUE : FALSE;
	gaminggear_timeout_bar_stop(priv->timeout_bar);

	return retval;
}

GtkWidget *gaminggear_timeout_dialog_new(GtkWindow *parent, gchar const *title, gchar const *text) {
	GaminggearTimeoutDialog *timeout_dialog;

	timeout_dialog = g_object_new(GAMINGGEAR_TIMEOUT_DIALOG_TYPE, NULL);

	gtk_dialog_add_buttons(GTK_DIALOG(timeout_dialog),
			GTK_STOCK_YES, GTK_RESPONSE_ACCEPT,
			GTK_STOCK_NO, GTK_RESPONSE_CANCEL,
			NULL);
	gtk_window_set_title(GTK_WINDOW(timeout_dialog), title);
	gtk_window_set_transient_for(GTK_WINDOW(timeout_dialog), parent);
	gtk_window_set_modal(GTK_WINDOW(timeout_dialog), TRUE);
	gtk_window_set_destroy_with_parent(GTK_WINDOW(timeout_dialog), TRUE);

	gtk_label_set_text(timeout_dialog->priv->text_label, text);

	return GTK_WIDGET(timeout_dialog);
}

static void timeout_cb(GaminggearTimeoutBar *timeout_bar, gpointer user_data) {
	gtk_dialog_response(GTK_DIALOG(user_data), GTK_RESPONSE_CANCEL);
}

static void gaminggear_timeout_dialog_init(GaminggearTimeoutDialog *timeout_dialog) {
	GaminggearTimeoutDialogPrivate *priv = GAMINGGEAR_TIMEOUT_DIALOG_GET_PRIVATE(timeout_dialog);
	GtkVBox *content_area;

	timeout_dialog->priv = priv;

	content_area = GTK_VBOX(gtk_dialog_get_content_area(GTK_DIALOG(timeout_dialog)));

	priv->text_label = GTK_LABEL(gtk_label_new(NULL));
	gtk_box_pack_start(GTK_BOX(content_area), GTK_WIDGET(priv->text_label), FALSE, FALSE, 0);

	priv->timeout_bar = GAMINGGEAR_TIMEOUT_BAR(gaminggear_timeout_bar_new());
	gtk_box_pack_start(GTK_BOX(content_area), GTK_WIDGET(priv->timeout_bar), FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(priv->timeout_bar), "timeout", G_CALLBACK(timeout_cb), timeout_dialog);

	gtk_widget_show_all(GTK_WIDGET(content_area));
}

static void gaminggear_timeout_dialog_class_init(GaminggearTimeoutDialogClass *klass) {
	g_type_class_add_private(klass, sizeof(GaminggearTimeoutDialogPrivate));
}
