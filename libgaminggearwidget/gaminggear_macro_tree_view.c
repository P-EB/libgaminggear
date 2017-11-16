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

#include "gaminggear_macro_tree_view.h"
#include "gaminggear_macro_tree_store.h"
#include "gaminggear/gaminggear_text_dialog.h"
#include "gaminggear_helper.h"
#include "gaminggearwidget_helper.h"
#include "i18n-lib.h"
#include <string.h>

#define GAMINGGEAR_MACRO_TREE_VIEW_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GAMINGGEAR_MACRO_TREE_VIEW_TYPE, GaminggearMacroTreeViewClass))
#define IS_GAMINGGEAR_MACRO_TREE_VIEW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GAMINGGEAR_MACRO_TREE_VIEW_TYPE))
#define GAMINGGEAR_MACRO_TREE_VIEW_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), GAMINGGEAR_MACRO_TREE_VIEW_TYPE, GaminggearMacroTreeViewPrivate))

typedef struct _GaminggearMacroTreeViewClass GaminggearMacroTreeViewClass;

struct _GaminggearMacroTreeViewClass {
	GtkTreeViewClass parent_class;
	void(*removed)(GaminggearMacroTreeView *macro_tree_view);
	void(*selected)(GaminggearMacroTreeView *macro_tree_view, GtkTreeRowReference *reference);
};

struct _GaminggearMacroTreeViewPrivate {
	GtkMenu *macro_menu;
	GtkMenu *macroset_menu;
};

G_DEFINE_TYPE(GaminggearMacroTreeView, gaminggear_macro_tree_view, GTK_TYPE_TREE_VIEW);

enum {
	REMOVED,
	SELECTED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

static const GtkTargetEntry drag_drop_types[] = {
		{ "GTK_TREE_MODEL_ROW", GTK_TARGET_SAME_WIDGET, 0 }
};

static guint timer_expander = 0;

static void removed(GaminggearMacroTreeView *macro_tree_view) {
	g_signal_emit((gpointer)macro_tree_view, signals[REMOVED], 0);
}

static void selected(GaminggearMacroTreeView *macro_tree_view, GtkTreeRowReference *reference) {
	g_signal_emit((gpointer)macro_tree_view, signals[SELECTED], 0, reference);
}

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

void gaminggear_macro_tree_view_add_macroset(GaminggearMacroTreeView *macro_tree_view) {
	GaminggearMacroTreeStore *store;
	GtkTreeIter iter;
	gchar *new_name;
	GtkTreeSelection *selection;

	store = gaminggear_macro_tree_view_get_store(macro_tree_view);

	new_name = gaminggear_text_dialog(NULL, _("New macroset"), _("Please enter name of macroset"), NULL);
	if (!name_is_valid(new_name))
		goto out_free;

	if (!gaminggear_macro_tree_store_is_macroset_name_unique(store, new_name))
		goto out_free;

	gaminggear_macro_tree_store_add_macroset(store, &iter, new_name);
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(macro_tree_view));
	gtk_tree_selection_select_iter(selection, &iter);

out_free:
	g_free(new_name);
}

static void row_activated_cb(GtkTreeView *tree_view,
		GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data) {
	GaminggearMacroTreeView *macro_tree_view = GAMINGGEAR_MACRO_TREE_VIEW(user_data);
	GtkTreeModel *model;
	GtkTreeRowReference *reference;

	model = gtk_tree_view_get_model(tree_view);

	if (gaminggear_macro_tree_store_is_macroset_with_path(path))
		gtk_gaminggear_tree_view_toggle_row_expansion(tree_view, path, FALSE);
	else {
		reference = gtk_tree_row_reference_new(model, path);
		selected(macro_tree_view, reference);
	}
}

static void edited_cb(GtkCellRendererText *renderer, gchar *path_string, gchar *new_text, gpointer user_data) {
	GaminggearMacroTreeView *macro_tree_view = GAMINGGEAR_MACRO_TREE_VIEW(user_data);
	GtkTreeIter iter;
	GaminggearMacroTreeStore *store;
	GtkTreePath *path;
	gboolean valid;

	if (!name_is_valid(new_text))
		return;

	store = gaminggear_macro_tree_view_get_store(macro_tree_view);

	path = gtk_tree_path_new_from_string(path_string);

	valid = gtk_tree_model_get_iter(GTK_TREE_MODEL(store), &iter, path);
	gtk_tree_path_free(path);

	if (!valid)
		return;

	gaminggear_macro_tree_store_rename(store, &iter, new_text);
}

static gint expand_row_timeout(GtkTreeView *treeview) {
	GtkTreePath *dest_path = NULL;
	GtkTreeViewDropPosition pos;

	GDK_THREADS_ENTER();

	gtk_tree_view_get_drag_dest_row(treeview, &dest_path, &pos);

	if (dest_path && (pos == GTK_TREE_VIEW_DROP_INTO_OR_AFTER || pos == GTK_TREE_VIEW_DROP_INTO_OR_BEFORE))
		gtk_tree_view_expand_row(treeview, dest_path, FALSE);

	if (dest_path)
		gtk_tree_path_free(dest_path);

	GDK_THREADS_LEAVE();

	/* always remove if work is done or not */
	return FALSE;
}

static void drag_data_received_cb(GtkWidget *treeview, GdkDragContext *context,
		gint x, gint y,
		GtkSelectionData *selection_data, guint info,
		guint time, gpointer dummy) {
	GtkTreeModel *source_model, *dest_model;
	GtkTreeRowReference *source_row_reference;
	GtkTreePath *source_row = NULL, *dest_row = NULL;
	GtkTreeViewDropPosition pos;
	gboolean success = FALSE;
	gboolean test;
	GtkTreeIter macroset_iter, macro_iter;
	gchar *macro_name;

	g_signal_stop_emission_by_name(treeview, "drag-data-received");

	if (gtk_selection_data_get_length(selection_data) == 0)
		goto out;

	gtk_tree_get_row_drag_data(selection_data, &source_model, &source_row);

	dest_model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
	gtk_tree_view_get_dest_row_at_pos(GTK_TREE_VIEW(treeview), x, y, &dest_row, &pos);

	/* don't accept macroset as source */
	if (gaminggear_macro_tree_store_is_macroset_with_path(source_row))
		goto out_free;

	if (gaminggear_macro_tree_store_is_macroset_with_path(dest_row)) {
		/* if dest_row is a macroset only IN drops are allowed */
		if (pos == GTK_TREE_VIEW_DROP_BEFORE || pos == GTK_TREE_VIEW_DROP_AFTER)
			goto out_free;
	} else {
		/* if dest row is a macro all drop positions are allowed */
		/* always use macroset as destination */
		gtk_tree_path_up(dest_row);
	}

	gtk_tree_model_get_iter(dest_model, &macroset_iter, dest_row);
	gtk_tree_model_get_iter(source_model, &macro_iter, source_row);
	macro_name = gaminggear_macro_tree_store_get_name(GAMINGGEAR_MACRO_TREE_STORE(source_model), &macro_iter);

	/* TODO rename if needed */
	test = !gaminggear_macro_tree_store_is_macro_name_unique(GAMINGGEAR_MACRO_TREE_STORE(dest_model), &macroset_iter, macro_name);
	g_free(macro_name);
	if (test)
		goto out_free;

	/* insert as first child */
	gtk_tree_path_down(dest_row);

	/* gtk_tree_drag_dest_row_drop_possible would return FALSE because tree is sorted. */

	source_row_reference = gtk_tree_row_reference_new(source_model, source_row);

	if (gtk_tree_drag_dest_drag_data_received(GTK_TREE_DRAG_DEST(dest_model), dest_row, selection_data)) {
		/* clear highlight */
		gtk_tree_view_set_drag_dest_row(GTK_TREE_VIEW(treeview), NULL, GTK_TREE_VIEW_DROP_BEFORE);

		gtk_tree_path_free(source_row);
		source_row = gtk_tree_row_reference_get_path(source_row_reference);
		gtk_tree_drag_source_drag_data_delete(GTK_TREE_DRAG_SOURCE(source_model), source_row);

		success = TRUE;
	}
	gtk_tree_row_reference_free(source_row_reference);
out_free:
	gtk_tree_path_free(source_row);
	gtk_tree_path_free(dest_row);
out:
	gtk_drag_finish(context, success, FALSE, time);
}

static gboolean drag_motion_cb(GtkWidget *treeview, GdkDragContext *context,
		gint x, gint y,
		guint time, gpointer dummy) {
	GtkTreeViewDropPosition pos;
	GtkTreePath *dest_row;

	if (timer_expander != 0) {
		g_source_remove(timer_expander);
		timer_expander = 0;
	}

	gtk_tree_view_get_dest_row_at_pos(GTK_TREE_VIEW(treeview), x, y, &dest_row, &pos);

	if (!dest_row)
		return FALSE;

	if (!gtk_tree_view_row_expanded(GTK_TREE_VIEW(treeview), dest_row)) {
		gtk_tree_view_set_drag_dest_row(GTK_TREE_VIEW(treeview), dest_row, pos);
		timer_expander = g_timeout_add(500, (GSourceFunc)expand_row_timeout, treeview);
	}
	gtk_tree_path_free(dest_row);
	return TRUE;
}

static void drag_data_get_cb(GtkWidget *treeview, GdkDragContext *context,
		GtkSelectionData *selection_data, guint info,
		guint time, gpointer dummy) {
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter iter;
	GtkTreePath *source_row;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
	if (!gtk_tree_selection_get_selected(selection, &model, &iter))
		return;

	source_row = gtk_tree_model_get_path(model, &iter);
	gtk_tree_set_row_drag_data(selection_data, model, source_row);
	gtk_tree_path_free(source_row);
}

static void popup_copy_macro_cb(GtkMenuItem *item, gpointer user_data) {
	GaminggearMacroTreeView *macro_tree_view = GAMINGGEAR_MACRO_TREE_VIEW(user_data);
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	GtkTreeIter macro_iter;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(macro_tree_view));
	if (!gtk_tree_selection_get_selected(selection, &model, &macro_iter))
		return;

	gaminggear_macro_tree_store_dup_macro(GAMINGGEAR_MACRO_TREE_STORE(model), &macro_iter);
}

static void popup_add_macro_cb(GtkMenuItem *item, gpointer user_data) {
	GaminggearMacroTreeView *macro_tree_view = GAMINGGEAR_MACRO_TREE_VIEW(user_data);
	GtkTreeModel *model;
	GtkTreeIter macroset, macro;
	gchar *macro_name;
	GtkTreeSelection *selection;
	GtkTreeViewColumn *column;
	GtkTreePath *macro_path, *macroset_path;
	GaminggearMacroTreeStore *macro_tree_store;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(macro_tree_view));
	if (!gtk_tree_selection_get_selected(selection, &model, &macroset))
		return;

	macro_tree_store = GAMINGGEAR_MACRO_TREE_STORE(model);

	/* getting macroset_path now, since iter might not be valid anymore once macro is added */
	macroset_path = gtk_tree_model_get_path(model, &macroset);

	if (!gaminggear_macro_tree_store_is_macroset_with_iter(macro_tree_store, &macroset))
		return;

	macro_name = gaminggear_text_dialog(NULL, _("New macro"), _("Please enter name of macro"), NULL);
	if (!name_is_valid(macro_name))
		goto out_free;

	if (!gaminggear_macro_tree_store_is_macro_name_unique(macro_tree_store, &macroset, macro_name))
		goto out_free;

	gaminggear_macro_tree_store_add_macro(macro_tree_store, &macroset, &macro, macro_name);

	/* expand macroset */
	gtk_tree_view_expand_row(GTK_TREE_VIEW(macro_tree_view), macroset_path, FALSE);
	gtk_tree_path_free(macroset_path);

	/* select and activate new macro */
	macro_path = gtk_tree_model_get_path(model, &macro);
	column = gtk_tree_view_get_column(GTK_TREE_VIEW(macro_tree_view), gaminggear_macro_tree_store_name_column());
	gtk_tree_selection_select_iter(selection, &macro);
	gtk_tree_view_row_activated(GTK_TREE_VIEW(macro_tree_view), macro_path, column);
	gtk_tree_path_free(macro_path);

out_free:
	g_free(macro_name);
}

static void popup_rename_cb(GtkMenuItem *item, gpointer user_data) {
	GtkTreeView *tree_view = GTK_TREE_VIEW(user_data);
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	GtkCellRenderer *cell;
	GList *rows, *ptr, *cells;
	GtkTreePath *row;
	GtkTreeViewColumn *column;

	model = gtk_tree_view_get_model(tree_view);
	selection = gtk_tree_view_get_selection(tree_view);
	rows = gtk_tree_selection_get_selected_rows(selection, &model);

	column = gtk_tree_view_get_column(tree_view, gaminggear_macro_tree_store_name_column());
	cells = gtk_cell_layout_get_cells(GTK_CELL_LAYOUT(column));
	cell = cells->data;

	ptr = rows;
	while(ptr != NULL) {
		row = (GtkTreePath *)ptr->data;

		g_object_set(cell, "editable", TRUE, "editable-set", TRUE, NULL);

		gtk_tree_view_scroll_to_cell(tree_view, NULL, column, TRUE, 0, 0);

		/* start editing */
		gtk_tree_view_set_cursor(tree_view, row, column, TRUE);

		g_object_set(cell, "editable", FALSE, "editable-set", FALSE, NULL);

		ptr = ptr->next;
	}

	g_list_free(cells);
	g_list_free_full(rows, (GDestroyNotify)gtk_tree_path_free);
}

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

static void popup_remove_cb(GtkMenuItem *item, gpointer user_data) {
	GaminggearMacroTreeView *macro_tree_view = GAMINGGEAR_MACRO_TREE_VIEW(user_data);
	GtkTreeSelection *selection;
	GtkTreeModel *model;
	GtkTreeIter macro;
	GaminggearMacroTreeStore *macro_tree_store;

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(macro_tree_view));
	if (!gtk_tree_selection_get_selected(selection, &model, &macro))
		return;

	macro_tree_store = GAMINGGEAR_MACRO_TREE_STORE(model);

	if (gaminggear_macro_tree_store_is_macroset_with_iter(macro_tree_store, &macro)) {
		if (should_remove(_("Remove macroset"), _("Do you really want to remove this macroset?")))
			gaminggear_macro_tree_store_remove(macro_tree_store, &macro);
	} else {
		if (should_remove(_("Remove macro"), _("Do you really want to remove this macro?")))
			gaminggear_macro_tree_store_remove(macro_tree_store, &macro);
	}
	removed(macro_tree_view);
}

static gboolean popup_menu_on_right_mouse_button_cb(GtkTreeView *treeview, GdkEventButton *event, gpointer user_data) {
	GaminggearMacroTreeViewPrivate *priv = GAMINGGEAR_MACRO_TREE_VIEW(user_data)->priv;
	GtkTreePath *path_sort;
	GtkMenu *menu;
	GtkTreeSelection *selection;

	if (!(event->type == GDK_BUTTON_PRESS && event->button == 3))
		return FALSE;

	if (gtk_tree_view_get_path_at_pos(treeview, event->x, event->y, &path_sort, NULL, NULL, NULL) == FALSE)
		return FALSE;

	selection = gtk_tree_view_get_selection(treeview);
	gtk_tree_selection_select_path(selection, path_sort);

	if (gaminggear_macro_tree_store_is_macroset_with_path(path_sort))
		menu = priv->macroset_menu;
	else
		menu = priv->macro_menu;

	gtk_tree_path_free(path_sort);

	gtk_menu_popup(menu, NULL, NULL, NULL, NULL, event->button, gdk_event_get_time((GdkEvent*)event));

	return TRUE;
}

static void menu_add_common(GtkWidget *menu, GaminggearMacroTreeView *macro_tree_view) {
	GtkWidget *item;

	item = gtk_image_menu_item_new_with_label(_("Rename"));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(popup_rename_cb), macro_tree_view);

	item = gtk_image_menu_item_new_from_stock(GTK_STOCK_REMOVE, NULL);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(popup_remove_cb), macro_tree_view);
}

static GtkWidget *macro_menu_new(GaminggearMacroTreeView *macro_tree_view) {
	GtkWidget *menu, *item;

	menu = gtk_menu_new();
	menu_add_common(menu, macro_tree_view);

	item = gtk_image_menu_item_new_with_label(_("Copy"));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(popup_copy_macro_cb), macro_tree_view);

	gtk_widget_show_all(menu);
	return menu;
}

static GtkWidget *macroset_menu_new(GaminggearMacroTreeView *macro_tree_view) {
	GtkWidget *menu, *item;

	menu = gtk_menu_new();
	menu_add_common(menu, macro_tree_view);

	item = gtk_image_menu_item_new_with_label(_("Add macro"));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
	g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(popup_add_macro_cb), macro_tree_view);

	gtk_widget_show_all(menu);
	return menu;
}

GtkWidget *gaminggear_macro_tree_view_new(GaminggearMacroTreeStore *store) {
	GaminggearMacroTreeView *macro_tree_view;

	macro_tree_view = g_object_new(GAMINGGEAR_MACRO_TREE_VIEW_TYPE, NULL);

	gtk_tree_view_set_model(GTK_TREE_VIEW(macro_tree_view), GTK_TREE_MODEL(store));

	return GTK_WIDGET(macro_tree_view);
}

static void gaminggear_macro_tree_view_init(GaminggearMacroTreeView *macro_tree_view) {
	GaminggearMacroTreeViewPrivate *priv = GAMINGGEAR_MACRO_TREE_VIEW_GET_PRIVATE(macro_tree_view);
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;

	macro_tree_view->priv = priv;

	priv->macro_menu = GTK_MENU(macro_menu_new(macro_tree_view));
	g_object_ref_sink(priv->macro_menu);

	priv->macroset_menu = GTK_MENU(macroset_menu_new(macro_tree_view));
	g_object_ref_sink(priv->macroset_menu);

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Macro"), renderer, "text", gaminggear_macro_tree_store_name_column(), NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(macro_tree_view), column);

	/* renderer is not set editable. Editing is not started through click, but via popup menu entry */
	g_signal_connect(G_OBJECT(renderer), "edited", G_CALLBACK(edited_cb), macro_tree_view);

	gtk_tree_view_set_enable_search(GTK_TREE_VIEW(macro_tree_view), TRUE);
	gtk_tree_view_set_search_column(GTK_TREE_VIEW(macro_tree_view), gaminggear_macro_tree_store_name_column());
	gtk_tree_view_set_search_equal_func(GTK_TREE_VIEW(macro_tree_view),
			gtk_gaminggear_tree_view_search_string_case_equal_func,
			NULL,
			NULL);

	gtk_drag_source_set(GTK_WIDGET(macro_tree_view), GDK_BUTTON1_MASK, drag_drop_types, G_N_ELEMENTS(drag_drop_types), GDK_ACTION_MOVE);
	gtk_drag_dest_set(GTK_WIDGET(macro_tree_view), GTK_DEST_DEFAULT_ALL, drag_drop_types, G_N_ELEMENTS(drag_drop_types), GDK_ACTION_MOVE);

	g_signal_connect(G_OBJECT(macro_tree_view), "row-activated", G_CALLBACK(row_activated_cb), macro_tree_view);
	g_signal_connect(G_OBJECT(macro_tree_view), "button-press-event", G_CALLBACK(popup_menu_on_right_mouse_button_cb), macro_tree_view);
	g_signal_connect(G_OBJECT(macro_tree_view), "drag-data-received", G_CALLBACK(drag_data_received_cb), NULL);
	g_signal_connect(G_OBJECT(macro_tree_view), "drag-motion", G_CALLBACK(drag_motion_cb), NULL);
	g_signal_connect(G_OBJECT(macro_tree_view), "drag-data-get", G_CALLBACK(drag_data_get_cb), NULL);
}

static void gaminggear_macro_tree_view_finalize(GObject *object) {
	GaminggearMacroTreeViewPrivate *priv = GAMINGGEAR_MACRO_TREE_VIEW(object)->priv;
	g_object_unref(priv->macro_menu);
	g_object_unref(priv->macroset_menu);
	G_OBJECT_CLASS(gaminggear_macro_tree_view_parent_class)->finalize(object);
}

static void gaminggear_macro_tree_view_class_init(GaminggearMacroTreeViewClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = (GObjectClass*)klass;

	gobject_class->finalize = gaminggear_macro_tree_view_finalize;

	g_type_class_add_private(klass, sizeof(GaminggearMacroTreeViewPrivate));

	signals[SELECTED] = g_signal_new("selected",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			G_STRUCT_OFFSET(GaminggearMacroTreeViewClass, selected),
			NULL, NULL, g_cclosure_marshal_VOID__OBJECT, G_TYPE_NONE,
			1, GTK_TYPE_TREE_ROW_REFERENCE);


	signals[REMOVED] = g_signal_new("removed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			G_STRUCT_OFFSET(GaminggearMacroTreeViewClass, removed),
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}

GaminggearMacroTreeStore *gaminggear_macro_tree_view_get_store(GaminggearMacroTreeView *macro_tree_view) {
	return GAMINGGEAR_MACRO_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(macro_tree_view)));
}
