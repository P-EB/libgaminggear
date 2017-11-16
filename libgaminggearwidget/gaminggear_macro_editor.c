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

#include "config.h"
#include "gaminggear_macro_editor.h"
#include "gaminggear_macro_editor_macros_frame.h"
#include "gaminggear_macro_editor_macro_options_frame.h"
#include "gaminggear_macro_editor_record_options_frame.h"
#include "gaminggear_macro_editor_key_sequence_frame.h"
#include "gaminggear_macro_tree_store.h"
#include "gaminggear/key_translations.h"
#include "gaminggear_helper.h"
#include "gaminggearwidget_helper.h"
#include "gaminggear/gdk_key_translations.h"
#include "gaminggear/hid_uid.h"
#include "i18n-lib.h"
#include <gdk/gdkkeysyms.h>
#include <time.h>

#define GAMINGGEAR_MACRO_EDITOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GAMINGGEAR_MACRO_EDITOR_TYPE, GaminggearMacroEditorClass))
#define IS_GAMINGGEAR_MACRO_EDITOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GAMINGGEAR_MACRO_EDITOR_TYPE))
#define GAMINGGEAR_MACRO_EDITOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), GAMINGGEAR_MACRO_EDITOR_TYPE, GaminggearMacroEditorPrivate))

typedef struct _GaminggearMacroEditorClass GaminggearMacroEditorClass;

enum {
	NUM_VIEWS = 2
};

struct _GaminggearMacroEditorClass {
	GtkHPanedClass parent_class;
};

struct _GaminggearMacroEditorPrivate {
	GaminggearMacroEditorMacrosFrame *macros_frame;
	GaminggearMacroEditorRecordOptionsFrame *record_options_frame;
	GaminggearMacroEditorMacroOptionsFrame *macro_options_frame;
	GaminggearMacroEditorKeySequenceFrame *key_sequence_frame;
	GtkButton *save_button;
	GtkTreeRowReference *reference;
	struct timespec record_last_event_time;
	gboolean edited_modified;
	gboolean modified;
};

G_DEFINE_TYPE(GaminggearMacroEditor, gaminggear_macro_editor, GTK_TYPE_HPANED);

/*
 * left - right
 * Returns msec
 */
static glong time_diff(struct timespec const *left, struct timespec const *right) {
	return ((glong)left->tv_sec - (glong)right->tv_sec) * G_GAMINGGEAR_MSEC_PER_SEC + ((glong)left->tv_nsec - (glong)right->tv_nsec) / G_GAMINGGEAR_NSEC_PER_MSEC;
}

void gaminggear_macro_editor_set_modified(GaminggearMacroEditor *macro_editor, gboolean state) {
	macro_editor->priv->modified = state;
}

gboolean gaminggear_macro_editor_get_modified(GaminggearMacroEditor *macro_editor) {
	GaminggearMacroEditorPrivate *priv = macro_editor->priv;
	return priv->edited_modified || priv->modified;
}

static void buttons_set_sensitive(GaminggearMacroEditor *macro_editor) {
	GaminggearMacroEditorPrivate *priv = macro_editor->priv;
	gboolean valid = gtk_tree_row_reference_valid(priv->reference);
	gboolean modified = priv->edited_modified;
	gboolean empty = gaminggear_macro_editor_key_sequence_frame_empty(priv->key_sequence_frame);

	gtk_widget_set_sensitive(GTK_WIDGET(priv->save_button), valid && !empty && modified);
	gtk_widget_set_sensitive(GTK_WIDGET(priv->record_options_frame), valid);
	gtk_widget_set_sensitive(GTK_WIDGET(priv->macro_options_frame), valid);
	gtk_widget_set_sensitive(GTK_WIDGET(priv->key_sequence_frame), valid);
}

static void unset(GaminggearMacroEditor *macro_editor) {
	GaminggearMacroEditorPrivate *priv = macro_editor->priv;
	gaminggear_macro_editor_key_sequence_frame_clear(priv->key_sequence_frame);

	g_clear_pointer(&priv->reference, gtk_tree_row_reference_free);

	priv->edited_modified = FALSE;
	buttons_set_sensitive(macro_editor);
}

static void unset_if(GaminggearMacroEditor *macro_editor) {
	if (!gtk_tree_row_reference_valid(macro_editor->priv->reference))
		unset(macro_editor);
}

/* moves keystrokes from key_sequence view back to macros_tree_view */
static void store(GaminggearMacroEditor *macro_editor) {
	GaminggearMacroEditorPrivate *priv = macro_editor->priv;
	GtkTreeModel *model;
	GtkTreeIter iter;
	GaminggearMacroKeystrokes *keystrokes;

	if (!gtk_tree_row_reference_valid(priv->reference))
		return;

	keystrokes = gaminggear_macro_editor_key_sequence_frame_get_keystrokes(priv->key_sequence_frame);
	keystrokes->loop = gaminggear_macro_editor_macro_options_frame_get_loop(priv->macro_options_frame);

	model = gtk_tree_row_reference_get_model(priv->reference);
	gtk_gaminggear_tree_row_reference_get_iter(priv->reference, &iter);

	gaminggear_macro_tree_store_set_keystrokes(GAMINGGEAR_MACRO_TREE_STORE(model), &iter, keystrokes);
	g_free(keystrokes);

	priv->edited_modified = FALSE;
	gaminggear_macro_editor_set_modified(macro_editor, TRUE);
	buttons_set_sensitive(macro_editor);
}

static void store_if_needed(GaminggearMacroEditor *macro_editor) {
	GaminggearMacroEditorPrivate *priv = macro_editor->priv;
	GtkWidget *dialog;

	if (!priv->edited_modified ||
			gaminggear_macro_editor_key_sequence_frame_empty(priv->key_sequence_frame) ||
			!gtk_tree_row_reference_valid(priv->reference))
		return;

	dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, _("Would you like to store actual modifications?"));
	gtk_window_set_title(GTK_WINDOW(dialog), _("Store modifications"));

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_YES)
		store(macro_editor);

	gtk_widget_destroy(dialog);
}

static void set(GaminggearMacroEditor *macro_editor, GtkTreeRowReference *reference) {
	GaminggearMacroEditorPrivate *priv = macro_editor->priv;
	GaminggearMacroKeystrokes *keystrokes;
	GtkTreeIter iter;
	GtkTreeModel *model;

	store_if_needed(macro_editor);
	unset(macro_editor);

	gtk_tree_row_reference_free(priv->reference);
	priv->reference = gtk_tree_row_reference_copy(reference);

	model = gtk_tree_row_reference_get_model(reference);
	gtk_gaminggear_tree_row_reference_get_iter(reference, &iter);

	keystrokes = gaminggear_macro_tree_store_get_keystrokes(GAMINGGEAR_MACRO_TREE_STORE(model), &iter);

	gaminggear_macro_editor_macro_options_frame_set_loop(priv->macro_options_frame, keystrokes->loop);
	gaminggear_macro_editor_key_sequence_frame_set_keystrokes(priv->key_sequence_frame, keystrokes);
	priv->edited_modified = FALSE;

	g_free(keystrokes);
	buttons_set_sensitive(macro_editor);
}

static void edited_modified_cb(GtkWidget *widget, gpointer user_data) {
	GaminggearMacroEditor *macro_editor = GAMINGGEAR_MACRO_EDITOR(user_data);
	macro_editor->priv->edited_modified = TRUE;
	buttons_set_sensitive(macro_editor);
}

static void save_button_clicked_cb(GtkButton *button, gpointer user_data) {
	GaminggearMacroEditor *macro_editor = GAMINGGEAR_MACRO_EDITOR(user_data);
	store(macro_editor);
}

static void record_button_clicked_cb(GaminggearMacroEditorRecordOptionsFrame *frame, gpointer user_data) {
	GaminggearMacroEditorPrivate *priv = GAMINGGEAR_MACRO_EDITOR(user_data)->priv;
	gboolean on = gaminggear_macro_editor_record_options_frame_is_record_on(priv->record_options_frame);

	if (on)
		clock_gettime(CLOCK_REALTIME, &priv->record_last_event_time);

	gaminggear_macro_editor_key_sequence_record_mode(priv->key_sequence_frame, on);
}

static GdkKeymapKey *get_key_with_group_0(GdkKeymapKey *keys, gint n_keys) {
	gint i;

	for (i = 0; i < n_keys; ++i)
		if (keys[i].group == 0)
			return &keys[i];

	g_warning(_("Could not find key for group 0 in keymap"));
	/* fallback: return first element */
	return keys;
}

static void paste_button_clicked_cb(GaminggearMacroEditorRecordOptionsFrame *frame, gpointer user_data) {
	GaminggearMacroEditorPrivate *priv = GAMINGGEAR_MACRO_EDITOR(user_data)->priv;
	GtkClipboard *clipboard;
	gchar *text;
	gchar const *iter;
	guint keyval;
	gboolean shift = FALSE;
	gboolean alt = FALSE;
	gunichar character;
	GdkKeymapKey *keys;
	GdkKeymapKey *key;
	gint n_keys;
	GdkKeymap *keymap;
	glong rel_time;

	clipboard = gtk_clipboard_get_for_display(gdk_display_get_default(), GDK_SELECTION_PRIMARY);
	text = gtk_clipboard_wait_for_text(clipboard);
	if (text) {
		iter = text;

		keymap = gdk_keymap_get_default();
		if (gaminggear_macro_editor_record_options_frame_delay_as_recorded(frame))
			rel_time = 0;
		else
			rel_time = gaminggear_macro_editor_record_options_frame_get_delay(frame);

		while(TRUE) {
			character = g_utf8_get_char_validated(iter, -1);
			if (character <= 0)
				break;

			iter = g_utf8_find_next_char(iter, NULL);

			keyval = gdk_unicode_to_keyval(character);
			/* Some unicode characters have to be converted manually.
			 * Also linefeed does not exist in HID usage table. Since we are on Linux,
			 * linefeed gets converted to return and return is ignored.
			 */
			if (keyval == (character | 0x01000000)) {
				if (character == 0x09) /* Horizontal tab */
					keyval = GDK_Tab;
				else if (character == 0x0a) /* Linefeed */
					keyval = GDK_Return;
				else if (character == 0x0d) /* Return */
					continue;
			}

			if (!gdk_keymap_get_entries_for_keyval(keymap, keyval, &keys, &n_keys)) {
				g_warning(_("Keyval 0x%04x has no corresponding keys in keymap"), keyval);
				break;
			}

			key = get_key_with_group_0(keys, n_keys);

			/* First release keys before pressing others */

			if (key->level != 1 && shift) {
				gaminggear_macro_editor_key_sequence_frame_add_keystroke(priv->key_sequence_frame,
						HID_UID_KB_LEFT_SHIFT, GAMINGGEAR_MACRO_KEYSTROKE_ACTION_RELEASE, rel_time);
				shift = FALSE;
			}

			if (key->level != 2 && alt) {
				gaminggear_macro_editor_key_sequence_frame_add_keystroke(priv->key_sequence_frame,
						HID_UID_KB_RIGHT_ALT, GAMINGGEAR_MACRO_KEYSTROKE_ACTION_RELEASE, rel_time);
				alt = FALSE;
			}

			if (key->level == 1 && !shift) {
				gaminggear_macro_editor_key_sequence_frame_add_keystroke(priv->key_sequence_frame,
						HID_UID_KB_LEFT_SHIFT, GAMINGGEAR_MACRO_KEYSTROKE_ACTION_PRESS, rel_time);
				shift = TRUE;
			}

			if (key->level == 2 && !alt) {
				gaminggear_macro_editor_key_sequence_frame_add_keystroke(priv->key_sequence_frame,
						HID_UID_KB_RIGHT_ALT, GAMINGGEAR_MACRO_KEYSTROKE_ACTION_PRESS, rel_time);
				alt = TRUE;
			}

			gaminggear_macro_editor_key_sequence_frame_add_keystroke(priv->key_sequence_frame,
					gaminggear_xkeycode_to_hid(key->keycode), GAMINGGEAR_MACRO_KEYSTROKE_ACTION_PRESS, rel_time);
			gaminggear_macro_editor_key_sequence_frame_add_keystroke(priv->key_sequence_frame,
					gaminggear_xkeycode_to_hid(key->keycode), GAMINGGEAR_MACRO_KEYSTROKE_ACTION_RELEASE, rel_time);

			g_free(keys);
		}

		if (shift) {
			gaminggear_macro_editor_key_sequence_frame_add_keystroke(priv->key_sequence_frame,
					HID_UID_KB_LEFT_SHIFT, GAMINGGEAR_MACRO_KEYSTROKE_ACTION_RELEASE, rel_time);
		}

		if (alt) {
			gaminggear_macro_editor_key_sequence_frame_add_keystroke(priv->key_sequence_frame,
					HID_UID_KB_RIGHT_ALT, GAMINGGEAR_MACRO_KEYSTROKE_ACTION_RELEASE, rel_time);
		}

		g_free(text);
	}
}

static void macro_selected_cb(GaminggearMacroEditorMacrosFrame *frame, GtkTreeRowReference *reference, gpointer user_data) {
	GaminggearMacroEditor *macro_editor = GAMINGGEAR_MACRO_EDITOR(user_data);
	set(macro_editor, reference);
}

static void macro_removed_cb(GaminggearMacroEditorMacrosFrame *frame, gpointer user_data) {
	GaminggearMacroEditor *macro_editor = GAMINGGEAR_MACRO_EDITOR(user_data);
	gaminggear_macro_editor_set_modified(macro_editor, TRUE);
	unset_if(macro_editor);
}

/* To distinguish between a pressed and released key and autorepeat GDK sends
 * press/press/press/release on autorepeat.
 * Has to see every event in order.
 * It's not reentrant, but doesn't matter here.
 */
static gboolean is_autorepeat_event(GdkEventKey *event) {
	static guint16 last_keycode;
	static guint last_type = GDK_NOTHING;

	if (event->type == GDK_KEY_PRESS && last_type == GDK_KEY_PRESS && event->hardware_keycode == last_keycode)
		return TRUE;

	last_type = event->type;
	last_keycode = event->hardware_keycode;

	return FALSE;
}

static gboolean key_event_cb(GtkWindow *window, GdkEventKey *event, gpointer user_data) {
	GaminggearMacroEditor *macro_editor = GAMINGGEAR_MACRO_EDITOR(user_data);
	GaminggearMacroEditorPrivate *priv = macro_editor->priv;
	glong rel_time;
	struct timespec actual_time;
	guint action;

	if (!gaminggear_macro_editor_record_options_frame_is_record_on(priv->record_options_frame))
		return FALSE; /* propagate */

	if (is_autorepeat_event(event))
		return TRUE;

	/* always keep record_last_event_time up to date, so user can switch
	 * record options while recording
	 */
	clock_gettime(CLOCK_REALTIME, &actual_time);
	if (gaminggear_macro_editor_record_options_frame_delay_as_recorded(priv->record_options_frame))
		rel_time = time_diff(&actual_time, &priv->record_last_event_time);
	else
		rel_time = gaminggear_macro_editor_record_options_frame_get_delay(priv->record_options_frame);
	priv->record_last_event_time = actual_time;

	action = (event->type == GDK_KEY_PRESS) ? GAMINGGEAR_MACRO_KEYSTROKE_ACTION_PRESS : GAMINGGEAR_MACRO_KEYSTROKE_ACTION_RELEASE;

	gaminggear_macro_editor_key_sequence_frame_add_keystroke(priv->key_sequence_frame,
			gaminggear_xkeycode_to_hid(event->hardware_keycode), action, rel_time);

	return TRUE;
}

static gboolean button_event_cb(GaminggearMacroEditorKeySequenceFrame *key_sequence_frame, guint action, guint button, gpointer user_data) {
	GaminggearMacroEditor *macro_editor = GAMINGGEAR_MACRO_EDITOR(user_data);
	GaminggearMacroEditorPrivate *priv = macro_editor->priv;
	glong rel_time;
	struct timespec actual_time;

	if (!gaminggear_macro_editor_record_options_frame_is_record_on(priv->record_options_frame))
		return FALSE; /* propagate */

	if (action != GDK_BUTTON_PRESS && action != GDK_BUTTON_RELEASE)
		return TRUE;

	/* always keep record_last_event_time up to date, so user can switch
	 * record options while recording
	 */
	clock_gettime(CLOCK_REALTIME, &actual_time);
	if (gaminggear_macro_editor_record_options_frame_delay_as_recorded(priv->record_options_frame))
		rel_time = time_diff(&actual_time, &priv->record_last_event_time);
	else
		rel_time = gaminggear_macro_editor_record_options_frame_get_delay(priv->record_options_frame);
	priv->record_last_event_time = actual_time;

	gaminggear_macro_editor_key_sequence_frame_add_keystroke(priv->key_sequence_frame,
			button - 1 + GAMINGGEAR_MACRO_KEYSTROKE_KEY_BUTTON_LEFT,
			(action == GDK_BUTTON_PRESS) ? GAMINGGEAR_MACRO_KEYSTROKE_ACTION_PRESS : GAMINGGEAR_MACRO_KEYSTROKE_ACTION_RELEASE,
			rel_time);

	return TRUE;
}

GtkWidget *gaminggear_macro_editor_new(void) {
	GaminggearMacroEditor *macro_editor;

	macro_editor = g_object_new(GAMINGGEAR_MACRO_EDITOR_TYPE, NULL);

	buttons_set_sensitive(macro_editor);

	return GTK_WIDGET(macro_editor);
}

static void gaminggear_macro_editor_init(GaminggearMacroEditor *macro_editor) {
	GaminggearMacroEditorPrivate *priv = GAMINGGEAR_MACRO_EDITOR_GET_PRIVATE(macro_editor);
	GtkWidget *hbox;
	GtkWidget *vbox;
	GtkBox *action_area;

	macro_editor->priv = priv;

	priv->reference = NULL;
	priv->modified = FALSE;
	priv->edited_modified = FALSE;

	vbox = gtk_vbox_new(FALSE, 0);
	hbox = gtk_hbox_new(FALSE, 0);

	priv->save_button = GTK_BUTTON(gtk_button_new_with_label(_("Save")));
	g_object_ref_sink(priv->save_button);
	priv->macros_frame = GAMINGGEAR_MACRO_EDITOR_MACROS_FRAME(gaminggear_macro_editor_macros_frame_new());
	g_object_ref_sink(priv->macros_frame);
	priv->macro_options_frame = GAMINGGEAR_MACRO_EDITOR_MACRO_OPTIONS_FRAME(gaminggear_macro_editor_macro_options_frame_new());
	g_object_ref_sink(priv->macro_options_frame);
	priv->record_options_frame = GAMINGGEAR_MACRO_EDITOR_RECORD_OPTIONS_FRAME(gaminggear_macro_editor_record_options_frame_new());
	g_object_ref_sink(priv->record_options_frame);
	priv->key_sequence_frame = GAMINGGEAR_MACRO_EDITOR_KEY_SEQUENCE_FRAME(gaminggear_macro_editor_key_sequence_frame_new());
	g_object_ref_sink(priv->key_sequence_frame);

	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->record_options_frame), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(priv->macro_options_frame), TRUE, TRUE, 0);

	gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(priv->key_sequence_frame), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), vbox, FALSE, FALSE, 0);

	gtk_paned_pack1(GTK_PANED(macro_editor), GTK_WIDGET(priv->macros_frame), FALSE, FALSE);
	gtk_paned_pack2(GTK_PANED(macro_editor), hbox, TRUE, FALSE);

	action_area = GTK_BOX(gaminggear_macro_editor_key_sequence_get_action_area(priv->key_sequence_frame));
	gtk_box_pack_start(action_area, GTK_WIDGET(priv->save_button), FALSE, FALSE, 0);
	gtk_box_reorder_child(action_area, GTK_WIDGET(priv->save_button), 0);

	g_signal_connect(G_OBJECT(priv->key_sequence_frame), "modified", G_CALLBACK(edited_modified_cb), macro_editor);
	g_signal_connect(G_OBJECT(priv->key_sequence_frame), "button-event", G_CALLBACK(button_event_cb), macro_editor);
	g_signal_connect(G_OBJECT(priv->macro_options_frame), "modified", G_CALLBACK(edited_modified_cb), macro_editor);
	g_signal_connect(G_OBJECT(priv->record_options_frame), "record-event", G_CALLBACK(record_button_clicked_cb), macro_editor);
	g_signal_connect(G_OBJECT(priv->record_options_frame), "paste", G_CALLBACK(paste_button_clicked_cb), macro_editor);
	g_signal_connect(G_OBJECT(macro_editor), "key-press-event", G_CALLBACK(key_event_cb), macro_editor);
	g_signal_connect(G_OBJECT(macro_editor), "key-release-event", G_CALLBACK(key_event_cb), macro_editor);
	g_signal_connect(G_OBJECT(priv->macros_frame), "removed", G_CALLBACK(macro_removed_cb), macro_editor);
	g_signal_connect(G_OBJECT(priv->macros_frame), "selected", G_CALLBACK(macro_selected_cb), macro_editor);
	g_signal_connect(G_OBJECT(priv->save_button), "clicked", G_CALLBACK(save_button_clicked_cb), macro_editor);

	gtk_widget_show(GTK_WIDGET(priv->macros_frame));
	gtk_widget_show(GTK_WIDGET(priv->macro_options_frame));
	gtk_widget_show(GTK_WIDGET(priv->record_options_frame));
	gtk_widget_show(GTK_WIDGET(priv->key_sequence_frame));
	gtk_widget_show(GTK_WIDGET(priv->save_button));
	gtk_widget_show(hbox);
	gtk_widget_show(vbox);
}

static void gaminggear_macro_editor_finalize(GObject *object) {
	GaminggearMacroEditorPrivate *priv = GAMINGGEAR_MACRO_EDITOR(object)->priv;
	g_object_unref(priv->macros_frame);
	g_object_unref(priv->macro_options_frame);
	g_object_unref(priv->record_options_frame);
	g_object_unref(priv->key_sequence_frame);
	g_object_unref(priv->save_button);
	G_OBJECT_CLASS(gaminggear_macro_editor_parent_class)->finalize(object);
}

static void gaminggear_macro_editor_class_init(GaminggearMacroEditorClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = (GObjectClass*)klass;

	gobject_class->finalize = gaminggear_macro_editor_finalize;

	g_type_class_add_private(klass, sizeof(GaminggearMacroEditorPrivate));
}

void gaminggear_macro_editor_clear(GaminggearMacroEditor *macro_editor) {
	gaminggear_macro_editor_macros_frame_clear(macro_editor->priv->macros_frame);
}

void gaminggear_macro_editor_add_macros(GaminggearMacroEditor *macro_editor, GaminggearMacros *macros) {
	if (gaminggear_macro_editor_macros_frame_add_macros(macro_editor->priv->macros_frame, macros))
		gaminggear_macro_editor_set_modified(macro_editor, TRUE);
}

GaminggearMacros *gaminggear_macro_editor_get_macros(GaminggearMacroEditor *macro_editor) {
	store_if_needed(macro_editor);
	return gaminggear_macro_editor_macros_frame_get_macros(macro_editor->priv->macros_frame);
}
