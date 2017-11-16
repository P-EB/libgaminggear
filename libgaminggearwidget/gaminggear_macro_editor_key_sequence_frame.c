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

#include "gaminggear_macro_editor_key_sequence_frame.h"
#include "gaminggear_macro_editor_advanced.h"
#include "gaminggear_macro_editor_basic.h"
#include "g_cclosure_gaminggear_marshaller.h"
#include "i18n-lib.h"

#define GAMINGGEAR_MACRO_EDITOR_KEY_SEQUENCE_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GAMINGGEAR_MACRO_EDITOR_KEY_SEQUENCE_FRAME_TYPE, GaminggearMacroEditorKeySequenceFrameClass))
#define IS_GAMINGGEAR_MACRO_EDITOR_KEY_SEQUENCE_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GAMINGGEAR_MACRO_EDITOR_KEY_SEQUENCE_FRAME_TYPE))
#define GAMINGGEAR_MACRO_EDITOR_KEY_SEQUENCE_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), GAMINGGEAR_MACRO_EDITOR_KEY_SEQUENCE_FRAME_TYPE, GaminggearMacroEditorKeySequenceFramePrivate))

typedef struct _GaminggearMacroEditorKeySequenceFrameClass GaminggearMacroEditorKeySequenceFrameClass;
typedef struct _AbsoluteKeystroke AbsoluteKeystroke;

enum {
	NUM_VIEWS = 2
};

struct _GaminggearMacroEditorKeySequenceFrameClass {
	GtkFrameClass parent_class;
};

struct _GaminggearMacroEditorKeySequenceFramePrivate {
	GtkButton *clear_button;
	GtkComboBox *view_combo;
	GtkWidget *action_area;
	GtkEventBox *event_box;
	GaminggearMacroEditorInterface *active_view;
	GaminggearMacroEditorInterface *views[NUM_VIEWS];
	gulong handlers[NUM_VIEWS];
	gboolean record;
};

G_DEFINE_TYPE(GaminggearMacroEditorKeySequenceFrame, gaminggear_macro_editor_key_sequence_frame, GTK_TYPE_FRAME);

enum {
	MODIFIED,
	BUTTON_EVENT,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

enum {
	VIEW_TITLE_COLUMN,
	VIEW_INDEX_COLUMN,
	VIEW_N_COLUMNS
};

static void set_clear_button_sensitive(GaminggearMacroEditorKeySequenceFrame *key_sequence_frame) {
	GaminggearMacroEditorKeySequenceFramePrivate *priv = key_sequence_frame->priv;
	gboolean empty = gaminggear_macro_editor_interface_empty(priv->active_view);
	gtk_widget_set_sensitive(GTK_WIDGET(priv->clear_button), !empty);
}

static void modified(GaminggearMacroEditorKeySequenceFrame *key_sequence_frame) {
	g_signal_emit((gpointer)key_sequence_frame, signals[MODIFIED], 0);
}

static void modified_cb(GaminggearMacroEditorInterface *view, gpointer user_data) {
	GaminggearMacroEditorKeySequenceFrame *key_sequence_frame = GAMINGGEAR_MACRO_EDITOR_KEY_SEQUENCE_FRAME(user_data);
	modified(key_sequence_frame);
}

static void empty_changed_cb(GaminggearMacroEditorInterface *view, gpointer user_data) {
	GaminggearMacroEditorKeySequenceFrame *key_sequence_frame = GAMINGGEAR_MACRO_EDITOR_KEY_SEQUENCE_FRAME(user_data);
	set_clear_button_sensitive(key_sequence_frame);
}

static void clear_button_clicked_cb(GtkButton *button, gpointer user_data) {
	GaminggearMacroEditorKeySequenceFrame *key_sequence_frame = GAMINGGEAR_MACRO_EDITOR_KEY_SEQUENCE_FRAME(user_data);
	GaminggearMacroEditorKeySequenceFramePrivate *priv = key_sequence_frame->priv;
	gaminggear_macro_editor_interface_clear(priv->active_view);
}

static GaminggearMacroKeystrokes *deactivate_view(GaminggearMacroEditorInterface *view) {
	gtk_widget_hide(GTK_WIDGET(view));
	return gaminggear_macro_editor_interface_get_keystrokes(view);
}

static void activate_view(GaminggearMacroEditorKeySequenceFrame *key_sequence_frame, GaminggearMacroKeystrokes *keystrokes) {
	GaminggearMacroEditorKeySequenceFramePrivate *priv = key_sequence_frame->priv;
	GtkTreeModel *model;
	GtkTreeIter iter;
	guint index;

	if (!gtk_combo_box_get_active_iter(priv->view_combo, &iter)) {
		/* activate first iter if non active */
		model = gtk_combo_box_get_model(priv->view_combo);
		if (!gtk_tree_model_get_iter_first(model, &iter)) {
			priv->active_view = NULL;
			return;
		}
		gtk_combo_box_set_active_iter(priv->view_combo, &iter);
	}

	gtk_tree_model_get(gtk_combo_box_get_model(priv->view_combo), &iter, VIEW_INDEX_COLUMN, &index, -1);
	priv->active_view = priv->views[index];
	if (keystrokes)
		gaminggear_macro_editor_interface_set_keystrokes(priv->active_view, keystrokes);
	else
		gaminggear_macro_editor_interface_clear(priv->active_view);
	gtk_widget_show(GTK_WIDGET(priv->active_view));

	/* If user activates another view while recording, EventBox is not on top anymore.
	 * Setting again here is not enough, above_child needs to be toggled to work.
	 */
	gtk_event_box_set_above_child(key_sequence_frame->priv->event_box, !priv->record);
	gtk_event_box_set_above_child(key_sequence_frame->priv->event_box, priv->record);
}

static void block_handlers(GaminggearMacroEditorKeySequenceFrame *key_sequence_frame) {
	GaminggearMacroEditorKeySequenceFramePrivate *priv = key_sequence_frame->priv;
	guint i;
	for (i = 0; i < NUM_VIEWS; ++i)
		g_signal_handler_block(priv->views[i], priv->handlers[i]);
}

static void unblock_handlers(GaminggearMacroEditorKeySequenceFrame *key_sequence_frame) {
	GaminggearMacroEditorKeySequenceFramePrivate *priv = key_sequence_frame->priv;
	guint i;
	for (i = 0; i < NUM_VIEWS; ++i)
		g_signal_handler_unblock(priv->views[i], priv->handlers[i]);
}

static void combo_button_changed_cb(GtkComboBox *combo, gpointer user_data) {
	GaminggearMacroEditorKeySequenceFrame *key_sequence_frame = GAMINGGEAR_MACRO_EDITOR_KEY_SEQUENCE_FRAME(user_data);
	GaminggearMacroEditorKeySequenceFramePrivate *priv = key_sequence_frame->priv;
	GaminggearMacroKeystrokes *keystrokes;

	block_handlers(key_sequence_frame);

	if (priv->active_view)
		keystrokes = deactivate_view(priv->active_view);
	else
		keystrokes = NULL;

	activate_view(key_sequence_frame, keystrokes);

	unblock_handlers(key_sequence_frame);
}

static GtkWidget *view_combo_new(GaminggearMacroEditorKeySequenceFrame *key_sequence_frame) {
	GaminggearMacroEditorKeySequenceFramePrivate *priv = key_sequence_frame->priv;
	GtkWidget *combo;
	GtkListStore *store;
	GtkTreeIter iter;
	GtkCellRenderer *renderer;
	guint i;

	store = gtk_list_store_new(VIEW_N_COLUMNS, G_TYPE_STRING, G_TYPE_INT);

	for (i = 0; i < NUM_VIEWS; ++i) {
		gtk_list_store_append(store, &iter);
		gtk_list_store_set(store, &iter,
				VIEW_TITLE_COLUMN, gaminggear_macro_editor_interface_name(priv->views[i]),
				VIEW_INDEX_COLUMN, i,
				-1);
	}

	combo = gtk_combo_box_new_with_model(GTK_TREE_MODEL(store));
	g_object_unref(store);

	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo), renderer, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo), renderer, "text", VIEW_TITLE_COLUMN, NULL);

	return combo;
}

GtkWidget *gaminggear_macro_editor_key_sequence_frame_new(void) {
	GaminggearMacroEditorKeySequenceFrame *key_sequence_frame;

	key_sequence_frame = g_object_new(GAMINGGEAR_MACRO_EDITOR_KEY_SEQUENCE_FRAME_TYPE,
			"label", _("Key sequence"),
			NULL);

	set_clear_button_sensitive(key_sequence_frame);

	return GTK_WIDGET(key_sequence_frame);
}

static gboolean button_event_cb(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
	GaminggearMacroEditorKeySequenceFrame *key_sequence_frame = GAMINGGEAR_MACRO_EDITOR_KEY_SEQUENCE_FRAME(user_data);
	gboolean retval;
	g_signal_emit((gpointer)key_sequence_frame, signals[BUTTON_EVENT], 0, event->type, event->button, &retval);
	return retval;
}

static void gaminggear_macro_editor_key_sequence_frame_init(GaminggearMacroEditorKeySequenceFrame *key_sequence_frame) {
	GaminggearMacroEditorKeySequenceFramePrivate *priv = GAMINGGEAR_MACRO_EDITOR_KEY_SEQUENCE_FRAME_GET_PRIVATE(key_sequence_frame);
	GtkWidget *vbox;
	GtkWidget *hbox;
	guint i;

	key_sequence_frame->priv = priv;

	vbox = gtk_vbox_new(FALSE, 0);
	priv->action_area = gtk_hbox_new(FALSE, 0);

	priv->event_box = GTK_EVENT_BOX(gtk_event_box_new());
	gtk_event_box_set_visible_window(priv->event_box, TRUE);
	gtk_widget_add_events(GTK_WIDGET(priv->event_box), GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK);

	hbox = gtk_hbox_new(FALSE, 0);

	priv->clear_button = GTK_BUTTON(gtk_button_new_with_label(_("Clear")));
	g_object_ref_sink(priv->clear_button);

	priv->views[0] = GAMINGGEAR_MACRO_EDITOR_INTERFACE(gaminggear_macro_editor_advanced_new());
	g_object_ref_sink(priv->views[0]);

	priv->views[1] = GAMINGGEAR_MACRO_EDITOR_INTERFACE(gaminggear_macro_editor_basic_new());
	g_object_ref_sink(priv->views[1]);

	priv->view_combo = GTK_COMBO_BOX(view_combo_new(key_sequence_frame));
	g_object_ref_sink(priv->view_combo);

	for (i = 0; i < NUM_VIEWS; ++i)
		gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(priv->views[i]), TRUE, TRUE, 0);

	gtk_container_add(GTK_CONTAINER(priv->event_box), hbox);

	gtk_box_pack_start(GTK_BOX(priv->action_area), GTK_WIDGET(priv->clear_button), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(priv->action_area), GTK_WIDGET(priv->view_combo), FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->event_box), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), priv->action_area, FALSE, FALSE, 0);

	gtk_container_add(GTK_CONTAINER(key_sequence_frame), vbox);

	for (i = 0; i < NUM_VIEWS; ++i)
		gtk_widget_hide(GTK_WIDGET(priv->views[i]));

	gtk_widget_show_all(priv->action_area);
	gtk_widget_show(vbox);
	gtk_widget_show(GTK_WIDGET(priv->event_box));
	gtk_widget_show(hbox);
	activate_view(key_sequence_frame, NULL);

	for (i = 0; i < NUM_VIEWS; ++i) {
		priv->handlers[i] = g_signal_connect(G_OBJECT(priv->views[i]), "modified", G_CALLBACK(modified_cb), key_sequence_frame);
		g_signal_connect(G_OBJECT(priv->views[i]), "empty-changed", G_CALLBACK(empty_changed_cb), key_sequence_frame);
	}
	g_signal_connect(G_OBJECT(priv->clear_button), "clicked", G_CALLBACK(clear_button_clicked_cb), key_sequence_frame);
	g_signal_connect(G_OBJECT(priv->view_combo), "changed", G_CALLBACK(combo_button_changed_cb), key_sequence_frame);
	g_signal_connect(G_OBJECT(priv->event_box), "button-press-event", G_CALLBACK(button_event_cb), key_sequence_frame);
	g_signal_connect(G_OBJECT(priv->event_box), "button-release-event", G_CALLBACK(button_event_cb), key_sequence_frame);
}

static void gaminggear_macro_editor_key_sequence_frame_finalize(GObject *object) {
	GaminggearMacroEditorKeySequenceFramePrivate *priv = GAMINGGEAR_MACRO_EDITOR_KEY_SEQUENCE_FRAME(object)->priv;
	guint i;

	g_object_unref(priv->clear_button);
	g_object_unref(priv->view_combo);

	for (i = 0; i < NUM_VIEWS; ++i)
		g_object_unref(priv->views[i]);

	G_OBJECT_CLASS(gaminggear_macro_editor_key_sequence_frame_parent_class)->finalize(object);
}

static void gaminggear_macro_editor_key_sequence_frame_class_init(GaminggearMacroEditorKeySequenceFrameClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->finalize = gaminggear_macro_editor_key_sequence_frame_finalize;

	g_type_class_add_private(klass, sizeof(GaminggearMacroEditorKeySequenceFramePrivate));

	signals[MODIFIED] = g_signal_new("modified",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

	signals[BUTTON_EVENT] = g_signal_new("button-event",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_gaminggear_marshal_BOOLEAN__UINT_UINT, G_TYPE_BOOLEAN,
			2, G_TYPE_UINT, G_TYPE_UINT);
}

void gaminggear_macro_editor_key_sequence_frame_set_keystrokes(GaminggearMacroEditorKeySequenceFrame *key_sequence_frame, GaminggearMacroKeystrokes *keystrokes) {
	block_handlers(key_sequence_frame);
	gaminggear_macro_editor_interface_set_keystrokes(key_sequence_frame->priv->active_view, keystrokes);
	unblock_handlers(key_sequence_frame);
}

GaminggearMacroKeystrokes *gaminggear_macro_editor_key_sequence_frame_get_keystrokes(GaminggearMacroEditorKeySequenceFrame *key_sequence_frame) {
	return gaminggear_macro_editor_interface_get_keystrokes(key_sequence_frame->priv->active_view);
}

void gaminggear_macro_editor_key_sequence_frame_add_keystroke(GaminggearMacroEditorKeySequenceFrame *key_sequence_frame, guint key, guint action, glong rel_time) {
	gaminggear_macro_editor_interface_add_keystroke(key_sequence_frame->priv->active_view, key, action, rel_time);
}

void gaminggear_macro_editor_key_sequence_frame_clear(GaminggearMacroEditorKeySequenceFrame *key_sequence_frame) {
	guint i;

	for (i = 0; i < NUM_VIEWS; ++i)
		gaminggear_macro_editor_interface_clear(key_sequence_frame->priv->views[i]);
}

gboolean gaminggear_macro_editor_key_sequence_frame_empty(GaminggearMacroEditorKeySequenceFrame *key_sequence_frame) {
	return gaminggear_macro_editor_interface_empty(key_sequence_frame->priv->active_view);
}

GtkWidget *gaminggear_macro_editor_key_sequence_get_action_area(GaminggearMacroEditorKeySequenceFrame *key_sequence_frame) {
	return key_sequence_frame->priv->action_area;
}

void gaminggear_macro_editor_key_sequence_record_mode(GaminggearMacroEditorKeySequenceFrame *key_sequence_frame, gboolean on) {
	GaminggearMacroEditorKeySequenceFramePrivate *priv = key_sequence_frame->priv;
	priv->record = on;
	gtk_event_box_set_above_child(key_sequence_frame->priv->event_box, on);
}
