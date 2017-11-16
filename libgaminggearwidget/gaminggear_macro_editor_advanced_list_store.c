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

#include "gaminggear_macro_editor_advanced_list_store.h"
#include "gaminggear_dscale.h"
#include "gaminggear/gdk_key_translations.h"
#include "gaminggearwidget_helper.h"
#include "gaminggear_helper.h"
#include "gaminggear_dialogs.h"
#include "i18n-lib.h"
#include <math.h>

// TODO bookkeeping of max for speedup
// FIXME recalc abs_time after some upper changed

#define GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_TYPE, GaminggearMacroEditorAdvancedListStoreClass))
#define IS_GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_TYPE))
#define GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_TYPE, GaminggearMacroEditorAdvancedListStorePrivate))

typedef struct _GaminggearMacroEditorAdvancedListStoreClass GaminggearMacroEditorAdvancedListStoreClass;

struct _GaminggearMacroEditorAdvancedListStoreClass {
	GtkListStoreClass parent_class;
	void (*max_changed)(GaminggearMacroEditorAdvancedListStore *gaminggear_macro_editor_advanced_list_store);
	void (*empty_changed)(GaminggearMacroEditorAdvancedListStore *gaminggear_macro_editor_advanced_list_store);
	void (*modified)(GaminggearMacroEditorAdvancedListStore *gaminggear_macro_editor_advanced_list_store);
	void (*keypress_added)(GaminggearMacroEditorAdvancedListStore *gaminggear_macro_editor_advanced_list_store, GtkTreeIter *iter);
};

struct _GaminggearMacroEditorAdvancedListStorePrivate {
	gboolean modified;
	glong abs_time;
};

G_DEFINE_TYPE(GaminggearMacroEditorAdvancedListStore, gaminggear_macro_editor_advanced_list_store, GTK_TYPE_LIST_STORE);

enum {
	MAX_CHANGED,
	EMPTY_CHANGED,
	MODIFIED,
	KEYPRESS_ADDED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

// TODO thats ugly (see also spin_button_set_value_without_upper_bound)
static gdouble spin_button_initial_max = 10.0;

enum {
	GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_KEY_COLUMN,
	GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_ACTION_COLUMN,
	GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_LABEL_COLUMN,
	GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_LOWER_COLUMN,
	GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_UPPER_COLUMN,
	GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_SCALE_COLUMN,
	GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_N_COLUMNS
};

static glong sec_to_msec(gdouble sec) {
	return (glong)round(sec * (gdouble)G_GAMINGGEAR_MSEC_PER_SEC);
}

static gdouble msec_to_sec(glong msec) {
	return (gdouble)msec / (gdouble)G_GAMINGGEAR_MSEC_PER_SEC;
}

static void modified(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store) {
	g_signal_emit((gpointer)macro_editor_advanced_list_store, signals[MODIFIED], 0);
}

static void keypress_added(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store, GtkTreeIter *iter) {
	g_signal_emit((gpointer)macro_editor_advanced_list_store, signals[KEYPRESS_ADDED], 0, iter);
}

static void max_changed(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store) {
	g_signal_emit((gpointer)macro_editor_advanced_list_store, signals[MAX_CHANGED], 0);
}

static void empty_changed(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store) {
	g_signal_emit((gpointer)macro_editor_advanced_list_store, signals[EMPTY_CHANGED], 0);
}

void gaminggear_macro_editor_advanced_list_store_set_modified(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store, gboolean state) {
	macro_editor_advanced_list_store->priv->modified = state;
	if (state)
		modified(macro_editor_advanced_list_store);
}

gboolean gaminggear_macro_editor_advanced_list_store_get_modified(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store) {
	return macro_editor_advanced_list_store->priv->modified;
}

GaminggearMacroEditorAdvancedListStore *gaminggear_macro_editor_advanced_list_store_new(void) {
	GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store;
	GType types[GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_N_COLUMNS] = {
		G_TYPE_UINT,
		G_TYPE_UINT,
		G_TYPE_OBJECT,
		G_TYPE_OBJECT,
		G_TYPE_OBJECT,
		G_TYPE_OBJECT
	};

	macro_editor_advanced_list_store = g_object_new(GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_TYPE,
			NULL);

	gtk_list_store_set_column_types(GTK_LIST_STORE(macro_editor_advanced_list_store),
			GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_N_COLUMNS, types);

	return macro_editor_advanced_list_store;
}

static void gaminggear_macro_editor_advanced_list_store_init(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store) {
	GaminggearMacroEditorAdvancedListStorePrivate *priv = GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_GET_PRIVATE(macro_editor_advanced_list_store);

	macro_editor_advanced_list_store->priv = priv;

	priv->modified = FALSE;
	priv->abs_time = 0L;
}

static void gaminggear_macro_editor_advanced_list_store_finalize(GObject *object) {
	G_OBJECT_CLASS(gaminggear_macro_editor_advanced_list_store_parent_class)->finalize(object);
}

static void gaminggear_macro_editor_advanced_list_store_class_init(GaminggearMacroEditorAdvancedListStoreClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->finalize = gaminggear_macro_editor_advanced_list_store_finalize;

	g_type_class_add_private(klass, sizeof(GaminggearMacroEditorAdvancedListStorePrivate));

	signals[MAX_CHANGED] = g_signal_new("max-changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			G_STRUCT_OFFSET(GaminggearMacroEditorAdvancedListStoreClass, max_changed),
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

	signals[EMPTY_CHANGED] = g_signal_new("empty-changed",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			G_STRUCT_OFFSET(GaminggearMacroEditorAdvancedListStoreClass, empty_changed),
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

	signals[MODIFIED] = g_signal_new("modified",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			G_STRUCT_OFFSET(GaminggearMacroEditorAdvancedListStoreClass, modified),
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

	signals[KEYPRESS_ADDED] = g_signal_new("keypress-added",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			G_STRUCT_OFFSET(GaminggearMacroEditorAdvancedListStoreClass, keypress_added),
			NULL, NULL, g_cclosure_marshal_VOID__BOXED, G_TYPE_NONE, 1, GTK_TYPE_TREE_ITER);
}

/*
 * Settings max to G_MAXDOUBLE makes the spinbutton bigger than needed.
 * TODO how to disable upper bound a better way?
 */
static void spin_button_set_value_without_upper_bound(GtkSpinButton *spin, gdouble new_value) {
	gdouble max;
	gtk_spin_button_get_range(spin, NULL, &max);
	/* + 1 lets me simulate variable max for spin button */
	gtk_spin_button_set_range(spin, 0, MAX(max, new_value + 0.001));
	gtk_spin_button_set_value(spin, new_value);
}

static void dscale_max_changed_cb(GaminggearDscale *dscale, gpointer user_data) {
	GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store = GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE(user_data);
	gdouble max;
	max = gaminggear_dscale_get_max(dscale);
	gaminggear_macro_editor_advanced_list_store_set_all_max(macro_editor_advanced_list_store, max);
	max_changed(macro_editor_advanced_list_store);
}

static void dscale_lower_changed_cb(GaminggearDscale *dscale, gpointer user_data) {
	GtkTreeRowReference *reference = (GtkTreeRowReference *)user_data;
	GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store;
	GtkWidget *spin;
	GtkTreeIter iter;

	macro_editor_advanced_list_store = GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE(gtk_tree_row_reference_get_model(reference));
	gtk_gaminggear_tree_row_reference_get_iter(reference, &iter);

	spin = gaminggear_macro_editor_advanced_list_store_get_lower_widget(macro_editor_advanced_list_store, &iter);
	spin_button_set_value_without_upper_bound(GTK_SPIN_BUTTON(spin), gaminggear_dscale_get_lower(dscale));
	g_object_unref(spin);

	gaminggear_macro_editor_advanced_list_store_set_modified(macro_editor_advanced_list_store, TRUE);
}

static void dscale_upper_changed_cb(GaminggearDscale *dscale, gpointer user_data) {
	GtkTreeRowReference *reference = (GtkTreeRowReference *)user_data;
	GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store;
	GtkWidget *spin;
	GtkTreeIter iter;

	macro_editor_advanced_list_store = GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE(gtk_tree_row_reference_get_model(reference));
	gtk_gaminggear_tree_row_reference_get_iter(reference, &iter);

	spin = gaminggear_macro_editor_advanced_list_store_get_upper_widget(macro_editor_advanced_list_store, &iter);
	spin_button_set_value_without_upper_bound(GTK_SPIN_BUTTON(spin), gaminggear_dscale_get_upper(dscale));
	g_object_unref(spin);

	gaminggear_macro_editor_advanced_list_store_set_modified(macro_editor_advanced_list_store, TRUE);
}

static void spin_button_lower_changed_cb(GtkSpinButton *spin_button, gpointer user_data) {
	GtkTreeRowReference *reference = (GtkTreeRowReference *)user_data;
	GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store;
	GtkWidget *scale;
	GtkTreeIter iter;

	macro_editor_advanced_list_store = GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE(gtk_tree_row_reference_get_model(reference));
	gtk_gaminggear_tree_row_reference_get_iter(reference, &iter);

	scale = gaminggear_macro_editor_advanced_list_store_get_scale_widget(macro_editor_advanced_list_store, &iter);
	gaminggear_dscale_set_lower(GAMINGGEAR_DSCALE(scale), gtk_spin_button_get_value(spin_button));
	g_object_unref(scale);

	gaminggear_macro_editor_advanced_list_store_set_modified(macro_editor_advanced_list_store, TRUE);
}

static void spin_button_upper_changed_cb(GtkSpinButton *spin_button, gpointer user_data) {
	GtkTreeRowReference *reference = (GtkTreeRowReference *)user_data;
	GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store;
	GtkWidget *scale;
	GtkTreeIter iter;

	macro_editor_advanced_list_store = GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE(gtk_tree_row_reference_get_model(reference));
	gtk_gaminggear_tree_row_reference_get_iter(reference, &iter);

	scale = gaminggear_macro_editor_advanced_list_store_get_scale_widget(macro_editor_advanced_list_store, &iter);
	gaminggear_dscale_set_upper(GAMINGGEAR_DSCALE(scale), gtk_spin_button_get_value(spin_button));
	g_object_unref(scale);

	gaminggear_macro_editor_advanced_list_store_set_modified(macro_editor_advanced_list_store, TRUE);
}

static void set_label(GtkLabel *label, guint usage_id) {
	gchar *keyname;
	keyname = gaminggear_hid_to_keyname(usage_id);
	gtk_label_set_text(label, keyname);
	g_free(keyname);
}

static void add_press(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store,
		GtkTreeIter *iter, guint key, guint action) {
	GtkLabel *label;
	GtkSpinButton *lower;
	GtkSpinButton *upper;
	GaminggearDscale *scale;
	GtkTreeRowReference *reference;
	gboolean was_empty;
	gdouble seconds;

	was_empty = gaminggear_macro_editor_advanced_list_store_empty(macro_editor_advanced_list_store);

	gtk_list_store_append(GTK_LIST_STORE(macro_editor_advanced_list_store), iter);

	// FIXME Store reference in store (as boxed?) and make sure gtk_tree_row_reference_free is called
	reference = gtk_gaminggear_tree_row_reference_new_from_iter(GTK_TREE_MODEL(macro_editor_advanced_list_store), iter);

	label = GTK_LABEL(gtk_label_new(NULL));

	lower = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.0, spin_button_initial_max, 0.001));
	gtk_spin_button_set_digits(lower, 3);
	gtk_spin_button_set_update_policy(lower, GTK_UPDATE_IF_VALID);

	upper = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.0, spin_button_initial_max, 0.001));
	gtk_spin_button_set_digits(upper, 3);
	gtk_spin_button_set_update_policy(upper, GTK_UPDATE_IF_VALID);

	scale = GAMINGGEAR_DSCALE(gaminggear_dscale_new());
	gaminggear_dscale_set_min(scale, (gdouble)0.0);
	gaminggear_dscale_set_fix_min(scale, TRUE);
	gaminggear_dscale_set_fix_max(scale, FALSE);

	gtk_list_store_set(GTK_LIST_STORE(macro_editor_advanced_list_store), iter,
			GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_KEY_COLUMN, key,
			GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_ACTION_COLUMN, action,
			GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_LABEL_COLUMN, label,
			GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_LOWER_COLUMN, lower,
			GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_UPPER_COLUMN, upper,
			GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_SCALE_COLUMN, scale,
			-1);

	seconds = msec_to_sec(macro_editor_advanced_list_store->priv->abs_time);

	set_label(label, key);
	spin_button_set_value_without_upper_bound(lower, seconds);
	gaminggear_dscale_set_lower(scale, seconds);

	g_signal_connect(G_OBJECT(lower), "value-changed", G_CALLBACK(spin_button_lower_changed_cb), reference);
	g_signal_connect(G_OBJECT(upper), "value-changed", G_CALLBACK(spin_button_upper_changed_cb), reference);
	g_signal_connect(G_OBJECT(scale), "max-changed", G_CALLBACK(dscale_max_changed_cb), macro_editor_advanced_list_store);
	g_signal_connect(G_OBJECT(scale), "lower-changed", G_CALLBACK(dscale_lower_changed_cb), reference);
	g_signal_connect(G_OBJECT(scale), "upper-changed", G_CALLBACK(dscale_upper_changed_cb), reference);

	gaminggear_macro_editor_advanced_list_store_set_modified(macro_editor_advanced_list_store, TRUE);
	keypress_added(macro_editor_advanced_list_store, iter);
	if (was_empty)
		empty_changed(macro_editor_advanced_list_store);
}

static void add_release(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store,
		GtkTreeIter *iter, GtkTreeIter *press, guint key, guint action) {
	GtkWidget *label;
	GtkWidget *lower;
	GtkWidget *upper;
	GtkWidget *scale;
	gboolean was_empty;
	gdouble seconds;

	was_empty = gaminggear_macro_editor_advanced_list_store_empty(macro_editor_advanced_list_store);

	gtk_list_store_append(GTK_LIST_STORE(macro_editor_advanced_list_store), iter);

	gtk_tree_model_get(GTK_TREE_MODEL(macro_editor_advanced_list_store), press,
			GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_LABEL_COLUMN, &label,
			GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_LOWER_COLUMN, &lower,
			GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_UPPER_COLUMN, &upper,
			GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_SCALE_COLUMN, &scale,
			-1);

	gtk_list_store_set(GTK_LIST_STORE(macro_editor_advanced_list_store), iter,
			GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_KEY_COLUMN, key,
			GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_ACTION_COLUMN, action,
			GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_LABEL_COLUMN, label,
			GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_LOWER_COLUMN, lower,
			GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_UPPER_COLUMN, upper,
			GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_SCALE_COLUMN, scale,
			-1);

	seconds = msec_to_sec(macro_editor_advanced_list_store->priv->abs_time);

	gaminggear_dscale_set_upper(GAMINGGEAR_DSCALE(scale), seconds);
	spin_button_set_value_without_upper_bound(GTK_SPIN_BUTTON(upper), seconds);

	gaminggear_macro_editor_advanced_list_store_set_modified(macro_editor_advanced_list_store, TRUE);
	if (was_empty)
		empty_changed(macro_editor_advanced_list_store);
}

static guint get_key(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store, GtkTreeIter *iter) {
	guint key;
	gtk_tree_model_get(GTK_TREE_MODEL(macro_editor_advanced_list_store), iter,
			GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_KEY_COLUMN, &key,
			-1);
	return key;
}

static guint get_action(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store, GtkTreeIter *iter) {
	guint action;
	gtk_tree_model_get(GTK_TREE_MODEL(macro_editor_advanced_list_store), iter,
			GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_ACTION_COLUMN, &action,
			-1);
	return action;
}

static gdouble get_max(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store, GtkTreeIter *iter) {
	GtkWidget *dscale;
	gdouble value;
	dscale = gaminggear_macro_editor_advanced_list_store_get_scale_widget(macro_editor_advanced_list_store, iter);
	value = gaminggear_dscale_get_max(GAMINGGEAR_DSCALE(dscale));
	g_object_unref(dscale);
	return value;
}

static gboolean find_backwards(GaminggearMacroEditorAdvancedListStore *store, GtkTreeIter *iter, guint key) {
	guint other_key;
	GtkTreeIter temp_iter;

	if (!gtk_gaminggear_tree_model_get_iter_last(GTK_TREE_MODEL(store), &temp_iter, NULL))
		return FALSE;

	do {
		other_key = get_key(store, &temp_iter);
		if (other_key == key) {
			*iter = temp_iter;
			return TRUE;
		}
	} while (gtk_gaminggear_tree_model_get_iter_previous(GTK_TREE_MODEL(store), &temp_iter));

	return FALSE;
}

static void add_single_action(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store, guint key, guint action) {
	GtkTreeIter press_iter;
	GtkTreeIter iter;
	gboolean iter_is_valid;

	if (action == GAMINGGEAR_MACRO_KEYSTROKE_ACTION_PRESS)
		add_press(macro_editor_advanced_list_store, &iter, key, action);
	else {
		iter_is_valid = find_backwards(macro_editor_advanced_list_store, &press_iter, key);
		if (iter_is_valid)
			add_release(macro_editor_advanced_list_store, &iter, &press_iter, key, action);
		else
			g_warning(_("There is no corresponding press event for key %x"), key);
	}
}

static void add_wait(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store, glong rel_time) {
	/* initial waiting time is not supported */
	if (!gaminggear_macro_editor_advanced_list_store_empty(macro_editor_advanced_list_store))
		macro_editor_advanced_list_store->priv->abs_time += rel_time;
}

void gaminggear_macro_editor_advanced_list_store_add_keystroke(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store, guint key, guint action, glong rel_time) {
	add_wait(macro_editor_advanced_list_store, rel_time);
	add_single_action(macro_editor_advanced_list_store, key, action);
}

GtkWidget *gaminggear_macro_editor_advanced_list_store_get_label_widget(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store, GtkTreeIter *iter) {
	GtkWidget *widget;
	gtk_tree_model_get(GTK_TREE_MODEL(macro_editor_advanced_list_store), iter,
			GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_LABEL_COLUMN, &widget,
			-1);
	return widget;
}

GtkWidget *gaminggear_macro_editor_advanced_list_store_get_lower_widget(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store, GtkTreeIter *iter) {
	GtkWidget *widget;
	gtk_tree_model_get(GTK_TREE_MODEL(macro_editor_advanced_list_store), iter,
			GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_LOWER_COLUMN, &widget,
			-1);
	return widget;
}

GtkWidget *gaminggear_macro_editor_advanced_list_store_get_upper_widget(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store, GtkTreeIter *iter) {
	GtkWidget *widget;
	gtk_tree_model_get(GTK_TREE_MODEL(macro_editor_advanced_list_store), iter,
			GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_UPPER_COLUMN, &widget,
			-1);
	return widget;
}

GtkWidget *gaminggear_macro_editor_advanced_list_store_get_scale_widget(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store, GtkTreeIter *iter) {
	GtkWidget *widget;
	gtk_tree_model_get(GTK_TREE_MODEL(macro_editor_advanced_list_store), iter,
			GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_SCALE_COLUMN, &widget,
			-1);
	return widget;
}

gdouble gaminggear_macro_editor_advanced_list_store_get_all_max(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store) {
	gdouble max = 0.0;
	gdouble value;
	GtkTreeIter iter;

	if (!gtk_tree_model_get_iter_first(GTK_TREE_MODEL(macro_editor_advanced_list_store), &iter))
		return 0.0;

	do {
		value = get_max(macro_editor_advanced_list_store, &iter);
		if (value > max)
			max = value;
	} while (gtk_tree_model_iter_next(GTK_TREE_MODEL(macro_editor_advanced_list_store), &iter));

	return max;
}

/* TODO disadvantage: most scales are set twice */
void gaminggear_macro_editor_advanced_list_store_set_all_max(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store, gdouble value) {
	GtkWidget *dscale;
	GtkTreeIter iter;

	if (!gtk_tree_model_get_iter_first(GTK_TREE_MODEL(macro_editor_advanced_list_store), &iter))
		return;

	do {
		dscale = gaminggear_macro_editor_advanced_list_store_get_scale_widget(macro_editor_advanced_list_store, &iter);
		gaminggear_dscale_set_max(GAMINGGEAR_DSCALE(dscale), value);
		g_object_unref(dscale);
	} while (gtk_tree_model_iter_next(GTK_TREE_MODEL(macro_editor_advanced_list_store), &iter));
}

static gdouble get_seconds(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store, GtkTreeIter *iter) {
	GtkWidget *dscale;
	gdouble value;

	dscale = gaminggear_macro_editor_advanced_list_store_get_scale_widget(macro_editor_advanced_list_store, iter);

	if (get_action(macro_editor_advanced_list_store, iter) == GAMINGGEAR_MACRO_KEYSTROKE_ACTION_PRESS)
		value = gaminggear_dscale_get_lower(GAMINGGEAR_DSCALE(dscale));
	else
		value = gaminggear_dscale_get_upper(GAMINGGEAR_DSCALE(dscale));

	g_object_unref(dscale);

	return value;
}

/*
 * Needs a stable sort algorithm. Current implementation is bubble sort.
 * TODO replace current sort implemenation with faster alternative.
 */
void sort(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store) {
	GtkTreeIter actual_iter;
	GtkTreeIter next_iter;
	gboolean swapped = TRUE;
	gdouble actual_value;
	gdouble next_value;

	while (swapped) {
		if (!gtk_tree_model_get_iter_first(GTK_TREE_MODEL(macro_editor_advanced_list_store), &actual_iter))
			return;

		swapped = FALSE;

		while (TRUE) {
			next_iter = actual_iter;
			if (!gtk_tree_model_iter_next(GTK_TREE_MODEL(macro_editor_advanced_list_store), &next_iter))
				break;

			actual_value = get_seconds(macro_editor_advanced_list_store, &actual_iter);
			next_value = get_seconds(macro_editor_advanced_list_store, &next_iter);

			if (next_value < actual_value) {
				gtk_list_store_swap(GTK_LIST_STORE(macro_editor_advanced_list_store), &actual_iter, &next_iter);
				swapped = TRUE;
			}

			actual_iter = next_iter;
		};
	}
}

GaminggearMacroKeystrokes *gaminggear_macro_editor_advanced_list_store_get_keystrokes(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store) {
	GaminggearMacroKeystrokes *keystrokes;
	GaminggearMacroKeystroke keystroke;
	GtkTreeIter actual_iter;
	GtkTreeIter next_iter;
	gdouble actual_value;
	gdouble next_value;
	gboolean next_valid;

	sort(macro_editor_advanced_list_store);

	keystrokes = gaminggear_macro_keystrokes_new();

	if (!gtk_tree_model_get_iter_first(GTK_TREE_MODEL(macro_editor_advanced_list_store), &actual_iter))
		return keystrokes;

	while (TRUE) {
		next_iter = actual_iter;
		next_valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(macro_editor_advanced_list_store), &next_iter);

		if (next_valid) {
			actual_value = get_seconds(macro_editor_advanced_list_store, &actual_iter);
			next_value = get_seconds(macro_editor_advanced_list_store, &next_iter);
			gaminggear_macro_keystroke_set_period(&keystroke, sec_to_msec(next_value - actual_value));
		} else
			gaminggear_macro_keystroke_set_period(&keystroke, 0);

		keystroke.key = get_key(macro_editor_advanced_list_store, &actual_iter);
		keystroke.action = get_action(macro_editor_advanced_list_store, &actual_iter);

		gaminggear_macro_keystrokes_add(keystrokes, &keystroke);

		if (next_valid)
			actual_iter = next_iter;
		else
			break;
	};

	return keystrokes;
}

static void equalize_max_time(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store) {
	gdouble max;
	max = gaminggear_macro_editor_advanced_list_store_get_all_max(macro_editor_advanced_list_store);
	gaminggear_macro_editor_advanced_list_store_set_all_max(macro_editor_advanced_list_store, MAX(1.001, max));
}

void gaminggear_macro_editor_advanced_list_store_set_keystrokes(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store, GaminggearMacroKeystrokes const *macro_keystrokes) {
	GaminggearMacroKeystroke const *keystroke = NULL;
	guint i;
	guint count;
	guint final_wait;
	gchar *text;

	gaminggear_macro_editor_advanced_list_store_clear(macro_editor_advanced_list_store);

	count = gaminggear_macro_keystrokes_get_count(macro_keystrokes);
	for (i = 0; i < count; ++i) {
		keystroke = &macro_keystrokes->keystrokes[i];
		add_single_action(macro_editor_advanced_list_store, keystroke->key, keystroke->action);
		add_wait(macro_editor_advanced_list_store, gaminggear_macro_keystroke_get_period(keystroke));
	}

	if (keystroke) {
		final_wait = gaminggear_macro_keystroke_get_period(keystroke);
		if (final_wait != 0) {
			text = g_strdup_printf(_("This macro contains a final wait of %u ms"), final_wait);
			gaminggear_warning_dialog(NULL, text,
					_("Advanced view does not support final waits. Please switch to another view to add it again if needed."));
			g_free(text);
		}
	}

	equalize_max_time(macro_editor_advanced_list_store);
}

void gaminggear_macro_editor_advanced_list_store_clear(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store) {
	gboolean was_empty;

	was_empty = gaminggear_macro_editor_advanced_list_store_empty(macro_editor_advanced_list_store);

	gtk_list_store_clear(GTK_LIST_STORE(macro_editor_advanced_list_store));
	macro_editor_advanced_list_store->priv->abs_time = 0L;
	modified(macro_editor_advanced_list_store);

	if (!was_empty)
		empty_changed(macro_editor_advanced_list_store);
}

gboolean gaminggear_macro_editor_advanced_list_store_empty(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store) {
	GtkTreeIter iter;
	return !gtk_tree_model_get_iter_first(GTK_TREE_MODEL(macro_editor_advanced_list_store), &iter);
}
