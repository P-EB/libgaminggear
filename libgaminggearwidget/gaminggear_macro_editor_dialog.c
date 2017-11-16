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

#include "gaminggear_macro_editor_dialog_menu_bar.h"
#include "gaminggear_macro_editor.h"
#include "gaminggear_macro_editor_dialog_internal.h"
#include "gaminggear_dialogs.h"
#include "i18n-lib.h"

#define GAMINGGEAR_MACRO_EDITOR_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GAMINGGEAR_MACRO_EDITOR_DIALOG_TYPE, GaminggearMacroEditorDialogClass))
#define IS_GAMINGGEAR_MACRO_EDITOR_DIALOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GAMINGGEAR_MACRO_EDITOR_DIALOG_TYPE))
#define GAMINGGEAR_MACRO_EDITOR_DIALOG_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), GAMINGGEAR_MACRO_EDITOR_DIALOG_TYPE, GaminggearMacroEditorDialogPrivate))

typedef struct _GaminggearMacroEditorDialogClass GaminggearMacroEditorDialogClass;

struct _GaminggearMacroEditorDialogClass {
	GtkDialogClass parent_class;
};

struct _GaminggearMacroEditorDialogPrivate {
	GaminggearMacroEditor *macro_editor;
};

G_DEFINE_TYPE(GaminggearMacroEditorDialog, gaminggear_macro_editor_dialog, GTK_TYPE_DIALOG);

void gaminggear_macro_editor_dialog_clear(GaminggearMacroEditorDialog *dialog) {
	gaminggear_macro_editor_clear(dialog->priv->macro_editor);
}

void gaminggear_macro_editor_dialog_add_macros(GaminggearMacroEditorDialog *dialog, GaminggearMacros *gaminggear_macros) {
	gaminggear_macro_editor_add_macros(dialog->priv->macro_editor, gaminggear_macros);
}

GaminggearMacros *gaminggear_macro_editor_dialog_get_macros(GaminggearMacroEditorDialog *dialog) {
	return gaminggear_macro_editor_get_macros(dialog->priv->macro_editor);
}

static void gaminggear_macro_editor_dialog_load_macros(GaminggearMacroEditorDialog *dialog) {
	GaminggearMacros *gaminggear_macros;

	gaminggear_macro_editor_clear(dialog->priv->macro_editor);

	gaminggear_macros = gaminggear_macros_load();
	gaminggear_macro_editor_add_macros(dialog->priv->macro_editor, gaminggear_macros);
	gaminggear_macros_free(gaminggear_macros);

	gaminggear_macro_editor_set_modified(dialog->priv->macro_editor, FALSE);
}

gboolean gaminggear_macro_editor_dialog_save_macros(GaminggearMacroEditorDialog *dialog) {
	GaminggearMacros *gaminggear_macros;
	gboolean result;
	GError *error = NULL;

	gaminggear_macros = gaminggear_macro_editor_get_macros(dialog->priv->macro_editor);
	result = gaminggear_macros_save(gaminggear_macros, &error);
	gaminggear_macros_free(gaminggear_macros);

	if (error) {
		gaminggear_warning_dialog(GTK_WINDOW(dialog), _("Could not save macros"), error->message);
		g_clear_error(&error);
	} else
		gaminggear_macro_editor_set_modified(dialog->priv->macro_editor, FALSE);

	return result;
}

void gaminggear_macro_editor_dialog_run(GaminggearMacroEditorDialog *dialog) {
	gaminggear_macro_editor_dialog_load_macros(dialog);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
		gaminggear_macro_editor_dialog_save_macros(dialog);
	else {
		if (gaminggear_macro_editor_get_modified(dialog->priv->macro_editor)) {
			if (gaminggear_save_unsaved_dialog(NULL, FALSE) == GTK_RESPONSE_ACCEPT)
				gaminggear_macro_editor_dialog_save_macros(dialog);
		}
	}
}

void gaminggear_macro_editor_dialog(GtkWindow *parent) {
	GaminggearMacroEditorDialog *dialog;

	dialog = GAMINGGEAR_MACRO_EDITOR_DIALOG(gaminggear_macro_editor_dialog_new(parent));
	gaminggear_macro_editor_dialog_run(dialog);
	gtk_widget_destroy(GTK_WIDGET(dialog));
}

GtkWidget *gaminggear_macro_editor_dialog_new(GtkWindow *parent) {
	GaminggearMacroEditorDialog *macro_editor_dialog;

	macro_editor_dialog = g_object_new(GAMINGGEAR_MACRO_EDITOR_DIALOG_TYPE, NULL);

	gtk_dialog_add_buttons(GTK_DIALOG(macro_editor_dialog),
			GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
			GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
			NULL);
	gtk_window_set_title(GTK_WINDOW(macro_editor_dialog), _("Edit macros"));
	gtk_window_set_transient_for(GTK_WINDOW(macro_editor_dialog), parent);
	gtk_window_set_modal(GTK_WINDOW(macro_editor_dialog), TRUE);
	gtk_window_set_destroy_with_parent(GTK_WINDOW(macro_editor_dialog), TRUE);
	gtk_window_set_type_hint(GTK_WINDOW(macro_editor_dialog), GDK_WINDOW_TYPE_HINT_NORMAL);
	gtk_window_resize(GTK_WINDOW(macro_editor_dialog), 640, 480);

	return GTK_WIDGET(macro_editor_dialog);
}

static void gaminggear_macro_editor_dialog_init(GaminggearMacroEditorDialog *macro_editor_dialog) {
	GaminggearMacroEditorDialogPrivate *priv = GAMINGGEAR_MACRO_EDITOR_DIALOG_GET_PRIVATE(macro_editor_dialog);
	GtkVBox *content_area;
	GtkWidget *menu_bar;

	macro_editor_dialog->priv = priv;

	content_area = GTK_VBOX(gtk_dialog_get_content_area(GTK_DIALOG(macro_editor_dialog)));

	priv->macro_editor = GAMINGGEAR_MACRO_EDITOR(gaminggear_macro_editor_new());
	menu_bar = gaminggear_macro_editor_dialog_menu_bar_new(macro_editor_dialog);

	gtk_box_pack_start(GTK_BOX(content_area), menu_bar, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(content_area), GTK_WIDGET(priv->macro_editor), TRUE, TRUE, 0);

	gtk_widget_show_all(menu_bar);
	gtk_widget_show(GTK_WIDGET(priv->macro_editor));
}

static void gaminggear_macro_editor_dialog_class_init(GaminggearMacroEditorDialogClass *klass) {
	g_type_class_add_private(klass, sizeof(GaminggearMacroEditorDialogPrivate));
}
