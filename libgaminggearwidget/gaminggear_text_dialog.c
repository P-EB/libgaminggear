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

#include "gaminggear/gaminggear_text_dialog.h"
#include <gdk/gdkkeysyms.h>

#define GAMINGGEAR_TEXT_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GAMINGGEAR_TEXT_DIALOG_TYPE, GaminggearTextDialogClass))
#define IS_GAMINGGEAR_TEXT_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GAMINGGEAR_TEXT_DIALOG_TYPE))
#define GAMINGGEAR_TEXT_DIALOG_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), GAMINGGEAR_TEXT_DIALOG_TYPE, GaminggearTextDialogPrivate))

typedef struct _GaminggearTextDialogClass GaminggearTextDialogClass;

struct _GaminggearTextDialogClass {
	GtkDialogClass parent_class;
};

struct _GaminggearTextDialogPrivate {
	GtkEntry *entry;
	GtkLabel *caption;
};

G_DEFINE_TYPE(GaminggearTextDialog, gaminggear_text_dialog, GTK_TYPE_DIALOG);

gchar *gaminggear_text_dialog(GtkWindow *parent, gchar const *title, gchar const *caption, gchar const *text) {
	GaminggearTextDialog *text_dialog;
	gchar *result;

	text_dialog = GAMINGGEAR_TEXT_DIALOG(gaminggear_text_dialog_new(parent, title, caption));
	if (text)
		gaminggear_text_dialog_set_text(text_dialog, text);
	result = gaminggear_text_dialog_run(text_dialog);
	gtk_widget_destroy(GTK_WIDGET(text_dialog));

	return result;
}
gchar *gaminggear_text_dialog_run(GaminggearTextDialog *text_dialog) {
	if (gtk_dialog_run(GTK_DIALOG(text_dialog)) == GTK_RESPONSE_ACCEPT)
		return gaminggear_text_dialog_get_text(text_dialog);
	else
		return NULL;
}

gchar *gaminggear_text_dialog_get_text(GaminggearTextDialog *text_dialog) {
	return g_strdup(gtk_entry_get_text(text_dialog->priv->entry));
}

void gaminggear_text_dialog_set_text(GaminggearTextDialog *text_dialog, gchar const *text) {
	gtk_entry_set_text(text_dialog->priv->entry, text);
}

void gaminggear_text_dialog_set_title(GaminggearTextDialog *text_dialog, gchar const *title) {
	gtk_window_set_title(GTK_WINDOW(text_dialog), title);
}

void gaminggear_text_dialog_set_caption(GaminggearTextDialog *text_dialog, gchar const *caption) {
	gtk_label_set_text(text_dialog->priv->caption, caption);
}

// TODO set ok button sensitivity on validity of string
GtkWidget *gaminggear_text_dialog_new(GtkWindow *parent, gchar const *title, gchar const *caption) {
	GaminggearTextDialog *text_dialog;

	text_dialog = g_object_new(GAMINGGEAR_TEXT_DIALOG_TYPE, NULL);

	gtk_dialog_add_buttons(GTK_DIALOG(text_dialog),
			GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
			GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
			NULL);
	gtk_window_set_transient_for(GTK_WINDOW(text_dialog), parent);
	gtk_window_set_modal(GTK_WINDOW(text_dialog), TRUE);
	gtk_window_set_destroy_with_parent(GTK_WINDOW(text_dialog), TRUE);

	gaminggear_text_dialog_set_title(text_dialog, title);
	gaminggear_text_dialog_set_caption(text_dialog, caption);

	return GTK_WIDGET(text_dialog);
}

/* makes it possible to exit dialog by pressing enter */
static gboolean key_press_cb(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
	GaminggearTextDialog *text_dialog = GAMINGGEAR_TEXT_DIALOG(user_data);
	if (event->type == GDK_KEY_PRESS && event->keyval == GDK_Return)
		gtk_dialog_response(GTK_DIALOG(text_dialog), GTK_RESPONSE_ACCEPT);
	return FALSE; /* propagate */
}

static void gaminggear_text_dialog_init(GaminggearTextDialog *text_dialog) {
	GaminggearTextDialogPrivate *priv = GAMINGGEAR_TEXT_DIALOG_GET_PRIVATE(text_dialog);
	GtkVBox *content_area;

	text_dialog->priv = priv;

	content_area = GTK_VBOX(gtk_dialog_get_content_area(GTK_DIALOG(text_dialog)));

	priv->caption = GTK_LABEL(gtk_label_new(NULL));

	priv->entry = GTK_ENTRY(gtk_entry_new());

	gtk_box_pack_start(GTK_BOX(content_area), GTK_WIDGET(priv->caption), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(content_area), GTK_WIDGET(priv->entry), FALSE, FALSE, 0);

	g_signal_connect(G_OBJECT(priv->entry), "key-press-event", G_CALLBACK(key_press_cb), text_dialog);

	gtk_widget_show_all(GTK_WIDGET(content_area));
}

static void gaminggear_text_dialog_class_init(GaminggearTextDialogClass *klass) {
	g_type_class_add_private(klass, sizeof(GaminggearTextDialogPrivate));
}
