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

#include "gaminggear_macro_editor_macros_frame.h"
#include "gaminggear_macro_tree_view.h"
#include "i18n-lib.h"

#define GAMINGGEAR_MACRO_EDITOR_MACROS_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GAMINGGEAR_MACRO_EDITOR_MACROS_FRAME_TYPE, GaminggearMacroEditorMacrosFrameClass))
#define IS_GAMINGGEAR_MACRO_EDITOR_MACROS_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GAMINGGEAR_MACRO_EDITOR_MACROS_FRAME_TYPE))
#define GAMINGGEAR_MACRO_EDITOR_MACROS_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), GAMINGGEAR_MACRO_EDITOR_MACROS_FRAME_TYPE, GaminggearMacroEditorMacrosFramePrivate))

typedef struct _GaminggearMacroEditorMacrosFrameClass GaminggearMacroEditorMacrosFrameClass;
typedef struct _AbsoluteKeystroke AbsoluteKeystroke;

struct _GaminggearMacroEditorMacrosFrameClass {
	GtkFrameClass parent_class;
	void(*removed)(GaminggearMacroEditorMacrosFrame *frame);
	void(*selected)(GaminggearMacroEditorMacrosFrame *frame, GtkTreeRowReference *reference);
};

struct _GaminggearMacroEditorMacrosFramePrivate {
	GaminggearMacroTreeView *tree_view;
};

G_DEFINE_TYPE(GaminggearMacroEditorMacrosFrame, gaminggear_macro_editor_macros_frame, GTK_TYPE_FRAME);

enum {
	REMOVED,
	SELECTED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

GtkWidget *gaminggear_macro_editor_macros_frame_new(void) {
	GaminggearMacroEditorMacrosFrame *macros_frame;

	macros_frame = g_object_new(GAMINGGEAR_MACRO_EDITOR_MACROS_FRAME_TYPE,
			"label", _("Macros"),
			NULL);

	return GTK_WIDGET(macros_frame);
}

static void removed(GaminggearMacroEditorMacrosFrame *macros_frame) {
	g_signal_emit((gpointer)macros_frame, signals[REMOVED], 0);
}

static void selected(GaminggearMacroEditorMacrosFrame *macros_frame, GtkTreeRowReference *reference) {
	g_signal_emit((gpointer)macros_frame, signals[SELECTED], 0, reference);
}

static void macro_selected_cb(GaminggearMacroTreeView *tree_view, GtkTreeRowReference *reference, gpointer user_data) {
	GaminggearMacroEditorMacrosFrame *macros_frame = GAMINGGEAR_MACRO_EDITOR_MACROS_FRAME(user_data);
	selected(macros_frame, reference);
}

static void macro_removed_cb(GaminggearMacroTreeView *tree_view, gpointer user_data) {
	GaminggearMacroEditorMacrosFrame *macros_frame = GAMINGGEAR_MACRO_EDITOR_MACROS_FRAME(user_data);
	removed(macros_frame);
}

static void add_macroset_clicked_cb(GtkButton *button, gpointer user_data) {
	GaminggearMacroEditorMacrosFramePrivate *priv = GAMINGGEAR_MACRO_EDITOR_MACROS_FRAME(user_data)->priv;
	gaminggear_macro_tree_view_add_macroset(priv->tree_view);
}

static void gaminggear_macro_editor_macros_frame_init(GaminggearMacroEditorMacrosFrame *macros_frame) {
	GaminggearMacroEditorMacrosFramePrivate *priv = GAMINGGEAR_MACRO_EDITOR_MACROS_FRAME_GET_PRIVATE(macros_frame);
	GaminggearMacroTreeStore *store;
	GtkWidget *vbox;
	GtkWidget *scrolled_window;
	GtkWidget *hbox;
	GtkWidget *button;

	macros_frame->priv = priv;

	store = gaminggear_macro_tree_store_new();
	priv->tree_view = GAMINGGEAR_MACRO_TREE_VIEW(gaminggear_macro_tree_view_new(store));
	g_object_ref_sink(priv->tree_view);
	g_object_unref(store);

	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(priv->tree_view), FALSE);

	vbox = gtk_vbox_new(FALSE, 0);
	hbox = gtk_hbox_new(FALSE, 0);
	scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	button = gtk_button_new_with_label(_("Add macroset"));

	gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(priv->tree_view));

	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

	gtk_container_add(GTK_CONTAINER(macros_frame), vbox);

	g_signal_connect(G_OBJECT(priv->tree_view), "removed", G_CALLBACK(macro_removed_cb), macros_frame);
	g_signal_connect(G_OBJECT(priv->tree_view), "selected", G_CALLBACK(macro_selected_cb), macros_frame);
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(add_macroset_clicked_cb), macros_frame);

	gtk_widget_show_all(GTK_WIDGET(vbox));
}

static void gaminggear_macro_editor_macros_frame_finalize(GObject *object) {
	GaminggearMacroEditorMacrosFramePrivate *priv = GAMINGGEAR_MACRO_EDITOR_MACROS_FRAME(object)->priv;
	g_object_unref(priv->tree_view);
	G_OBJECT_CLASS(gaminggear_macro_editor_macros_frame_parent_class)->finalize(object);
}

static void gaminggear_macro_editor_macros_frame_class_init(GaminggearMacroEditorMacrosFrameClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->finalize = gaminggear_macro_editor_macros_frame_finalize;

	g_type_class_add_private(klass, sizeof(GaminggearMacroEditorMacrosFramePrivate));

	signals[SELECTED] = g_signal_new("selected",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			G_STRUCT_OFFSET(GaminggearMacroEditorMacrosFrameClass, selected),
			NULL, NULL, g_cclosure_marshal_VOID__OBJECT, G_TYPE_NONE,
			1, GTK_TYPE_TREE_ROW_REFERENCE);


	signals[REMOVED] = g_signal_new("removed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			G_STRUCT_OFFSET(GaminggearMacroEditorMacrosFrameClass, removed),
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}

GaminggearMacros *gaminggear_macro_editor_macros_frame_get_macros(GaminggearMacroEditorMacrosFrame *macros_frame) {
	GaminggearMacroTreeStore *store;

	store = gaminggear_macro_tree_view_get_store(macros_frame->priv->tree_view);
	return gaminggear_macro_tree_store_get_macros(store);
}

gboolean gaminggear_macro_editor_macros_frame_add_macros(GaminggearMacroEditorMacrosFrame *macros_frame, GaminggearMacros *gaminggear_macros) {
	GaminggearMacroTreeStore *store;
	store = gaminggear_macro_tree_view_get_store(macros_frame->priv->tree_view);
	return gaminggear_macro_tree_store_add_macros(store, gaminggear_macros);
}

void gaminggear_macro_editor_macros_frame_clear(GaminggearMacroEditorMacrosFrame *macros_frame) {
	GaminggearMacroTreeStore *store;
	store = gaminggear_macro_tree_view_get_store(macros_frame->priv->tree_view);
	gaminggear_macro_tree_store_clear(store);
}
