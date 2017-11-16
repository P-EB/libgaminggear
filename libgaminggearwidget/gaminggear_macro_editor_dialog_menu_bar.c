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

#include "config.h"
#include "gaminggear_configuration.h"
#include "gaminggear_macro_editor_dialog_menu_bar.h"
#include "gaminggear_macro_editor_dialog_internal.h"
#include "gaminggear_about_dialog.h"
#include "gaminggear_dialogs.h"
#include "gaminggear_helper.h"
#include "macros_converter_roccat_arvo.h"
#include "macros_converter_roccat_kone.h"
#include "macros_converter_roccat_swarm.h"
#include "macros_converter_roccat_valo.h"
#include "macros_converter_roccat.h"
#ifdef SQLITE3_FOUND
#include "macros_converter_steelseries_engine3.h"
#endif
#include "i18n-lib.h"

static gchar const * const filter_converter_key = "converter";

static void add_file_filter_from_converter(GtkFileChooser *chooser, GaminggearMacrosConverter const *converter, gboolean import) {
	GtkFileFilter *filter;

	if (import) {
		if (!converter->import) return;
	} else {
		if (!converter->export) return;
	}

	filter = gtk_file_filter_new();
	g_object_set_data(G_OBJECT(filter), filter_converter_key, (gpointer)converter);
	gtk_file_filter_set_name(filter, _N(converter->name));
	gtk_file_filter_add_pattern(filter, converter->pattern);

	gtk_file_chooser_add_filter(chooser, filter);
}

static void add_file_filter(GtkFileChooser *chooser, gboolean import) {
	add_file_filter_from_converter(chooser, macros_converter_roccat(), import);
	add_file_filter_from_converter(chooser, macros_converter_roccat_arvo(), import);
	add_file_filter_from_converter(chooser, macros_converter_roccat_kone(), import);
	add_file_filter_from_converter(chooser, macros_converter_roccat_swarm(), import);
	add_file_filter_from_converter(chooser, macros_converter_roccat_valo(), import);
#ifdef SQLITE3_FOUND
	add_file_filter_from_converter(chooser, macros_converter_steelseries_engine3(), import);
#endif
}

static void show_import_cb(GtkMenuItem *item, gpointer user_data) {
	GaminggearMacroEditorDialog *parent = GAMINGGEAR_MACRO_EDITOR_DIALOG(user_data);
	GaminggearKeyFile *config;
	GtkWidget *dialog;
	GError *error = NULL;
	gchar *filename, *path;
	GtkFileFilter *filter;
	GaminggearMacros *gaminggear_macros;
	GaminggearMacrosConverter const *converter;

	config = gaminggear_configuration_load();

	dialog = gtk_file_chooser_dialog_new(_("Import macros"),
			GTK_WINDOW(parent),
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL
	);

	add_file_filter(GTK_FILE_CHOOSER(dialog), TRUE);

	path = gaminggear_configuration_get_macro_save_path(config);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), path);
	g_free(path);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		filter = gtk_file_chooser_get_filter(GTK_FILE_CHOOSER(dialog));
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

		path = g_path_get_dirname(filename);
		gaminggear_configuration_set_macro_save_path(config, path);
		g_free(path);
		gaminggear_configuration_save(config, &error);

		converter = (GaminggearMacrosConverter const * const)g_object_get_data(G_OBJECT(filter), filter_converter_key);
		gaminggear_macros = converter->import(filename, &error);

		g_free(filename);
		
		if (error) {
			gaminggear_warning_dialog(GTK_WINDOW(parent), _("Could not import macros"), error->message);
			g_clear_error(&error);
		} else {
			gaminggear_macro_editor_dialog_add_macros(parent, gaminggear_macros);
			gaminggear_macros_free(gaminggear_macros);
		}
	}
	gtk_widget_destroy(dialog);
	gaminggear_configuration_free(config);
}

static void show_export_cb(GtkMenuItem *item, gpointer user_data) {
	GaminggearMacroEditorDialog *parent = GAMINGGEAR_MACRO_EDITOR_DIALOG(user_data);
	GaminggearKeyFile *config;
	GtkWidget *dialog;
	GError *error = NULL;
	gchar *filename, *path;
	GtkFileFilter *filter;
	gchar *temp_filename;
	GaminggearMacrosConverter const *converter;
	GaminggearMacros *gaminggear_macros;

	config = gaminggear_configuration_load();

	dialog = gtk_file_chooser_dialog_new(_("Export macros"),
			GTK_WINDOW(parent),
			GTK_FILE_CHOOSER_ACTION_SAVE,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
			NULL
	);

	add_file_filter(GTK_FILE_CHOOSER(dialog), FALSE);

	path = gaminggear_configuration_get_macro_save_path(config);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), path);
	g_free(path);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		filter = gtk_file_chooser_get_filter(GTK_FILE_CHOOSER(dialog));
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

		path = g_path_get_dirname(filename);
		gaminggear_configuration_set_macro_save_path(config, path);
		g_free(path);
		gaminggear_configuration_save(config, &error);

		gaminggear_macros = gaminggear_macro_editor_dialog_get_macros(parent);

		converter = (GaminggearMacrosConverter const * const)g_object_get_data(G_OBJECT(filter), filter_converter_key);
		temp_filename = gaminggear_create_filename_with_extension(filename, converter->file_extension);
		converter->export(temp_filename, gaminggear_macros, &error);
		g_free(temp_filename);
		g_free(filename);
		gaminggear_macros_free(gaminggear_macros);

		if (error) {
			gaminggear_warning_dialog(GTK_WINDOW(parent), _("Could not export macros"), error->message);
			g_clear_error(&error);
		}
	}
	gtk_widget_destroy(dialog);
	gaminggear_configuration_free(config);
}

static void show_quit_cb(GtkMenuItem *item, gpointer user_data) {
	gtk_dialog_response(GTK_DIALOG(user_data), GTK_RESPONSE_REJECT);
}

static void show_about_cb(GtkMenuItem *item, gpointer user_data) {
	gaminggear_about_dialog(GTK_WINDOW(user_data), _("Gaminggear macro editor"), _("Application to edit gaminggear macros"));
}

static void show_open_cb(GtkMenuItem *item, gpointer user_data) {
	GaminggearMacroEditorDialog *parent = GAMINGGEAR_MACRO_EDITOR_DIALOG(user_data);
	GaminggearKeyFile *config;
	GtkWidget *dialog;
	GError *error = NULL;
	gchar *filename, *path;
	GtkFileFilter *filter;
	GaminggearMacros *gaminggear_macros;

	config = gaminggear_configuration_load();

	dialog = gtk_file_chooser_dialog_new(_("Load gaminggear macros"),
			GTK_WINDOW(parent),
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL
	);

	filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, _("Gaminggear macros"));
	gtk_file_filter_add_pattern(filter, "*.ini");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

	filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, _("All files"));
	gtk_file_filter_add_pattern(filter, "*");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

	path = gaminggear_configuration_get_macro_save_path(config);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), path);
	g_free(path);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

		path = g_path_get_dirname(filename);
		gaminggear_configuration_set_macro_save_path(config, path);
		g_free(path);
		gaminggear_configuration_save(config, &error);

		gaminggear_macros = gaminggear_macros_load_with_path(filename, &error);
		g_free(filename);

		if (error) {
			gaminggear_warning_dialog(GTK_WINDOW(parent), _("Could not load macros"), error->message);
			g_clear_error(&error);
		} else {
			gaminggear_macro_editor_dialog_clear(parent);
			gaminggear_macro_editor_dialog_add_macros(parent, gaminggear_macros);
			/* loaded macros keep modified state */
			gaminggear_macros_free(gaminggear_macros);
		}
	}
	gtk_widget_destroy(dialog);
	gaminggear_configuration_free(config);
}

static void show_save_cb(GtkMenuItem *item, gpointer user_data) {
	GaminggearMacroEditorDialog *parent = GAMINGGEAR_MACRO_EDITOR_DIALOG(user_data);
	gaminggear_macro_editor_dialog_save_macros(parent);
}

static void show_save_as_cb(GtkMenuItem *item, gpointer user_data) {
	GaminggearMacroEditorDialog *parent = GAMINGGEAR_MACRO_EDITOR_DIALOG(user_data);
	GaminggearKeyFile *config;
	GtkWidget *dialog;
	GError *error = NULL;
	gchar *filename, *path;
	GtkFileFilter *filter;
	gchar *temp_filename;
	GaminggearMacros *gaminggear_macros;

	config = gaminggear_configuration_load();

	dialog = gtk_file_chooser_dialog_new(_("Save gaminggear macros as"),
			GTK_WINDOW(parent),
			GTK_FILE_CHOOSER_ACTION_SAVE,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
			NULL
	);

	filter = gtk_file_filter_new();
	gtk_file_filter_set_name(filter, _("Gaminggear macros"));
	gtk_file_filter_add_pattern(filter, "*.ini");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

	path = gaminggear_configuration_get_macro_save_path(config);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), path);
	g_free(path);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

		path = g_path_get_dirname(filename);
		gaminggear_configuration_set_macro_save_path(config, path);
		g_free(path);
		gaminggear_configuration_save(config, &error);

		gaminggear_macros = gaminggear_macro_editor_dialog_get_macros(parent);

		temp_filename = gaminggear_create_filename_with_extension(filename, "ini");
		g_free(filename);
		gaminggear_macros_save_with_path(temp_filename, gaminggear_macros, &error);
		/* macros keep modified state for saving in default location */
		g_free(temp_filename);
		gaminggear_macros_free(gaminggear_macros);

		if (error) {
			gaminggear_warning_dialog(GTK_WINDOW(parent), _("Could not save macros"), error->message);
			g_clear_error(&error);
		}
	}
	gtk_widget_destroy(dialog);
	gaminggear_configuration_free(config);
}

GtkWidget *gaminggear_macro_editor_dialog_menu_bar_new(GaminggearMacroEditorDialog *dialog) {
	GtkWidget *menu_bar;
	GtkWidget *menu_item;
	GtkWidget *menu;

	menu_bar = gtk_menu_bar_new();

	menu_item = gtk_menu_item_new_with_label(_("File"));
	menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_item);

	menu_item = gtk_image_menu_item_new_from_stock(GTK_STOCK_OPEN, NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
	g_signal_connect(G_OBJECT(menu_item), "activate", G_CALLBACK(show_open_cb), dialog);

	menu_item = gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE, NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
	g_signal_connect(G_OBJECT(menu_item), "activate", G_CALLBACK(show_save_cb), dialog);

	menu_item = gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE_AS, NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
	g_signal_connect(G_OBJECT(menu_item), "activate", G_CALLBACK(show_save_as_cb), dialog);

	menu_item = gtk_image_menu_item_new_with_label(_("Import"));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
	g_signal_connect(G_OBJECT(menu_item), "activate", G_CALLBACK(show_import_cb), dialog);

	menu_item = gtk_image_menu_item_new_with_label(_("Export"));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
	g_signal_connect(G_OBJECT(menu_item), "activate", G_CALLBACK(show_export_cb), dialog);

	menu_item = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
	g_signal_connect(G_OBJECT(menu_item), "activate", G_CALLBACK(show_quit_cb), dialog);

	menu_item = gtk_menu_item_new_with_label(_("Help"));
	menu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item), menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_item);

	menu_item = gtk_image_menu_item_new_from_stock(GTK_STOCK_ABOUT, NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
	g_signal_connect(G_OBJECT(menu_item), "activate", G_CALLBACK(show_about_cb), dialog);

	return menu_bar;
}
