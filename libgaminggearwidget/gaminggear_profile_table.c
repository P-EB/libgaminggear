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

#include "gaminggear/gaminggear_profile_table.h"
#include "gaminggear_profile_list_store.h"
#include "gaminggear_dialogs.h"
#include "gaminggear_helper.h"
#include "i18n-lib.h"
#include <string.h>

#define GAMINGGEAR_PROFILE_TABLE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GAMINGGEAR_PROFILE_TABLE_TYPE, GaminggearProfileTableClass))
#define IS_GAMINGGEAR_PROFILE_TABLE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GAMINGGEAR_PROFILE_TABLE_TYPE))
#define GAMINGGEAR_PROFILE_TABLE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), GAMINGGEAR_PROFILE_TABLE_TYPE, GaminggearProfileTablePrivate))

typedef struct _GaminggearProfileTableClass GaminggearProfileTableClass;

struct _GaminggearProfileTableClass {
	GtkTreeViewClass parent_class;
};

struct _GaminggearProfileTablePrivate {
	gchar *path;
	GtkMenu *menu;
	GType profile_type;
	guint num_profiles;
	GaminggearProfileData *default_profile;
};

enum {
	GAMINGGEAR_PROFILE_TABLE_NAME_COLUMN = 0,
	GAMINGGEAR_PROFILE_TABLE_HARDWARE_COLUMN,
	GAMINGGEAR_PROFILE_TABLE_DESKTOP_PROFILE_COLUMN,
};

enum {
	PROP_0,
	PROP_NUM_PROFILES,
	PROP_PROFILE_TYPE,
	PROP_PATH,
};

enum {
	REMOVED,
	SELECTED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE(GaminggearProfileTable, gaminggear_profile_table, GTK_TYPE_TREE_VIEW);

static gchar const * const hardware_index_key = "hardware_index_key";

gboolean gaminggear_profile_table_get_modified(GaminggearProfileTable *table) {
	GaminggearProfileData *profile_data;
	GtkTreeModel *model;
	GtkTreeIter iter;
	gboolean valid;
	gboolean modified;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(table));

	valid = gtk_tree_model_get_iter_first(model, &iter);

	while (valid) {
		profile_data = gaminggear_profile_list_store_get_profile(GAMINGGEAR_PROFILE_LIST_STORE(model), &iter);
		modified = gaminggear_profile_data_get_modified(profile_data);
		g_object_unref(profile_data);
		if (modified)
			return TRUE;

		valid = gtk_tree_model_iter_next(model, &iter);
	}

	return FALSE;
}

static GaminggearProfileData *gaminggear_profile_table_find_by_name(GtkTreeModel *model, gchar const *wanted_name) {
	GaminggearProfileListStore *store = GAMINGGEAR_PROFILE_LIST_STORE(model);
	GaminggearProfileData *profile_data;
	GtkTreeIter iter;
	gboolean valid;
	gchar const *name;

	valid = gtk_tree_model_get_iter_first(model, &iter);

	while (valid) {
		profile_data = gaminggear_profile_list_store_get_profile(store, &iter);
		name = gaminggear_profile_data_get_name(profile_data);

		if (strcmp(name, wanted_name) == 0)
			return profile_data;

		g_object_unref(profile_data);
		valid = gtk_tree_model_iter_next(model, &iter);
	}

	return NULL;
}

static GaminggearProfileData *gaminggear_profile_table_find_by_hardware_index(GtkTreeModel *model, gint index) {
	GaminggearProfileListStore *store = GAMINGGEAR_PROFILE_LIST_STORE(model);
	GaminggearProfileData *profile_data;
	GtkTreeIter iter;
	gboolean valid;
	gint value;

	valid = gtk_tree_model_get_iter_first(model, &iter);

	while (valid) {
		profile_data = gaminggear_profile_list_store_get_profile(store, &iter);
		value = gaminggear_profile_data_get_hardware_index(profile_data);

		if (value == index)
			return profile_data;

		g_object_unref(profile_data);
		valid = gtk_tree_model_iter_next(model, &iter);
	}

	return NULL;
}

static GaminggearProfileData *profile_data_new(GaminggearProfileTable *table) {
	return GAMINGGEAR_PROFILE_DATA(g_object_new(table->priv->profile_type, NULL));
}

void gaminggear_profile_table_add_profile(GaminggearProfileTable *table) {
	gaminggear_profile_list_store_add_profile(GAMINGGEAR_PROFILE_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(table))),
			NULL, profile_data_new(table));
}

gboolean gaminggear_profile_table_load(GaminggearProfileTable *table, GError **error) {
	GaminggearProfileData *profile_data;
	GDir *directory;
	gchar const *name;
	gchar *abs_path;
	gboolean retval;

	directory = g_dir_open(table->priv->path, 0, error);
	if (directory == NULL)
		return FALSE;

	while ((name = g_dir_read_name(directory))) {
		/* TODO maybe g_pattern_match_string for file ending */
		profile_data = profile_data_new(table);
		abs_path = g_build_filename(table->priv->path, name, NULL);
		retval = gaminggear_profile_data_load(profile_data, abs_path, error);
		g_free(abs_path);

		if (!retval) {
			g_object_unref(G_OBJECT(profile_data));
			g_dir_close(directory);
			// TODO warning dialog and continue
			return FALSE;
		}

		gaminggear_profile_list_store_add_profile(GAMINGGEAR_PROFILE_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(table))), NULL, profile_data);
		g_object_unref(G_OBJECT(profile_data));
	}

	g_dir_close(directory);
	return TRUE;
}

gboolean gaminggear_profile_table_fill(GaminggearProfileTable *table, GaminggearDevice *device, guint count, GError **error) {
	GtkTreeModel *model;
	GaminggearProfileData *profile_data;
	guint i;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(table));

	for (i = 0; i < count; ++i) {
		profile_data = gaminggear_profile_table_find_by_hardware_index(model, i);
		if (!profile_data) {
			profile_data = profile_data_new(table);
			gaminggear_profile_data_set_hardware_index(profile_data, i);
			gaminggear_profile_list_store_add_profile(GAMINGGEAR_PROFILE_LIST_STORE(model), NULL, profile_data);
			if (!gaminggear_profile_data_read(profile_data, device, error)) {
				g_object_unref(profile_data);
				return FALSE;
			}
		}

		g_object_unref(profile_data);
	}

	return TRUE;
}

gboolean gaminggear_profile_table_read(GaminggearProfileTable *table, GaminggearDevice *device, GError **error) {
	GtkTreeModel *model;
	GaminggearProfileData *profile_data;
	gboolean valid;
	GtkTreeIter iter;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(table));

	valid = gtk_tree_model_get_iter_first(model, &iter);

	while (valid) {
		profile_data = gaminggear_profile_list_store_get_profile(GAMINGGEAR_PROFILE_LIST_STORE(model), &iter);
		if (gaminggear_profile_data_get_hardware_index(profile_data) != GAMINGGEAR_PROFILE_DATA_GAMEFILE_PROFILE_INVALID) {
			if (!gaminggear_profile_data_read(profile_data, device, error)) {
				g_object_unref(profile_data);
				return FALSE;
			}
		}

		g_object_unref(profile_data);
		valid = gtk_tree_model_iter_next(model, &iter);
	}

	return TRUE;
}

gboolean gaminggear_profile_table_store(GaminggearProfileTable *table, GaminggearDevice *device, GError **error) {
	GtkTreeModel *model;
	GaminggearProfileData *profile_data;
	gboolean valid;
	gboolean result;
	GtkTreeIter iter;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(table));

	valid = gtk_tree_model_get_iter_first(model, &iter);

	while (valid) {
		profile_data = gaminggear_profile_list_store_get_profile(GAMINGGEAR_PROFILE_LIST_STORE(model), &iter);

		result = gaminggear_profile_data_store(profile_data, device, error);
		g_object_unref(profile_data);
		if (!result)
			return FALSE;

		valid = gtk_tree_model_iter_next(model, &iter);
	}

	return TRUE;
}

static void popup_duplicate_cb(GtkMenuItem *item, gpointer user_data) {
	GaminggearProfileTable *table = GAMINGGEAR_PROFILE_TABLE(user_data);
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	GtkTreeIter entry;
	GaminggearProfileData *old_profile_data;
	GaminggearProfileData *new_profile_data;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(table));
	if (!gtk_tree_selection_get_selected(selection, &model, &entry))
		return;

	// TODO update from view first if needed -> done in window
	old_profile_data = gaminggear_profile_list_store_get_profile(GAMINGGEAR_PROFILE_LIST_STORE(model), &entry);
	new_profile_data = gaminggear_profile_data_dup(old_profile_data);
	gaminggear_profile_data_discriminate(new_profile_data);

	gaminggear_profile_list_store_add_profile(GAMINGGEAR_PROFILE_LIST_STORE(model), NULL, new_profile_data);

	g_object_unref(new_profile_data);
	g_object_unref(old_profile_data);
}

static void popup_rename_cb(GtkMenuItem *item, gpointer user_data) {
	GtkTreeView *table = GTK_TREE_VIEW(user_data);
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	GtkCellRenderer *cell;
	GList *rows, *ptr, *cells;
	GtkTreePath *row;
	GtkTreeViewColumn *column;

	model = gtk_tree_view_get_model(table);
	selection = gtk_tree_view_get_selection(table);
	rows = gtk_tree_selection_get_selected_rows(selection, &model);

	column = gtk_tree_view_get_column(table, GAMINGGEAR_PROFILE_TABLE_NAME_COLUMN);
	cells = gtk_cell_layout_get_cells(GTK_CELL_LAYOUT(column));
	cell = cells->data;

	ptr = rows;
	while(ptr != NULL) {
		row = (GtkTreePath *)ptr->data;

		g_object_set(cell, "editable", TRUE, "editable-set", TRUE, NULL);

		gtk_tree_view_scroll_to_cell(table, NULL, column, TRUE, 0, 0);

		/* start editing */
		gtk_tree_view_set_cursor(table, row, column, TRUE);

		g_object_set(cell, "editable", FALSE, "editable-set", FALSE, NULL);

		ptr = ptr->next;
	}

	g_list_free(cells);
	g_list_free_full(rows, (GDestroyNotify)gtk_tree_path_free);
}

// TODO code duplicate of gaminggear_macro_table
static gboolean should_remove(gchar const *title, gchar const *text) {
	GtkWidget *dialog;
	gboolean result = FALSE;

	dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "%s", text);
	gtk_window_set_title(GTK_WINDOW(dialog), title);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_YES)
		result = TRUE;

	gtk_widget_destroy(dialog);
	return result;
}

gchar const *gaminggear_profile_table_get_default_profile_name(GaminggearProfileTable *table) {
	GaminggearProfileTablePrivate *priv = table->priv;

	if (priv->default_profile)
		return gaminggear_profile_data_get_name(priv->default_profile);
	else
		return NULL;
}

void gaminggear_profile_table_set_default_profile_name(GaminggearProfileTable *table, gchar const *name) {
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(table));
	GaminggearProfileData *profile_data = gaminggear_profile_table_find_by_name(model, name);
	table->priv->default_profile = profile_data;
}

static void remove_default_profile_if(GaminggearProfileTable *table, GaminggearProfileData *profile_data) {
	GaminggearProfileTablePrivate *priv = table->priv;

	if (profile_data == priv->default_profile) {
		g_object_unref(priv->default_profile);
		priv->default_profile = NULL;
	}
}

static void toggle_default_profile(GaminggearProfileTable *table, GaminggearProfileData *profile_data) {
	GaminggearProfileTablePrivate *priv = table->priv;

	if (profile_data == priv->default_profile) {
		g_object_unref(priv->default_profile);
		priv->default_profile = NULL;
	} else {
		g_object_ref(profile_data);
		priv->default_profile = profile_data;
	}
}

static void popup_remove_cb(GtkMenuItem *item, gpointer user_data) {
	GaminggearProfileTable *table = GAMINGGEAR_PROFILE_TABLE(user_data);
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	GtkTreeIter entry;
	GaminggearProfileData *profile_data;
	GError *local_error = NULL;
	gboolean retval;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(table));
	if (!gtk_tree_selection_get_selected(selection, &model, &entry))
		return;

	if (should_remove(_("Remove profile"), _("Do you really want to remove this profile?"))) {
		profile_data = gaminggear_profile_list_store_get_profile(GAMINGGEAR_PROFILE_LIST_STORE(model), &entry);
		retval = gaminggear_profile_data_remove(profile_data, &local_error);
		remove_default_profile_if(table, profile_data);
		g_signal_emit((gpointer)table, signals[REMOVED], 0, profile_data);
		g_object_unref(profile_data);
		gaminggear_profile_list_store_remove(GAMINGGEAR_PROFILE_LIST_STORE(model), &entry);
		if (!retval) {
			gaminggear_warning_dialog(NULL, _("Could not remove profile"), local_error->message);
			g_clear_error(&local_error);
		}
	}

}

static void popup_default_profile_cb(GtkMenuItem *item, gpointer user_data) {
	GaminggearProfileTable *table = GAMINGGEAR_PROFILE_TABLE(user_data);
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	GtkTreeIter entry;
	GaminggearProfileData *profile_data;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(table));
	if (!gtk_tree_selection_get_selected(selection, &model, &entry))
		return;

	profile_data = gaminggear_profile_list_store_get_profile(GAMINGGEAR_PROFILE_LIST_STORE(model), &entry);
	toggle_default_profile(table, profile_data);
	g_object_unref(profile_data);
}

static void popup_set_hardware_index_cb(GtkMenuItem *item, gpointer user_data) {
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	GtkTreeIter entry;
	GaminggearProfileData *other_profile_data;
	GaminggearProfileData *this_profile_data;
	guint i;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(user_data));
	if (!gtk_tree_selection_get_selected(selection, &model, &entry))
		return;

	i = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(item), hardware_index_key));
	this_profile_data = gaminggear_profile_list_store_get_profile(GAMINGGEAR_PROFILE_LIST_STORE(model), &entry);
	other_profile_data = gaminggear_profile_table_find_by_hardware_index(model, i);

	if (other_profile_data)
		gaminggear_profile_data_set_hardware_index(other_profile_data, GAMINGGEAR_PROFILE_DATA_GAMEFILE_PROFILE_INVALID);

	gaminggear_profile_data_set_hardware_index(this_profile_data, i);

	g_object_unref(this_profile_data);
}

static GtkWidget *submenu(GaminggearProfileTable *table) {
	GaminggearProfileTablePrivate *priv = table->priv;
	GtkWidget *menu, *item;
	guint i;
	gchar *title;

	menu = gtk_menu_new();

	for (i = 0; i < priv->num_profiles; ++i) {

		title = g_strdup_printf("%u", i + 1);
		item = gtk_menu_item_new_with_label(title);
		g_free(title);
		g_object_set_data(G_OBJECT(item), hardware_index_key, GUINT_TO_POINTER(i));
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
		g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(popup_set_hardware_index_cb), table);
	}

	return menu;
}

static GtkWidget *menu_new(GaminggearProfileTable *table) {
	GtkWidget *menu, *item;

	menu = gtk_menu_new();

	item = gtk_menu_item_new_with_label(_("Rename"));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(popup_rename_cb), table);

	item = gtk_menu_item_new_with_label(_("Duplicate"));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(popup_duplicate_cb), table);

	item = gtk_image_menu_item_new_from_stock(GTK_STOCK_REMOVE, NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(popup_remove_cb), table);

	item = gtk_menu_item_new_with_label(_("Toggle desktop profile"));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(popup_default_profile_cb), table);

	item = gtk_menu_item_new_with_label(_("Hardware index"));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), submenu(table));

	gtk_widget_show_all(menu);
	return menu;
}

GtkWidget *gaminggear_profile_table_new(GType type, gchar const *path, guint profile_num) {
	GaminggearProfileTable *table;

	table = GAMINGGEAR_PROFILE_TABLE(g_object_new(GAMINGGEAR_PROFILE_TABLE_TYPE,
			"model", GTK_TREE_MODEL(gaminggear_profile_list_store_new()),
			"num-profiles", profile_num,
			"profile-type", type,
			"path", path,
			NULL));

	return GTK_WIDGET(table);
}

static gboolean popup_menu_on_right_mouse_button_cb(GtkTreeView *table, GdkEventButton *event, gpointer user_data) {
	GaminggearProfileTablePrivate *priv = GAMINGGEAR_PROFILE_TABLE(table)->priv;

	if (!(event->type == GDK_BUTTON_PRESS && event->button == 3))
		return FALSE;

	gtk_menu_popup(priv->menu, NULL, NULL, NULL, NULL, event->button, gdk_event_get_time((GdkEvent*)event));

	return TRUE;
}

static void name_cell_data_func(GtkTreeViewColumn *column, GtkCellRenderer *renderer, GtkTreeModel *model, GtkTreeIter *iter, gpointer data) {
	GaminggearProfileData *profile = gaminggear_profile_list_store_get_profile(GAMINGGEAR_PROFILE_LIST_STORE(model), iter);
	gchar *string;

	string = g_strdup_printf("%s%s", gaminggear_profile_data_get_modified(profile) ? "* " : "", gaminggear_profile_data_get_name(profile));
	g_object_set(renderer, "text", string, NULL);
	g_free(string);

	g_object_unref(profile);
}

static void hardware_cell_data_func(GtkTreeViewColumn *column, GtkCellRenderer *renderer, GtkTreeModel *model, GtkTreeIter *iter, gpointer data) {
	GaminggearProfileData *profile;
	gchar *string;
	gint index;

	profile = gaminggear_profile_list_store_get_profile(GAMINGGEAR_PROFILE_LIST_STORE(model), iter);
	index = gaminggear_profile_data_get_hardware_index(profile);
	g_object_unref(profile);

	if (index == GAMINGGEAR_PROFILE_DATA_GAMEFILE_PROFILE_INVALID) {
		g_object_set(renderer, "text", "", NULL);
	} else {
		string = g_strdup_printf("%i", index + 1);
		g_object_set(renderer, "text", string, NULL);
		g_free(string);
	}
}

static void dp_cell_data_func(GtkTreeViewColumn *column, GtkCellRenderer *renderer, GtkTreeModel *model, GtkTreeIter *iter, gpointer data) {
	GaminggearProfileTable *table = GAMINGGEAR_PROFILE_TABLE(data);
	GaminggearProfileData *profile;

	profile = gaminggear_profile_list_store_get_profile(GAMINGGEAR_PROFILE_LIST_STORE(model), iter);
	if (profile == table->priv->default_profile)
		g_object_set(renderer, "text", "DP", NULL);
	else
		g_object_set(renderer, "text", "", NULL);
	g_object_unref(profile);
}

// TODO code duplicate from gaminggear_macro_table
static gboolean name_is_valid(gchar const *name) {
	gchar *stripped;
	gboolean retval;

	if (name == NULL)
		return FALSE;

	stripped = g_strstrip(g_strdup(name));
	retval = !(g_ascii_strcasecmp(stripped, "") == 0);
	g_free(stripped);
	return retval;
}

static void name_column_edited_cb(GtkCellRendererText *renderer, gchar *path_string, gchar *new_text, gpointer user_data) {
	GaminggearProfileTable *table = GAMINGGEAR_PROFILE_TABLE(user_data);
	GtkTreeIter iter;
	GaminggearProfileListStore *store;
	GaminggearProfileData *profile_data;
	GtkTreePath *path;
	gboolean valid;

	if (!name_is_valid(new_text))
		return;

	store = GAMINGGEAR_PROFILE_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(table)));

	path = gtk_tree_path_new_from_string(path_string);

	valid = gtk_tree_model_get_iter(GTK_TREE_MODEL(store), &iter, path);
	gtk_tree_path_free(path);

	if (!valid)
		return;

	profile_data = gaminggear_profile_list_store_get_profile(store, &iter);
	gaminggear_profile_data_set_name(profile_data, new_text);
	g_object_unref(profile_data);
}

static void row_activated_cb(GtkTreeView *table,
		GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data) {
	GtkTreeModel *model;
	GtkTreeIter iter;
	GaminggearProfileData *profile;

	model = gtk_tree_view_get_model(table);
	if (!gtk_tree_model_get_iter(model, &iter, path))
		return;
	profile = gaminggear_profile_list_store_get_profile(GAMINGGEAR_PROFILE_LIST_STORE(model), &iter);
	g_signal_emit((gpointer)table, signals[SELECTED], 0, profile);
}

static void gaminggear_profile_table_init(GaminggearProfileTable *table) {
	table->priv = GAMINGGEAR_PROFILE_TABLE_GET_PRIVATE(table);
}

static GObject *gaminggear_profile_table_constructor(GType gtype, guint n_properties, GObjectConstructParam *properties) {
	GaminggearProfileTable *table;
	GaminggearProfileTablePrivate *priv;
	GtkCellRenderer *renderer;
	GObject *obj;

	obj = G_OBJECT_CLASS(gaminggear_profile_table_parent_class)->constructor(gtype, n_properties, properties);
	table = GAMINGGEAR_PROFILE_TABLE(obj);
	priv = table->priv;

	priv->default_profile = NULL;

	priv->menu = GTK_MENU(menu_new(table));
	g_object_ref_sink(priv->menu);

	/* GAMINGGEAR_PROFILE_TABLE_NAME_COLUMN */
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_data_func(GTK_TREE_VIEW(table), -1, _("Profile"), renderer, name_cell_data_func, table, NULL);
	/* renderer is not set editable. Editing is not started through click, but via popup menu entry */
	g_signal_connect(G_OBJECT(renderer), "edited", G_CALLBACK(name_column_edited_cb), table);

	/* GAMINGGEAR_PROFILE_TABLE_HARDWARE_COLUMN */
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_data_func(GTK_TREE_VIEW(table), -1, _("HW"), renderer, hardware_cell_data_func, table, NULL);

	/* GAMINGGEAR_PROFILE_TABLE_DESKTOP_PROFILE_COLUMN */
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_data_func(GTK_TREE_VIEW(table), -1, _("DP"), renderer, dp_cell_data_func, table, NULL);

	g_signal_connect(G_OBJECT(table), "row-activated", G_CALLBACK(row_activated_cb), table);
	g_signal_connect(G_OBJECT(table), "button-press-event", G_CALLBACK(popup_menu_on_right_mouse_button_cb), NULL);

	return obj;
}

static void gaminggear_profile_table_finalize(GObject *object) {
	GaminggearProfileTablePrivate *priv = GAMINGGEAR_PROFILE_TABLE(object)->priv;
	g_object_unref(priv->menu);
	g_clear_pointer(&priv->path, g_free);
	G_OBJECT_CLASS(gaminggear_profile_table_parent_class)->finalize(object);
}

static void gaminggear_profile_table_set_property(GObject *object, guint prop_id, GValue const *value, GParamSpec *pspec) {
	GaminggearProfileTablePrivate *priv = GAMINGGEAR_PROFILE_TABLE(object)->priv;

	switch(prop_id) {
	case PROP_PROFILE_TYPE:
		priv->profile_type = g_value_get_gtype(value);
		break;
	case PROP_NUM_PROFILES:
		priv->num_profiles = g_value_get_uint(value);
		break;
	case PROP_PATH:
		priv->path = g_strdup(g_value_get_string(value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static void gaminggear_profile_table_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec) {
	GaminggearProfileTablePrivate *priv = GAMINGGEAR_PROFILE_TABLE(object)->priv;

	switch(prop_id) {
	case PROP_PROFILE_TYPE:
		g_value_set_gtype(value, priv->profile_type);
		break;
	case PROP_NUM_PROFILES:
		g_value_set_uint(value, priv->num_profiles);
		break;
	case PROP_PATH:
		g_value_set_string(value, priv->path);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static void gaminggear_profile_table_class_init(GaminggearProfileTableClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->set_property = gaminggear_profile_table_set_property;
	gobject_class->get_property = gaminggear_profile_table_get_property;
	gobject_class->constructor = gaminggear_profile_table_constructor;
	gobject_class->finalize = gaminggear_profile_table_finalize;

	g_type_class_add_private(klass, sizeof(GaminggearProfileTablePrivate));

	signals[SELECTED] = g_signal_new("selected",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__OBJECT, G_TYPE_NONE,
			1, GAMINGGEAR_PROFILE_DATA_TYPE);

	signals[REMOVED] = g_signal_new("removed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__OBJECT, G_TYPE_NONE,
			1, GAMINGGEAR_PROFILE_DATA_TYPE);

	g_object_class_install_property(gobject_class, PROP_NUM_PROFILES,
			g_param_spec_uint("num-profiles",
					"num-profiles",
					"Number of profiles",
					0, G_MAXUINT, 1,
					G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

	g_object_class_install_property(gobject_class, PROP_PROFILE_TYPE,
			g_param_spec_gtype("profile-type",
					"profile type",
					"Profile type",
					GAMINGGEAR_PROFILE_DATA_TYPE,
					G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

	g_object_class_install_property(gobject_class, PROP_PATH,
			g_param_spec_string("path",
					"path",
					"Path",
					"",
					G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
}
