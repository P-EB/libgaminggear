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

#include "gaminggear_macro_editor_advanced_table.h"
#include "gaminggear_macro_editor_advanced_list_store.h"
#include "gaminggear_dscale.h"
#include "gaminggear_ruler.h"
#include "gaminggear_helper.h"
#include "gaminggearwidget_helper.h"
#include <stdlib.h>

#define GAMINGGEAR_MACRO_EDITOR_ADVANCED_TABLE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GAMINGGEAR_MACRO_EDITOR_ADVANCED_TABLE_TYPE, GaminggearMacroEditorAdvancedTableClass))
#define IS_GAMINGGEAR_MACRO_EDITOR_ADVANCED_TABLE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GAMINGGEAR_MACRO_EDITOR_ADVANCED_TABLE_TYPE))
#define GAMINGGEAR_MACRO_EDITOR_ADVANCED_TABLE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), GAMINGGEAR_MACRO_EDITOR_ADVANCED_TABLE_TYPE, GaminggearMacroEditorAdvancedTablePrivate))

typedef struct _GaminggearMacroEditorAdvancedTableClass GaminggearMacroEditorAdvancedTableClass;
typedef struct _AbsoluteKeystroke AbsoluteKeystroke;

struct _GaminggearMacroEditorAdvancedTableClass {
	GtkTableClass parent_class;
};

struct _AbsoluteKeystroke {
	guint key;
	guint action;
	GtkLabel *label;
	GtkSpinButton *lower;
	GtkSpinButton *upper;
	GaminggearDscale *dscale;
};

struct _GaminggearMacroEditorAdvancedTablePrivate {
	GaminggearMacroEditorAdvancedListStore *store;
};

G_DEFINE_TYPE(GaminggearMacroEditorAdvancedTable, gaminggear_macro_editor_advanced_table, GTK_TYPE_TABLE);

enum {
	GAMINGGEAR_MACRO_EDITOR_ADVANCED_TABLE_LABEL_COLUMN,
	GAMINGGEAR_MACRO_EDITOR_ADVANCED_TABLE_LOWER_COLUMN,
	GAMINGGEAR_MACRO_EDITOR_ADVANCED_TABLE_SCALE_COLUMN,
	GAMINGGEAR_MACRO_EDITOR_ADVANCED_TABLE_UPPER_COLUMN,
	GAMINGGEAR_MACRO_EDITOR_ADVANCED_TABLE_N_COLUMNS,
};

static void table_add_header_row(GaminggearMacroEditorAdvancedTable *table) {
	GtkWidget *ruler;
	ruler = gaminggear_ruler_new();
	g_object_set(G_OBJECT(ruler),
			"min", 0.0,
			"interval", 1.0,
			NULL);

	gtk_table_attach(GTK_TABLE(table), ruler,
			GAMINGGEAR_MACRO_EDITOR_ADVANCED_TABLE_SCALE_COLUMN, GAMINGGEAR_MACRO_EDITOR_ADVANCED_TABLE_SCALE_COLUMN + 1,
			0, 1,
			GTK_EXPAND | GTK_FILL, 0,
			0, 0);
	gtk_widget_show(ruler);
}

static void set_ruler_max(GaminggearMacroEditorAdvancedTable *macro_editor_advanced_table, gdouble max) {
	GList *children;
	GList *element;

	children = gtk_container_get_children(GTK_CONTAINER(macro_editor_advanced_table));
	for (element = children; element; element = g_list_next(element)) {
		if (IS_GAMINGGEAR_RULER(element->data)) {
			gaminggear_ruler_set_max(element->data, max);
			/* There is only one ruler */
			break;
		}
	}
	g_list_free(children);
}

static void prepare_table(GaminggearMacroEditorAdvancedTable *table) {
	table_add_header_row(table);
	gaminggear_macro_editor_advanced_list_store_set_all_max(table->priv->store, 0);
}

static void empty_changed_cb(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store, gpointer user_data) {
	GaminggearMacroEditorAdvancedTable *macro_editor_advanced_table = GAMINGGEAR_MACRO_EDITOR_ADVANCED_TABLE(user_data);
	if (gaminggear_macro_editor_advanced_list_store_empty(macro_editor_advanced_list_store)) {
		gtk_gaminggear_table_clear(GTK_TABLE(macro_editor_advanced_table));
		prepare_table(macro_editor_advanced_table);
	}
}

static void max_changed_cb(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store, gpointer user_data) {
	GaminggearMacroEditorAdvancedTable *macro_editor_advanced_table = GAMINGGEAR_MACRO_EDITOR_ADVANCED_TABLE(user_data);
	gdouble max;

	max = gaminggear_macro_editor_advanced_list_store_get_all_max(macro_editor_advanced_list_store);
	set_ruler_max(macro_editor_advanced_table, max);
}

static void keypress_added_cb(GaminggearMacroEditorAdvancedListStore *store, GtkTreeIter *iter, gpointer user_data) {
	GaminggearMacroEditorAdvancedTable *macro_editor_advanced_table = GAMINGGEAR_MACRO_EDITOR_ADVANCED_TABLE(user_data);
	GtkTable *table = GTK_TABLE(macro_editor_advanced_table);
	guint top_attach;
	GtkWidget *label;
	GtkWidget *lower;
	GtkWidget *upper;
	GtkWidget *scale;

#if (GTK_CHECK_VERSION(2, 22, 0))
	gtk_table_get_size(GTK_TABLE(macro_editor_advanced_table), &top_attach, NULL);
#else
	g_object_get(G_OBJECT(macro_editor_advanced_table), "n-rows", &top_attach, NULL);
#endif

	label = gaminggear_macro_editor_advanced_list_store_get_label_widget(store, iter);
	lower = gaminggear_macro_editor_advanced_list_store_get_lower_widget(store, iter);
	upper = gaminggear_macro_editor_advanced_list_store_get_upper_widget(store, iter);
	scale = gaminggear_macro_editor_advanced_list_store_get_scale_widget(store, iter);

	gtk_table_attach(table, label,
			GAMINGGEAR_MACRO_EDITOR_ADVANCED_TABLE_LABEL_COLUMN, GAMINGGEAR_MACRO_EDITOR_ADVANCED_TABLE_LABEL_COLUMN + 1,
			top_attach, top_attach + 1,
			0, 0,
			0, 0);
	gtk_widget_show(label);

	gtk_table_attach(table, lower,
			GAMINGGEAR_MACRO_EDITOR_ADVANCED_TABLE_LOWER_COLUMN, GAMINGGEAR_MACRO_EDITOR_ADVANCED_TABLE_LOWER_COLUMN + 1,
			top_attach, top_attach + 1,
			0, 0,
			0, 0);
	gtk_widget_show(lower);

	gtk_table_attach(table, upper,
			GAMINGGEAR_MACRO_EDITOR_ADVANCED_TABLE_UPPER_COLUMN, GAMINGGEAR_MACRO_EDITOR_ADVANCED_TABLE_UPPER_COLUMN + 1,
			top_attach, top_attach + 1,
			0, 0,
			0, 0);
	gtk_widget_show(upper);

	gtk_table_attach(table, scale,
			GAMINGGEAR_MACRO_EDITOR_ADVANCED_TABLE_SCALE_COLUMN, GAMINGGEAR_MACRO_EDITOR_ADVANCED_TABLE_SCALE_COLUMN + 1,
			top_attach, top_attach + 1,
			GTK_EXPAND | GTK_FILL, 0,
			0, 0);
	gtk_widget_show(scale);
}

GtkWidget *gaminggear_macro_editor_advanced_table_new(GaminggearMacroEditorAdvancedListStore *store) {
	GaminggearMacroEditorAdvancedTable *macro_editor_advanced_table;

	macro_editor_advanced_table = g_object_new(GAMINGGEAR_MACRO_EDITOR_ADVANCED_TABLE_TYPE,
			"n-columns", GAMINGGEAR_MACRO_EDITOR_ADVANCED_TABLE_N_COLUMNS,
			"n-rows", 1,
			"homogeneous", FALSE,
			NULL);

	g_object_ref(store);
	macro_editor_advanced_table->priv->store = store;
	g_signal_connect(G_OBJECT(store), "max-changed", G_CALLBACK(max_changed_cb), macro_editor_advanced_table);
	g_signal_connect(G_OBJECT(store), "keypress-added", G_CALLBACK(keypress_added_cb), macro_editor_advanced_table);
	g_signal_connect(G_OBJECT(store), "empty-changed", G_CALLBACK(empty_changed_cb), macro_editor_advanced_table);

	prepare_table(macro_editor_advanced_table);

	return GTK_WIDGET(macro_editor_advanced_table);
}

static void gaminggear_macro_editor_advanced_table_init(GaminggearMacroEditorAdvancedTable *macro_editor_advanced_table) {
	GaminggearMacroEditorAdvancedTablePrivate *priv = GAMINGGEAR_MACRO_EDITOR_ADVANCED_TABLE_GET_PRIVATE(macro_editor_advanced_table);
	macro_editor_advanced_table->priv = priv;
}

static void gaminggear_macro_editor_advanced_table_finalize(GObject *object) {
	GaminggearMacroEditorAdvancedTablePrivate *priv = GAMINGGEAR_MACRO_EDITOR_ADVANCED_TABLE(object)->priv;
	g_object_unref(priv->store);
	G_OBJECT_CLASS(gaminggear_macro_editor_advanced_table_parent_class)->finalize(object);
}

static void gaminggear_macro_editor_advanced_table_class_init(GaminggearMacroEditorAdvancedTableClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->finalize = gaminggear_macro_editor_advanced_table_finalize;

	g_type_class_add_private(klass, sizeof(GaminggearMacroEditorAdvancedTablePrivate));
}
