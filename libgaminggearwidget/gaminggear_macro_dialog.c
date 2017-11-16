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

#include "gaminggear/gaminggear_macro_dialog.h"
#include "gaminggear_macro_combo_box.h"
#include "i18n-lib.h"

#define GAMINGGEAR_MACRO_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GAMINGGEAR_MACRO_DIALOG_TYPE, GaminggearMacroDialogClass))
#define IS_GAMINGGEAR_MACRO_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GAMINGGEAR_MACRO_DIALOG_TYPE))
#define GAMINGGEAR_MACRO_DIALOG_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), GAMINGGEAR_MACRO_DIALOG_TYPE, GaminggearMacroDialogPrivate))

typedef struct _GaminggearMacroDialogClass GaminggearMacroDialogClass;

struct _GaminggearMacroDialogClass {
	GtkDialogClass parent_class;
};

struct _GaminggearMacroDialogPrivate {
	GtkWidget *ok_button;
	GtkComboBox *combo_box;
};

G_DEFINE_TYPE(GaminggearMacroDialog, gaminggear_macro_dialog, GTK_TYPE_DIALOG);

static void gaminggear_macro_dialog_set_macros(GaminggearMacroDialog *macro_dialog, GaminggearMacros *macros) {
	gaminggear_macro_combo_box_set_macros(macro_dialog->priv->combo_box, macros);
}

GaminggearMacro *gaminggear_macro_dialog(GtkWindow *parent) {
	GaminggearMacroDialog *macro_dialog;
	GaminggearMacro *macro;

	macro_dialog = GAMINGGEAR_MACRO_DIALOG(gaminggear_macro_dialog_new(parent));
	macro = gaminggear_macro_dialog_run(macro_dialog);
	gtk_widget_destroy(GTK_WIDGET(macro_dialog));

	return macro;
}

static GaminggearMacro *gaminggear_macro_dialog_get_value(GaminggearMacroDialog *macro_dialog) {
	return gaminggear_macro_combo_box_get_value(macro_dialog->priv->combo_box);
}

GaminggearMacro *gaminggear_macro_dialog_run(GaminggearMacroDialog *macro_dialog) {
	GaminggearMacros *macros;

	macros = gaminggear_macros_load();
	gaminggear_macro_dialog_set_macros(macro_dialog, macros);
	gaminggear_macros_free(macros);

	if (gtk_dialog_run(GTK_DIALOG(macro_dialog)) == GTK_RESPONSE_ACCEPT)
		return gaminggear_macro_dialog_get_value(macro_dialog);
	else
		return NULL;
}

GtkWidget *gaminggear_macro_dialog_new(GtkWindow *parent) {
	GaminggearMacroDialog *macro_dialog;
	GaminggearMacroDialogPrivate *priv;

	macro_dialog = g_object_new(GAMINGGEAR_MACRO_DIALOG_TYPE, NULL);
	priv = macro_dialog->priv;

	priv->ok_button = gtk_dialog_add_button(GTK_DIALOG(macro_dialog), GTK_STOCK_OK, GTK_RESPONSE_ACCEPT);
	gtk_widget_set_sensitive(priv->ok_button, FALSE);

	gtk_dialog_add_button(GTK_DIALOG(macro_dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT);

	gtk_window_set_title(GTK_WINDOW(macro_dialog), _("Select macro"));
	gtk_window_set_transient_for(GTK_WINDOW(macro_dialog), parent);
	gtk_window_set_modal(GTK_WINDOW(macro_dialog), TRUE);
	gtk_window_set_destroy_with_parent(GTK_WINDOW(macro_dialog), TRUE);

	return GTK_WIDGET(macro_dialog);
}

static void changed_cb(GtkComboBox *combo_box, gpointer user_data) {
	GaminggearMacroDialog *macro_dialog = GAMINGGEAR_MACRO_DIALOG(user_data);
	gtk_widget_set_sensitive(macro_dialog->priv->ok_button, TRUE);
}

static void gaminggear_macro_dialog_init(GaminggearMacroDialog *macro_dialog) {
	GaminggearMacroDialogPrivate *priv = GAMINGGEAR_MACRO_DIALOG_GET_PRIVATE(macro_dialog);
	GtkVBox *content_area;

	macro_dialog->priv = priv;

	content_area = GTK_VBOX(gtk_dialog_get_content_area(GTK_DIALOG(macro_dialog)));

	priv->combo_box = GTK_COMBO_BOX(gaminggear_macro_combo_box_new());
	g_signal_connect(G_OBJECT(priv->combo_box), "changed", G_CALLBACK(changed_cb), macro_dialog);
	gtk_box_pack_start(GTK_BOX(content_area), GTK_WIDGET(priv->combo_box), FALSE, FALSE, 0);

	gtk_widget_show_all(GTK_WIDGET(content_area));
}

static void gaminggear_macro_dialog_class_init(GaminggearMacroDialogClass *klass) {
	g_type_class_add_private(klass, sizeof(GaminggearMacroDialogPrivate));
}
