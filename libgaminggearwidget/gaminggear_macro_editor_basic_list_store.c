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

#include "gaminggear_macro_editor_basic_list_store.h"
#include "gaminggear/gdk_key_translations.h"
#include "gaminggearwidget_helper.h"
#include "i18n-lib.h"
#include <stdlib.h>

#define GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_TYPE, GaminggearMacroEditorBasicListStoreClass))
#define IS_GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_TYPE))
#define GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_TYPE, GaminggearMacroEditorBasicListStorePrivate))

typedef struct _GaminggearMacroEditorBasicListStoreClass GaminggearMacroEditorBasicListStoreClass;

struct _GaminggearMacroEditorBasicListStoreClass {
	GtkListStoreClass parent_class;
	void (*modified)(GaminggearMacroEditorBasicListStore *gaminggear_macro_editor_basic_list_store);
	void (*empty_changed)(GaminggearMacroEditorBasicListStore *gaminggear_macro_editor_basic_list_store);
};

struct _GaminggearMacroEditorBasicListStorePrivate {
	gboolean modified;
};

G_DEFINE_TYPE(GaminggearMacroEditorBasicListStore, gaminggear_macro_editor_basic_list_store, GTK_TYPE_LIST_STORE);

enum {
	MODIFIED,
	EMPTY_CHANGED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

enum {
	GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_ICON_COLUMN,
	GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_TEXT_COLUMN,
	GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_IS_KEY_COLUMN,
	GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_IS_NOT_KEY_COLUMN,
	GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_KEY_COLUMN,
	GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_ACTION_COLUMN,
	GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_PERIOD_COLUMN,
	GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_N_COLUMNS
};

static void modified(GaminggearMacroEditorBasicListStore *macro_editor_basic_list_store) {
	g_signal_emit((gpointer)macro_editor_basic_list_store, signals[MODIFIED], 0);
}

static void empty_changed(GaminggearMacroEditorBasicListStore *macro_editor_basic_list_store) {
	g_signal_emit((gpointer)macro_editor_basic_list_store, signals[EMPTY_CHANGED], 0);
}

void gaminggear_macro_editor_basic_list_store_set_modified(GaminggearMacroEditorBasicListStore *macro_editor_basic_list_store, gboolean state) {
	macro_editor_basic_list_store->priv->modified = state;
	if (state)
		modified(macro_editor_basic_list_store);
}

gboolean gaminggear_macro_editor_basic_list_store_get_modified(GaminggearMacroEditorBasicListStore *macro_editor_basic_list_store) {
	return macro_editor_basic_list_store->priv->modified;
}

GaminggearMacroEditorBasicListStore *gaminggear_macro_editor_basic_list_store_new(void) {
	GaminggearMacroEditorBasicListStore *macro_editor_basic_list_store;
	GType types[GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_N_COLUMNS] = {
		G_TYPE_STRING,
		G_TYPE_STRING,
		G_TYPE_BOOLEAN,
		G_TYPE_BOOLEAN,
		G_TYPE_UINT,
		G_TYPE_UINT,
		G_TYPE_UINT
	};

	macro_editor_basic_list_store = g_object_new(GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_TYPE,
			NULL);

	gtk_list_store_set_column_types(GTK_LIST_STORE(macro_editor_basic_list_store),
			GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_N_COLUMNS, types);

	return macro_editor_basic_list_store;
}

static void gaminggear_macro_editor_basic_list_store_init(GaminggearMacroEditorBasicListStore *macro_editor_basic_list_store) {
	GaminggearMacroEditorBasicListStorePrivate *priv = GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_GET_PRIVATE(macro_editor_basic_list_store);

	macro_editor_basic_list_store->priv = priv;

	priv->modified = FALSE;
}

static void gaminggear_macro_editor_basic_list_store_class_init(GaminggearMacroEditorBasicListStoreClass *klass) {
	g_type_class_add_private(klass, sizeof(GaminggearMacroEditorBasicListStorePrivate));

	signals[MODIFIED] = g_signal_new("modified",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			G_STRUCT_OFFSET(GaminggearMacroEditorBasicListStoreClass, modified),
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

	signals[EMPTY_CHANGED] = g_signal_new("empty-changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			G_STRUCT_OFFSET(GaminggearMacroEditorBasicListStoreClass, empty_changed),
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}

static void insert_single_action_key_before(GaminggearMacroEditorBasicListStore *macro_editor_basic_list_store, GtkTreeIter *sibling, guint key, guint action) {
	GtkListStore *store;
	GtkTreeIter iter;
	gchar *title;
	gchar const *icon;
	gboolean was_empty;

	was_empty = gaminggear_macro_editor_basic_list_store_empty(macro_editor_basic_list_store);
	store = GTK_LIST_STORE(macro_editor_basic_list_store);
	title = gaminggear_hid_to_keyname(key);
	icon = (action == GAMINGGEAR_MACRO_KEYSTROKE_ACTION_PRESS) ? "gaminggear-press" : "gaminggear-release";

	gtk_list_store_insert_before(store, &iter, sibling);
	gtk_list_store_set(store, &iter,
			GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_ICON_COLUMN, icon,
			GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_TEXT_COLUMN, title,
			GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_IS_KEY_COLUMN, TRUE,
			GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_IS_NOT_KEY_COLUMN, FALSE,
			GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_KEY_COLUMN, key,
			GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_ACTION_COLUMN, action,
			-1);
	g_free(title);

	gaminggear_macro_editor_basic_list_store_set_modified(macro_editor_basic_list_store, TRUE);

	if (was_empty)
		empty_changed(macro_editor_basic_list_store);
}

void gaminggear_macro_editor_basic_list_store_insert_single_action_key_before(GaminggearMacroEditorBasicListStore *macro_editor_basic_list_store, GtkTreeIter *sibling, guint key, guint action) {
	insert_single_action_key_before(macro_editor_basic_list_store, sibling, key, action);
}

static gboolean current_is_period(GaminggearMacroEditorBasicListStore *macro_editor_basic_list_store, GtkTreeIter *iter) {
	gboolean is_not_key;

	if (!iter)
		return FALSE;

	gtk_tree_model_get(GTK_TREE_MODEL(macro_editor_basic_list_store), iter, GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_IS_NOT_KEY_COLUMN, &is_not_key, -1);
	return is_not_key;
}

static gboolean previous_is_period(GaminggearMacroEditorBasicListStore *macro_editor_basic_list_store, GtkTreeIter *iter) {
	gboolean valid;
	GtkTreeIter test_iter;

	if (iter) {
		test_iter = *iter;
		valid = gtk_gaminggear_tree_model_get_iter_previous(GTK_TREE_MODEL(macro_editor_basic_list_store), &test_iter);
	} else
		valid = gtk_gaminggear_tree_model_get_iter_last(GTK_TREE_MODEL(macro_editor_basic_list_store), &test_iter, NULL);

	if (valid)
		return current_is_period(macro_editor_basic_list_store, &test_iter);
	else
		return FALSE;
}

static gboolean is_first(GaminggearMacroEditorBasicListStore *macro_editor_basic_list_store, GtkTreeIter *iter) {
	gboolean valid;
	GtkTreeIter test_iter;

	if (iter) {
		test_iter = *iter;
		valid = gtk_gaminggear_tree_model_get_iter_previous(GTK_TREE_MODEL(macro_editor_basic_list_store), &test_iter);
		return !valid;
	} else
		return gaminggear_macro_editor_basic_list_store_empty(macro_editor_basic_list_store);
}

static gboolean insert_period_before(GaminggearMacroEditorBasicListStore *macro_editor_basic_list_store, GtkTreeIter *sibling, glong rel_time) {
	GtkListStore *store;
	GtkTreeIter iter;
	gchar *title;

	if (rel_time == 0)
		return FALSE;

	if (is_first(macro_editor_basic_list_store, sibling))
		return FALSE;

	if (previous_is_period(macro_editor_basic_list_store, sibling))
		return FALSE;

	if (current_is_period(macro_editor_basic_list_store, sibling))
		return FALSE;

	store = GTK_LIST_STORE(macro_editor_basic_list_store);

	title = g_strdup_printf("%li", rel_time);

	gtk_list_store_insert_before(store, &iter, sibling);
	gtk_list_store_set(store, &iter,
			GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_ICON_COLUMN, "gaminggear-wait",
			GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_TEXT_COLUMN, title,
			GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_IS_KEY_COLUMN, FALSE,
			GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_IS_NOT_KEY_COLUMN, TRUE,
			GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_PERIOD_COLUMN, rel_time,
			-1);
	g_free(title);

	gaminggear_macro_editor_basic_list_store_set_modified(macro_editor_basic_list_store, TRUE);

	return TRUE;
}

gboolean gaminggear_macro_editor_basic_list_store_insert_period_before(GaminggearMacroEditorBasicListStore *macro_editor_basic_list_store, GtkTreeIter *sibling, glong rel_time) {
	return insert_period_before(macro_editor_basic_list_store, sibling, rel_time);
}

void gaminggear_macro_editor_basic_list_store_insert_keystroke_before(GaminggearMacroEditorBasicListStore *macro_editor_basic_list_store, GtkTreeIter *sibling, guint key, guint action, glong rel_time) {
	insert_period_before(macro_editor_basic_list_store, sibling, rel_time);
	insert_single_action_key_before(macro_editor_basic_list_store, sibling, key, action);
}

static void gaminggear_macro_editor_basic_list_store_insert_gaminggear_macro_keystroke_before(GaminggearMacroEditorBasicListStore *macro_editor_basic_list_store, GtkTreeIter *sibling, GaminggearMacroKeystroke const *keystroke) {
	insert_single_action_key_before(macro_editor_basic_list_store, sibling, keystroke->key, keystroke->action);
	/* period stored in keystroke is waiting time to next event */
	insert_period_before(macro_editor_basic_list_store, sibling, gaminggear_macro_keystroke_get_period(keystroke));
}

gboolean gaminggear_macro_editor_basic_list_store_remove(GaminggearMacroEditorBasicListStore *macro_editor_basic_list_store, GtkTreeIter *iter) {
	gboolean result;

	result = gtk_list_store_remove(GTK_LIST_STORE(macro_editor_basic_list_store), iter);
	if (result) {
		/* iter points now to following element
		 * remove it if its a period */
		if (current_is_period(macro_editor_basic_list_store, iter))
			gtk_list_store_remove(GTK_LIST_STORE(macro_editor_basic_list_store), iter);
	}

	gaminggear_macro_editor_basic_list_store_set_modified(macro_editor_basic_list_store, TRUE);
	if (gaminggear_macro_editor_basic_list_store_empty(macro_editor_basic_list_store))
		empty_changed(macro_editor_basic_list_store);

	return result;
}

void gaminggear_macro_editor_basic_list_store_set_keystrokes(GaminggearMacroEditorBasicListStore *macro_editor_basic_list_store, GaminggearMacroKeystrokes const *keystrokes) {
	guint count;
	guint i;

	gaminggear_macro_editor_basic_list_store_clear(macro_editor_basic_list_store);

	count = gaminggear_macro_keystrokes_get_count(keystrokes);
	for (i = 0; i < count; ++i)
		gaminggear_macro_editor_basic_list_store_insert_gaminggear_macro_keystroke_before(macro_editor_basic_list_store, NULL, &keystrokes->keystrokes[i]);
}

GaminggearMacroKeystrokes *gaminggear_macro_editor_basic_list_store_get_keystrokes(GaminggearMacroEditorBasicListStore *macro_editor_basic_list_store) {
	GtkTreeModel *model;
	GtkTreeIter iter;
	GaminggearMacroKeystrokes *keystrokes;
	GaminggearMacroKeystroke keystroke;
	gboolean added;
	gboolean key_type;
	guint key;
	guint action;
	guint period;

	keystrokes = gaminggear_macro_keystrokes_new();

	model = GTK_TREE_MODEL(macro_editor_basic_list_store);
	if (!gtk_tree_model_get_iter_first(model, &iter))
		return keystrokes;

	added = TRUE;

	do {
		gtk_tree_model_get(model, &iter,
				GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_IS_KEY_COLUMN, &key_type,
				GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_KEY_COLUMN, &key,
				GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_ACTION_COLUMN, &action,
				GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_PERIOD_COLUMN, &period,
				-1);

		if (key_type) {
			if (!added)
				gaminggear_macro_keystrokes_add(keystrokes, &keystroke);

			keystroke.action = action;
			keystroke.key = key;
			gaminggear_macro_keystroke_set_period(&keystroke, 0);
			added = FALSE;
		} else {
			if (added)
				g_critical(_("Wait without corresponding key event"));
			gaminggear_macro_keystroke_set_period(&keystroke, period);
		}

	} while (gtk_tree_model_iter_next(model, &iter));

	if (!added)
		gaminggear_macro_keystrokes_add(keystrokes, &keystroke);

	return keystrokes;
}

void gaminggear_macro_editor_basic_list_store_set_value(GaminggearMacroEditorBasicListStore *macro_editor_basic_list_store, GtkTreeIter *iter, gchar const *text) {
	gint value;

	value = atoi(text);
	if (value < 1 || value > GAMINGGEAR_MACRO_KEYSTROKE_PERIOD_MAX)
		return;

	gtk_list_store_set(GTK_LIST_STORE(macro_editor_basic_list_store), iter,
			GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_PERIOD_COLUMN, value,
			GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_TEXT_COLUMN, text,
			-1);

	gaminggear_macro_editor_basic_list_store_set_modified(macro_editor_basic_list_store, TRUE);
}

void gaminggear_macro_editor_basic_list_store_clear(GaminggearMacroEditorBasicListStore *macro_editor_basic_list_store) {
	gboolean was_empty = gaminggear_macro_editor_basic_list_store_empty(macro_editor_basic_list_store);

	gtk_list_store_clear(GTK_LIST_STORE(macro_editor_basic_list_store));
	gaminggear_macro_editor_basic_list_store_set_modified(macro_editor_basic_list_store, TRUE);
	if (!was_empty)
		empty_changed(macro_editor_basic_list_store);
}

gboolean gaminggear_macro_editor_basic_list_store_empty(GaminggearMacroEditorBasicListStore *macro_editor_basic_list_store) {
	GtkTreeIter iter;
	return !gtk_tree_model_get_iter_first(GTK_TREE_MODEL(macro_editor_basic_list_store), &iter);
}

guint gaminggear_macro_editor_basic_list_store_icon_column(void) {
	return GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_ICON_COLUMN;
}

guint gaminggear_macro_editor_basic_list_store_text_column(void) {
	return GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_TEXT_COLUMN;
}

guint gaminggear_macro_editor_basic_list_store_is_key_column(void) {
	return GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_IS_KEY_COLUMN;
}

guint gaminggear_macro_editor_basic_list_store_is_not_key_column(void) {
	return GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_IS_NOT_KEY_COLUMN;
}
