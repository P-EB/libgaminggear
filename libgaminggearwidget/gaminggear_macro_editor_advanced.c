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

#include "gaminggear_macro_editor_advanced.h"
#include "gaminggear_macro_editor_advanced_list_store.h"
#include "gaminggear_macro_editor_advanced_table.h"
#include "i18n-lib.h"

#define GAMINGGEAR_MACRO_EDITOR_ADVANCED_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GAMINGGEAR_MACRO_EDITOR_ADVANCED_TYPE, GaminggearMacroEditorAdvancedClass))
#define IS_GAMINGGEAR_MACRO_EDITOR_ADVANCED_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GAMINGGEAR_MACRO_EDITOR_ADVANCED_TYPE))
#define GAMINGGEAR_MACRO_EDITOR_ADVANCED_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), GAMINGGEAR_MACRO_EDITOR_ADVANCED_TYPE, GaminggearMacroEditorAdvancedPrivate))

typedef struct _GaminggearMacroEditorAdvancedClass GaminggearMacroEditorAdvancedClass;

struct _GaminggearMacroEditorAdvancedClass {
	GtkScrolledWindowClass parent_class;
};

struct _GaminggearMacroEditorAdvancedPrivate {
	GaminggearMacroEditorAdvancedListStore *list_store;
	GaminggearMacroEditorAdvancedTable *table;
};

static gchar const *gaminggear_macro_editor_advanced_name(GaminggearMacroEditorInterface *self) {
	return _("Advanced");
}

static void gaminggear_macro_editor_advanced_set_keystrokes(GaminggearMacroEditorInterface *self, GaminggearMacroKeystrokes const *keystrokes) {
	GaminggearMacroEditorAdvancedPrivate *priv = GAMINGGEAR_MACRO_EDITOR_ADVANCED(self)->priv;
	gaminggear_macro_editor_advanced_list_store_set_keystrokes(priv->list_store, keystrokes);
	gaminggear_macro_editor_advanced_list_store_set_modified(priv->list_store, FALSE);
}

static GaminggearMacroKeystrokes *gaminggear_macro_editor_advanced_get_keystrokes(GaminggearMacroEditorInterface *self) {
	GaminggearMacroEditorAdvancedPrivate *priv = GAMINGGEAR_MACRO_EDITOR_ADVANCED(self)->priv;
	return gaminggear_macro_editor_advanced_list_store_get_keystrokes(priv->list_store);
}

static void gaminggear_macro_editor_advanced_clear(GaminggearMacroEditorInterface *self) {
	GaminggearMacroEditorAdvancedPrivate *priv = GAMINGGEAR_MACRO_EDITOR_ADVANCED(self)->priv;
	gaminggear_macro_editor_advanced_list_store_clear(priv->list_store);
}

static gboolean gaminggear_macro_editor_advanced_empty(GaminggearMacroEditorInterface *self) {
	GaminggearMacroEditorAdvancedPrivate *priv = GAMINGGEAR_MACRO_EDITOR_ADVANCED(self)->priv;
	return gaminggear_macro_editor_advanced_list_store_empty(priv->list_store);
}

static void scrolled_window_scroll_to_end(GtkScrolledWindow *window) {
	GtkAdjustment *adjustment;
	adjustment = gtk_scrolled_window_get_vadjustment(window);
	gtk_adjustment_set_value(adjustment, gtk_adjustment_get_upper(adjustment) - gtk_adjustment_get_page_size(adjustment));
}

static void gaminggear_macro_editor_advanced_add_keystroke(GaminggearMacroEditorInterface *self, guint key, guint action, glong rel_time) {
	GaminggearMacroEditorAdvancedPrivate *priv = GAMINGGEAR_MACRO_EDITOR_ADVANCED(self)->priv;
	gaminggear_macro_editor_advanced_list_store_add_keystroke(priv->list_store, key, action, rel_time);
	scrolled_window_scroll_to_end(GTK_SCROLLED_WINDOW(self));
}

static gboolean gaminggear_macro_editor_advanced_get_modified(GaminggearMacroEditorInterface *self) {
	GaminggearMacroEditorAdvancedPrivate *priv = GAMINGGEAR_MACRO_EDITOR_ADVANCED(self)->priv;
	return gaminggear_macro_editor_advanced_list_store_get_modified(priv->list_store);
}

static void gaminggear_macro_editor_advanced_set_modified(GaminggearMacroEditorInterface *self, gboolean new_value) {
	GaminggearMacroEditorAdvancedPrivate *priv = GAMINGGEAR_MACRO_EDITOR_ADVANCED(self)->priv;
	return gaminggear_macro_editor_advanced_list_store_set_modified(priv->list_store, new_value);
}

static void gaminggear_macro_editor_advanced_interface_init(GaminggearMacroEditorInterfaceInterface *iface) {
	iface->name = gaminggear_macro_editor_advanced_name;
	iface->set_keystrokes = gaminggear_macro_editor_advanced_set_keystrokes;
	iface->get_keystrokes = gaminggear_macro_editor_advanced_get_keystrokes;
	iface->clear = gaminggear_macro_editor_advanced_clear;
	iface->empty = gaminggear_macro_editor_advanced_empty;
	iface->add_keystroke = gaminggear_macro_editor_advanced_add_keystroke;
	iface->get_modified = gaminggear_macro_editor_advanced_get_modified;
	iface->set_modified = gaminggear_macro_editor_advanced_set_modified;
}

G_DEFINE_TYPE_WITH_CODE(GaminggearMacroEditorAdvanced, gaminggear_macro_editor_advanced, GTK_TYPE_SCROLLED_WINDOW,
		G_IMPLEMENT_INTERFACE(GAMINGGEAR_MACRO_EDITOR_INTERFACE_TYPE, gaminggear_macro_editor_advanced_interface_init));


GtkWidget *gaminggear_macro_editor_advanced_new(void) {
	GaminggearMacroEditorAdvanced *macro_editor_advanced;

	macro_editor_advanced = g_object_new(GAMINGGEAR_MACRO_EDITOR_ADVANCED_TYPE,
			"hscrollbar-policy", GTK_POLICY_AUTOMATIC,
			"vscrollbar-policy", GTK_POLICY_AUTOMATIC,
			NULL);

	/* don't do this earlier or scrolled_window assertions GTK_IS_RANGE will fail */
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(macro_editor_advanced),
			GTK_WIDGET(macro_editor_advanced->priv->table));

	return GTK_WIDGET(macro_editor_advanced);
}

static void modified_cb(GaminggearMacroEditorAdvancedListStore *store, gpointer user_data) {
	gaminggear_macro_editor_interface_modified(GAMINGGEAR_MACRO_EDITOR_INTERFACE(user_data));
}

static void empty_changed_cb(GaminggearMacroEditorAdvancedListStore *store, gpointer user_data) {
	gaminggear_macro_editor_interface_empty_changed(GAMINGGEAR_MACRO_EDITOR_INTERFACE(user_data));
}

static void gaminggear_macro_editor_advanced_init(GaminggearMacroEditorAdvanced *macro_editor_advanced) {
	GaminggearMacroEditorAdvancedPrivate *priv = GAMINGGEAR_MACRO_EDITOR_ADVANCED_GET_PRIVATE(macro_editor_advanced);

	macro_editor_advanced->priv = priv;

	priv->list_store = gaminggear_macro_editor_advanced_list_store_new();
	g_signal_connect(G_OBJECT(priv->list_store), "modified", G_CALLBACK(modified_cb), macro_editor_advanced);
	g_signal_connect(G_OBJECT(priv->list_store), "empty-changed", G_CALLBACK(empty_changed_cb), macro_editor_advanced);

	priv->table = GAMINGGEAR_MACRO_EDITOR_ADVANCED_TABLE(gaminggear_macro_editor_advanced_table_new(priv->list_store));
	g_object_ref_sink(priv->table);

	gtk_widget_show(GTK_WIDGET(priv->table));
}

static void gaminggear_macro_editor_advanced_finalize(GObject *object) {
	GaminggearMacroEditorAdvancedPrivate *priv = GAMINGGEAR_MACRO_EDITOR_ADVANCED(object)->priv;
	g_object_unref(priv->list_store);
	g_object_unref(priv->table);
	G_OBJECT_CLASS(gaminggear_macro_editor_advanced_parent_class)->finalize(object);
}

static void gaminggear_macro_editor_advanced_class_init(GaminggearMacroEditorAdvancedClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->finalize = gaminggear_macro_editor_advanced_finalize;

	g_type_class_add_private(klass, sizeof(GaminggearMacroEditorAdvancedPrivate));
}
