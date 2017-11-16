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

#include "gaminggear_macro_combo_box.h"
#include "gaminggear_macro_tree_store.h"
#include "gaminggearwidget_helper.h"

static void cell_data_func(GtkCellLayout *cell_layout, GtkCellRenderer *cell, GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data) {
	/* disable toplevel elements */
	g_object_set(cell, "sensitive", gtk_gaminggear_tree_model_iter_has_parent(tree_model, iter), NULL);
}

GtkWidget *gaminggear_macro_combo_box_new(void) {
	GtkCellRenderer *cell;
	GtkWidget *combo_box;
	GaminggearMacroTreeStore *store;

	store = gaminggear_macro_tree_store_new();
	combo_box = gtk_combo_box_new_with_model(GTK_TREE_MODEL(store));
	g_object_unref(store);

	cell = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo_box), cell, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo_box), cell, "text",
			gaminggear_macro_tree_store_name_column(), NULL);
	gtk_cell_layout_set_cell_data_func(GTK_CELL_LAYOUT(combo_box), cell,
			cell_data_func, NULL, NULL);
	return combo_box;
}

void gaminggear_macro_combo_box_set_macros(GtkComboBox *combo_box, GaminggearMacros *macros) {
	GaminggearMacroTreeStore *store;

	store = GAMINGGEAR_MACRO_TREE_STORE(gtk_combo_box_get_model(combo_box));
	gaminggear_macro_tree_store_clear(store);
	gaminggear_macro_tree_store_add_macros(store, macros);
}

GaminggearMacro *gaminggear_macro_combo_box_get_value(GtkComboBox *combo_box) {
	GtkTreeIter iter;
	GaminggearMacroTreeStore *store;
	gboolean valid;

	store = GAMINGGEAR_MACRO_TREE_STORE(gtk_combo_box_get_model(combo_box));

	valid = gtk_combo_box_get_active_iter(combo_box, &iter);
	if (!valid)
		valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), &iter);

	if (valid)
		return gaminggear_macro_tree_store_get_macro(store, &iter);
	else
		return NULL;
}
