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

#include "gaminggear_dialogs.h"
#include "i18n-lib.h"

static gint gaminggear_save_dialog(GtkWindow *parent, gchar const *text, gboolean cancel) {
	GtkWidget *dialog, *label1, *label2, *image, *hbox, *vbox;
	gint retval;

	dialog = gtk_dialog_new_with_buttons(_("Warning"),
			parent, GTK_DIALOG_MODAL,
			GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
			GTK_STOCK_DISCARD, GTK_RESPONSE_REJECT,
			NULL
	);

	if (cancel)
		gtk_dialog_add_button(GTK_DIALOG(dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);

	label1 = gtk_label_new(text);
	label2 = gtk_label_new(_("Do you want to save?"));
	image = gtk_image_new_from_stock(GTK_STOCK_DIALOG_WARNING, GTK_ICON_SIZE_DIALOG);
	vbox = gtk_vbox_new(FALSE, 5);
	hbox = gtk_hbox_new(FALSE, 5);
	gtk_container_set_border_width(GTK_CONTAINER(hbox), 10);
	gtk_box_pack_start(GTK_BOX(hbox), image, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), vbox, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), label1, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), label2, FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), hbox, FALSE, FALSE, 0);
	gtk_widget_show_all(dialog);

	retval = gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
	return retval;
}

gint gaminggear_save_unsaved_dialog(GtkWindow *parent, gboolean cancel) {
	return gaminggear_save_dialog(parent, _("There is unsaved data."), cancel);
}

void gaminggear_warning_dialog(GtkWindow *parent, gchar const *first_text, gchar const *second_text) {
	GtkWidget *dialog;
	dialog = gtk_message_dialog_new(parent, GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK, "%s", first_text);
	if (second_text)
		gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog), "%s", second_text);
	gtk_window_set_title(GTK_WINDOW(dialog), _("Warning"));
	(void)gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}
