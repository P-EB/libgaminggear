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

#include "gaminggear_macro_editor_basic.h"
#include "gaminggear_macro_editor_basic_list_store.h"
#include "gaminggear_macro_editor_basic_tree_view.h"
#include "i18n-lib.h"

#define GAMINGGEAR_MACRO_EDITOR_BASIC_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GAMINGGEAR_MACRO_EDITOR_BASIC_TYPE, GaminggearMacroEditorBasicClass))
#define IS_GAMINGGEAR_MACRO_EDITOR_BASIC_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GAMINGGEAR_MACRO_EDITOR_BASIC_TYPE))
#define GAMINGGEAR_MACRO_EDITOR_BASIC_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), GAMINGGEAR_MACRO_EDITOR_BASIC_TYPE, GaminggearMacroEditorBasicPrivate))

typedef struct _GaminggearMacroEditorBasicClass GaminggearMacroEditorBasicClass;

struct _GaminggearMacroEditorBasicClass {
	GtkScrolledWindowClass parent_class;
};

struct _GaminggearMacroEditorBasicPrivate {
	GaminggearMacroEditorBasicListStore *list_store;
	GaminggearMacroEditorBasicTreeView *tree_view;
};

static gchar const *gaminggear_macro_editor_basic_name(GaminggearMacroEditorInterface *self) {
	return _("Basic");
}

static void gaminggear_macro_editor_basic_set_keystrokes(GaminggearMacroEditorInterface *self, GaminggearMacroKeystrokes const *keystrokes) {
	GaminggearMacroEditorBasicPrivate *priv = GAMINGGEAR_MACRO_EDITOR_BASIC(self)->priv;
	gaminggear_macro_editor_basic_list_store_set_keystrokes(priv->list_store, keystrokes);
	gaminggear_macro_editor_basic_list_store_set_modified(priv->list_store, FALSE);
}

static GaminggearMacroKeystrokes *gaminggear_macro_editor_basic_get_keystrokes(GaminggearMacroEditorInterface *self) {
	GaminggearMacroEditorBasicPrivate *priv = GAMINGGEAR_MACRO_EDITOR_BASIC(self)->priv;
	return gaminggear_macro_editor_basic_list_store_get_keystrokes(priv->list_store);
}

static void gaminggear_macro_editor_basic_clear(GaminggearMacroEditorInterface *self) {
	GaminggearMacroEditorBasicPrivate *priv = GAMINGGEAR_MACRO_EDITOR_BASIC(self)->priv;
	gaminggear_macro_editor_basic_list_store_clear(priv->list_store);
}

static gboolean gaminggear_macro_editor_basic_empty(GaminggearMacroEditorInterface *self) {
	GaminggearMacroEditorBasicPrivate *priv = GAMINGGEAR_MACRO_EDITOR_BASIC(self)->priv;
	return gaminggear_macro_editor_basic_list_store_empty(priv->list_store);
}

/* calling this directly leads to wrong position */
static gboolean tree_view_scroll_to_end(gpointer user_data) {
	GtkTreeView *tree_view = GTK_TREE_VIEW(user_data);
	GtkAdjustment *adjustment = gtk_tree_view_get_vadjustment(tree_view);
	gtk_adjustment_set_value(adjustment, gtk_adjustment_get_upper(adjustment) - gtk_adjustment_get_page_size(adjustment));
	return FALSE;
}

static void gaminggear_macro_editor_basic_add_keystroke(GaminggearMacroEditorInterface *self, guint key, guint action, glong rel_time) {
	GaminggearMacroEditorBasicPrivate *priv = GAMINGGEAR_MACRO_EDITOR_BASIC(self)->priv;
	gaminggear_macro_editor_basic_tree_view_add_keystroke(priv->tree_view, key, action, rel_time);
	g_idle_add(tree_view_scroll_to_end, priv->tree_view);
}

static gboolean gaminggear_macro_editor_basic_get_modified(GaminggearMacroEditorInterface *self) {
	GaminggearMacroEditorBasicPrivate *priv = GAMINGGEAR_MACRO_EDITOR_BASIC(self)->priv;
	return gaminggear_macro_editor_basic_list_store_get_modified(priv->list_store);
}

static void gaminggear_macro_editor_basic_set_modified(GaminggearMacroEditorInterface *self, gboolean new_value) {
	GaminggearMacroEditorBasicPrivate *priv = GAMINGGEAR_MACRO_EDITOR_BASIC(self)->priv;
	return gaminggear_macro_editor_basic_list_store_set_modified(priv->list_store, new_value);
}

static void gaminggear_macro_editor_basic_interface_init(GaminggearMacroEditorInterfaceInterface *iface) {
	iface->name = gaminggear_macro_editor_basic_name;
	iface->set_keystrokes = gaminggear_macro_editor_basic_set_keystrokes;
	iface->get_keystrokes = gaminggear_macro_editor_basic_get_keystrokes;
	iface->clear = gaminggear_macro_editor_basic_clear;
	iface->empty = gaminggear_macro_editor_basic_empty;
	iface->add_keystroke = gaminggear_macro_editor_basic_add_keystroke;
	iface->get_modified = gaminggear_macro_editor_basic_get_modified;
	iface->set_modified = gaminggear_macro_editor_basic_set_modified;
}

G_DEFINE_TYPE_WITH_CODE(GaminggearMacroEditorBasic, gaminggear_macro_editor_basic, GTK_TYPE_SCROLLED_WINDOW,
		G_IMPLEMENT_INTERFACE(GAMINGGEAR_MACRO_EDITOR_INTERFACE_TYPE, gaminggear_macro_editor_basic_interface_init));

GtkWidget *gaminggear_macro_editor_basic_new(void) {
	GaminggearMacroEditorBasic *macro_editor_basic;

	macro_editor_basic = g_object_new(GAMINGGEAR_MACRO_EDITOR_BASIC_TYPE,
			"hscrollbar-policy", GTK_POLICY_AUTOMATIC,
			"vscrollbar-policy", GTK_POLICY_AUTOMATIC,
			NULL);

	/* don't do this earlier or scrolled_window assertions GTK_IS_RANGE will fail */
	gtk_container_add(GTK_CONTAINER(macro_editor_basic), GTK_WIDGET(macro_editor_basic->priv->tree_view));

	return GTK_WIDGET(macro_editor_basic);
}

static void modified_cb(GaminggearMacroEditorBasicListStore *store, gpointer user_data) {
	gaminggear_macro_editor_interface_modified(GAMINGGEAR_MACRO_EDITOR_INTERFACE(user_data));
}

static void empty_changed_cb(GaminggearMacroEditorBasicListStore *store, gpointer user_data) {
	gaminggear_macro_editor_interface_empty_changed(GAMINGGEAR_MACRO_EDITOR_INTERFACE(user_data));
}

static void gaminggear_macro_editor_basic_init(GaminggearMacroEditorBasic *macro_editor_basic) {
	GaminggearMacroEditorBasicPrivate *priv = GAMINGGEAR_MACRO_EDITOR_BASIC_GET_PRIVATE(macro_editor_basic);

	macro_editor_basic->priv = priv;

	priv->list_store = gaminggear_macro_editor_basic_list_store_new();
	g_signal_connect(G_OBJECT(priv->list_store), "modified", G_CALLBACK(modified_cb), macro_editor_basic);
	g_signal_connect(G_OBJECT(priv->list_store), "empty-changed", G_CALLBACK(empty_changed_cb), macro_editor_basic);

	priv->tree_view = GAMINGGEAR_MACRO_EDITOR_BASIC_TREE_VIEW(gaminggear_macro_editor_basic_tree_view_new(priv->list_store));
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(priv->tree_view), FALSE);

	gtk_widget_show(GTK_WIDGET(priv->tree_view));
}

static void gaminggear_macro_editor_basic_finalize(GObject *object) {
	GaminggearMacroEditorBasicPrivate *priv = GAMINGGEAR_MACRO_EDITOR_BASIC(object)->priv;
	g_object_unref(priv->list_store);
	G_OBJECT_CLASS(gaminggear_macro_editor_basic_parent_class)->finalize(object);
}

static void gaminggear_macro_editor_basic_class_init(GaminggearMacroEditorBasicClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->finalize = gaminggear_macro_editor_basic_finalize;

	g_type_class_add_private(klass, sizeof(GaminggearMacroEditorBasicPrivate));
}
