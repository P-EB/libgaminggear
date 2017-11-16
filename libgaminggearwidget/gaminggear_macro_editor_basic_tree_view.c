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

#include "gaminggear_macro_editor_basic_tree_view.h"
#include "gaminggear/gdk_key_translations.h"
#include "i18n-lib.h"

#define GAMINGGEAR_MACRO_EDITOR_BASIC_TREE_VIEW_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GAMINGGEAR_MACRO_EDITOR_BASIC_TREE_VIEW_TYPE, GaminggearMacroEditorBasicTreeViewClass))
#define IS_GAMINGGEAR_MACRO_EDITOR_BASIC_TREE_VIEW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GAMINGGEAR_MACRO_EDITOR_BASIC_TREE_VIEW_TYPE))
#define GAMINGGEAR_MACRO_EDITOR_BASIC_TREE_VIEW_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), GAMINGGEAR_MACRO_EDITOR_BASIC_TREE_VIEW_TYPE, GaminggearMacroEditorBasicTreeViewPrivate))

typedef struct _GaminggearMacroEditorBasicTreeViewClass GaminggearMacroEditorBasicTreeViewClass;

struct _GaminggearMacroEditorBasicTreeViewClass {
	GtkTreeViewClass parent_class;
};

struct _GaminggearMacroEditorBasicTreeViewPrivate {
	GtkMenu *menu;
};

G_DEFINE_TYPE(GaminggearMacroEditorBasicTreeView, gaminggear_macro_editor_basic_tree_view, GTK_TYPE_TREE_VIEW);

static gchar const * const macro_list_key_key = "macro_list_key";

void gaminggear_macro_editor_basic_tree_view_add_keystroke(GaminggearMacroEditorBasicTreeView *tree_view, guint key, guint action, glong rel_time) {
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter insert_iter;
	GtkTreeIter *pinsert_iter;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
	if (gtk_tree_selection_get_selected(selection, &model, &insert_iter)) {
		pinsert_iter = &insert_iter;
	} else {
		model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree_view));
		pinsert_iter = NULL;
	}

	gaminggear_macro_editor_basic_list_store_insert_keystroke_before(GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE(model), pinsert_iter, key, action, rel_time);
}

static void popup_insert_delay_cb(GtkMenuItem *item, gpointer user_data) {
	GaminggearMacroEditorBasicTreeView *tree_view = GAMINGGEAR_MACRO_EDITOR_BASIC_TREE_VIEW(user_data);
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	GtkTreeIter insert_iter;
	GtkTreeIter *pinsert_iter;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
	if (gtk_tree_selection_get_selected(selection, &model, &insert_iter)) {
		pinsert_iter = &insert_iter;
	} else {
		model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree_view));
		pinsert_iter = NULL;
	}

	gaminggear_macro_editor_basic_list_store_insert_period_before(GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE(model), pinsert_iter, 1);
}

static void popup_insert_button_down_cb(GtkMenuItem *item, gpointer user_data) {
	GaminggearMacroEditorBasicTreeView *tree_view = GAMINGGEAR_MACRO_EDITOR_BASIC_TREE_VIEW(user_data);
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	GtkTreeIter insert_iter;
	GtkTreeIter *pinsert_iter;
	guint key;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
	if (gtk_tree_selection_get_selected(selection, &model, &insert_iter)) {
		pinsert_iter = &insert_iter;
	} else {
		model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree_view));
		pinsert_iter = NULL;
	}

	key = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(item), macro_list_key_key));

	gaminggear_macro_editor_basic_list_store_insert_single_action_key_before(GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE(model),
			pinsert_iter, key, GAMINGGEAR_MACRO_KEYSTROKE_ACTION_PRESS);
}

static void popup_insert_button_up_cb(GtkMenuItem *item, gpointer user_data) {
	GaminggearMacroEditorBasicTreeView *tree_view = GAMINGGEAR_MACRO_EDITOR_BASIC_TREE_VIEW(user_data);
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	GtkTreeIter insert_iter;
	GtkTreeIter *pinsert_iter;
	guint key;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
	if (gtk_tree_selection_get_selected(selection, &model, &insert_iter)) {
		pinsert_iter = &insert_iter;
	} else {
		model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree_view));
		pinsert_iter = NULL;
	}

	key = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(item), macro_list_key_key));

	gaminggear_macro_editor_basic_list_store_insert_single_action_key_before(GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE(model),
			pinsert_iter, key, GAMINGGEAR_MACRO_KEYSTROKE_ACTION_RELEASE);
}

static void popup_insert_button_click_cb(GtkMenuItem *item, gpointer user_data) {
	GaminggearMacroEditorBasicTreeView *tree_view = GAMINGGEAR_MACRO_EDITOR_BASIC_TREE_VIEW(user_data);
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	GtkTreeIter insert_iter;
	GtkTreeIter *pinsert_iter;
	guint key;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
	if (gtk_tree_selection_get_selected(selection, &model, &insert_iter)) {
		pinsert_iter = &insert_iter;
	} else {
		model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree_view));
		pinsert_iter = NULL;
	}

	key = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(item), macro_list_key_key));

	// FIXME with period
	gaminggear_macro_editor_basic_list_store_insert_keystroke_before(GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE(model), pinsert_iter, key, GAMINGGEAR_MACRO_KEYSTROKE_ACTION_PRESS, 0);
	gaminggear_macro_editor_basic_list_store_insert_keystroke_before(GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE(model), pinsert_iter, key, GAMINGGEAR_MACRO_KEYSTROKE_ACTION_RELEASE, 0);
}

static void popup_remove_cb(GtkMenuItem *item, gpointer user_data) {
	GaminggearMacroEditorBasicTreeView *tree_view = GAMINGGEAR_MACRO_EDITOR_BASIC_TREE_VIEW(user_data);
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	GtkTreeIter entry;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
	/* object to be removed has to be selected */
	if (!gtk_tree_selection_get_selected(selection, &model, &entry))
		return;

	gaminggear_macro_editor_basic_list_store_remove(GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE(model), &entry);
}

static GtkWidget *subsubmenu(GaminggearMacroEditorBasicTreeView *tree_view, guint key) {
	GtkWidget *menu, *item;

	menu = gtk_menu_new();

	item = gtk_menu_item_new_with_label(_("Down"));
	g_object_set_data(G_OBJECT(item), macro_list_key_key, GUINT_TO_POINTER(key));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(popup_insert_button_down_cb), tree_view);

	item = gtk_menu_item_new_with_label(_("Up"));
	g_object_set_data(G_OBJECT(item), macro_list_key_key, GUINT_TO_POINTER(key));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(popup_insert_button_up_cb), tree_view);

	item = gtk_menu_item_new_with_label(_("Click"));
	g_object_set_data(G_OBJECT(item), macro_list_key_key, GUINT_TO_POINTER(key));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(popup_insert_button_click_cb), tree_view);

	return menu;
}

static GtkWidget *submenu(GaminggearMacroEditorBasicTreeView *tree_view) {
	GtkWidget *menu, *item;
	guint i;
	gchar *title;

	menu = gtk_menu_new();

	for (i = GAMINGGEAR_MACRO_KEYSTROKE_KEY_BUTTON_LEFT; i < GAMINGGEAR_MACRO_KEYSTROKE_KEY_BUTTON_LEFT + 6; ++i) {
		title = gaminggear_hid_to_keyname(i);
		item = gtk_menu_item_new_with_label(title);
		g_free(title);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), subsubmenu(tree_view, i));
	}

	return menu;
}

static GtkWidget *menu_new(GaminggearMacroEditorBasicTreeView *tree_view) {
	GtkWidget *menu, *item;

	menu = gtk_menu_new();

	item = gtk_image_menu_item_new_from_stock(GTK_STOCK_REMOVE, NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(popup_remove_cb), tree_view);

	item = gtk_menu_item_new_with_label(_("Insert delay"));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(popup_insert_delay_cb), tree_view);

	item = gtk_menu_item_new_with_label(_("Insert mouse event"));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), submenu(tree_view));

	gtk_widget_show_all(menu);
	return menu;
}

static void delay_edited_cb(GtkCellRendererText *renderer, gchar const *path_string, gchar const *new_text, gpointer user_data) {
	GaminggearMacroEditorBasicTreeView *tree_view = GAMINGGEAR_MACRO_EDITOR_BASIC_TREE_VIEW(user_data);
	GaminggearMacroEditorBasicListStore *store;
	GtkTreePath *path;
	GtkTreeIter iter;

	store = GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(tree_view)));
	path = gtk_tree_path_new_from_string(path_string);
	gtk_tree_model_get_iter(GTK_TREE_MODEL(store), &iter, path);
	gaminggear_macro_editor_basic_list_store_set_value(store, &iter, new_text);
	gtk_tree_path_free(path);
}

GtkWidget *gaminggear_macro_editor_basic_tree_view_new(GaminggearMacroEditorBasicListStore *list_store) {
	GaminggearMacroEditorBasicTreeView *tree_view;

	tree_view = g_object_new(GAMINGGEAR_MACRO_EDITOR_BASIC_TREE_VIEW_TYPE,
			NULL);

	gtk_tree_view_set_model(GTK_TREE_VIEW(tree_view), GTK_TREE_MODEL(list_store));

	return GTK_WIDGET(tree_view);
}

static gboolean popup_menu_on_right_mouse_button_cb(GtkTreeView *tree_view, GdkEventButton *event, gpointer user_data) {
	GaminggearMacroEditorBasicTreeViewPrivate *priv = GAMINGGEAR_MACRO_EDITOR_BASIC_TREE_VIEW(tree_view)->priv;

	if (!(event->type == GDK_BUTTON_PRESS && event->button == 3))
		return FALSE;

	gtk_menu_popup(GTK_MENU(priv->menu), NULL, NULL, NULL, NULL, event->button, gdk_event_get_time((GdkEvent*)event));

	return TRUE;
}

static void gaminggear_macro_editor_basic_tree_view_init(GaminggearMacroEditorBasicTreeView *tree_view) {
	GaminggearMacroEditorBasicTreeViewPrivate *priv = GAMINGGEAR_MACRO_EDITOR_BASIC_TREE_VIEW_GET_PRIVATE(tree_view);
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;

	tree_view->priv = priv;

	priv->menu = GTK_MENU(menu_new(tree_view));
	g_object_ref_sink(priv->menu);

	column = gtk_tree_view_column_new();

	renderer = gtk_cell_renderer_pixbuf_new();
	gtk_tree_view_column_pack_start(GTK_TREE_VIEW_COLUMN(column), renderer, FALSE);
	gtk_tree_view_column_add_attribute(GTK_TREE_VIEW_COLUMN(column), renderer, "icon-name", gaminggear_macro_editor_basic_list_store_icon_column());

	renderer = gtk_cell_renderer_spin_new();
	g_object_set(renderer,
			"editable", TRUE,
			"adjustment", gtk_adjustment_new(1, 1, GAMINGGEAR_MACRO_KEYSTROKE_PERIOD_MAX, 1, 1000, 0),
			NULL);
	g_signal_connect(renderer, "edited", G_CALLBACK(delay_edited_cb), tree_view);
	gtk_tree_view_column_pack_start(GTK_TREE_VIEW_COLUMN(column), renderer, FALSE);
	gtk_tree_view_column_set_attributes(GTK_TREE_VIEW_COLUMN(column), renderer,
			"text", gaminggear_macro_editor_basic_list_store_text_column(),
			"visible", gaminggear_macro_editor_basic_list_store_is_not_key_column(),
			NULL);

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(GTK_TREE_VIEW_COLUMN(column), renderer, FALSE);
	gtk_tree_view_column_set_attributes(GTK_TREE_VIEW_COLUMN(column), renderer,
			"text", gaminggear_macro_editor_basic_list_store_text_column(),
			"visible", gaminggear_macro_editor_basic_list_store_is_key_column(),
			NULL);

	gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

	g_signal_connect(G_OBJECT(tree_view), "button-press-event", G_CALLBACK(popup_menu_on_right_mouse_button_cb), NULL);
}

static void gaminggear_macro_editor_basic_tree_view_finalize(GObject *object) {
	GaminggearMacroEditorBasicTreeViewPrivate *priv = GAMINGGEAR_MACRO_EDITOR_BASIC_TREE_VIEW(object)->priv;
	g_object_unref(priv->menu);
	G_OBJECT_CLASS(gaminggear_macro_editor_basic_tree_view_parent_class)->finalize(object);
}

static void gaminggear_macro_editor_basic_tree_view_class_init(GaminggearMacroEditorBasicTreeViewClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->finalize = gaminggear_macro_editor_basic_tree_view_finalize;

	g_type_class_add_private(klass, sizeof(GaminggearMacroEditorBasicTreeViewPrivate));
}
