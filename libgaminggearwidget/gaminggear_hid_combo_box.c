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

#include "gaminggear/gaminggear_hid_combo_box.h"
#include "gaminggear/gdk_key_translations.h"
#include "gaminggear/hid_uid.h"
#include "gtk_gaminggear_helper.h"

#define GAMINGGEAR_HID_COMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GAMINGGEAR_HID_COMBO_BOX_TYPE, GaminggearHidComboBoxClass))
#define IS_GAMINGGEAR_HID_COMBO_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GAMINGGEAR_HID_COMBO_BOX_TYPE))
#define GAMINGGEAR_HID_COMBO_BOX_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), GAMINGGEAR_HID_COMBO_BOX_TYPE, GaminggearHidComboBoxClass))

typedef struct _GaminggearHidComboBoxClass GaminggearHidComboBoxClass;

struct _GaminggearHidComboBox {
	GtkComboBox parent;
};

struct _GaminggearHidComboBoxClass {
	GtkComboBoxClass parent_class;
	GtkTreeModel *model;
};

enum {
	TITLE_COLUMN,
	HID_COLUMN,
	N_COLUMNS
};

G_DEFINE_TYPE(GaminggearHidComboBox, gaminggear_hid_combo_box, GTK_TYPE_COMBO_BOX);

static void set_model(GaminggearHidComboBox *hid_combo_box) {
	GaminggearHidComboBoxClass *hid_combo_box_class = GAMINGGEAR_HID_COMBO_BOX_GET_CLASS(hid_combo_box);
	gtk_combo_box_set_model(GTK_COMBO_BOX(hid_combo_box), hid_combo_box_class->model);
}

static void set_renderer(GaminggearHidComboBox *hid_combo_box) {
	GtkCellRenderer *renderer;
	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(hid_combo_box), renderer, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(hid_combo_box), renderer, "text", TITLE_COLUMN, NULL);
}

GtkWidget *gaminggear_hid_combo_box_new(void) {
	GaminggearHidComboBox *hid_combo_box;

	hid_combo_box = GAMINGGEAR_HID_COMBO_BOX(g_object_new(GAMINGGEAR_HID_COMBO_BOX_TYPE, NULL));

	set_model(hid_combo_box);
	set_renderer(hid_combo_box);

	return GTK_WIDGET(hid_combo_box);
}

static void gaminggear_hid_combo_box_init(GaminggearHidComboBox *hid_combo_box) {}

static void append_hid_usage_id(GtkListStore *store, guint hid_usage_id) {
	GtkTreeIter iter;
	gchar *name;

	name = gaminggear_hid_to_name(hid_usage_id);
	gtk_gaminggear_list_store_append_with_values(store, &iter, TITLE_COLUMN, name, HID_COLUMN, hid_usage_id, -1);
	g_free(name);
}

static GtkTreeModel *model_new(void) {
	GtkListStore *store;
	guint hid_usage_id;

	store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING, G_TYPE_INT);

	for (hid_usage_id = HID_UID_KB_A; hid_usage_id <= HID_UID_KB_APPLICATION; ++hid_usage_id)
		append_hid_usage_id(store, hid_usage_id);

	for (hid_usage_id = HID_UID_KB_LEFT_CONTROL; hid_usage_id <= HID_UID_KB_RIGHT_GUI; ++hid_usage_id)
		append_hid_usage_id(store, hid_usage_id);

	return GTK_TREE_MODEL(store);
}

static void gaminggear_hid_combo_box_class_init(GaminggearHidComboBoxClass *klass) {
	klass->model = model_new();
}

static gint gaminggear_hid_combo_box_model_get_type(GtkTreeModel *model, GtkTreeIter *iter) {
	gint type;
	gtk_tree_model_get(model, iter, HID_COLUMN, &type, -1);
	return type;
}

gint gaminggear_hid_combo_box_get_value(GaminggearHidComboBox *hid_combo_box) {
	GtkTreeIter iter;
	gboolean valid;

	valid = gtk_combo_box_get_active_iter(GTK_COMBO_BOX(hid_combo_box), &iter);
	if (!valid)
		return 0;

	return gaminggear_hid_combo_box_model_get_type(gtk_combo_box_get_model(GTK_COMBO_BOX(hid_combo_box)), &iter);
}

void gaminggear_hid_combo_box_set_value(GaminggearHidComboBox *hid_combo_box, gint new_value) {
	GtkTreeIter iter;
	GtkTreeModel *model;
	gboolean valid;

	model = gtk_combo_box_get_model(GTK_COMBO_BOX(hid_combo_box));
	valid = gtk_gaminggear_tree_model_iter_find_int(model, HID_COLUMN, new_value, &iter, NULL);
	if (valid)
		gtk_combo_box_set_active_iter(GTK_COMBO_BOX(hid_combo_box), &iter);
}
