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

#include "gaminggear_macro_tree_store.h"
#include "gaminggearwidget_helper.h"
#include "i18n-lib.h"
#include <string.h>

#define GAMINGGEAR_MACRO_TREE_STORE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GAMINGGEAR_MACRO_TREE_STORE_TYPE, GaminggearMacroTreeStoreClass))
#define IS_GAMINGGEAR_MACRO_TREE_STORE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GAMINGGEAR_MACRO_TREE_STORE_TYPE))
#define GAMINGGEAR_MACRO_TREE_STORE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), GAMINGGEAR_MACRO_TREE_STORE_TYPE, GaminggearMacroTreeStorePrivate))

typedef struct _GaminggearMacroTreeStoreClass GaminggearMacroTreeStoreClass;

struct _GaminggearMacroTreeStoreClass {
	GtkTreeStoreClass parent_class;
	void (*modified)(GaminggearMacroTreeStore *macro_tree_store);
};

struct _GaminggearMacroTreeStorePrivate {
	gboolean modified;
};

G_DEFINE_TYPE(GaminggearMacroTreeStore, gaminggear_macro_tree_store, GTK_TYPE_TREE_STORE);

enum {
	MODIFIED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

enum {
	GAMINGGEAR_MACRO_TREE_STORE_NAME_COLUMN,
	GAMINGGEAR_MACRO_TREE_STORE_POINTER_COLUMN,
	GAMINGGEAR_MACRO_TREE_STORE_N_COLUMNS
};

void gaminggear_macro_tree_store_set_modified(GaminggearMacroTreeStore *macro_tree_store, gboolean state) {
	macro_tree_store->priv->modified = state;
	if (state)
		g_signal_emit((gpointer)macro_tree_store, signals[MODIFIED], 0);
}

gboolean gaminggear_macro_tree_store_get_modified(GaminggearMacroTreeStore *macro_tree_store) {
	return macro_tree_store->priv->modified;
}

GaminggearMacroTreeStore *gaminggear_macro_tree_store_new(void) {
	GaminggearMacroTreeStore *macro_tree_store;

	macro_tree_store = g_object_new(GAMINGGEAR_MACRO_TREE_STORE_TYPE, NULL);

	return macro_tree_store;
}

static void gaminggear_macro_tree_store_init(GaminggearMacroTreeStore *macro_tree_store) {
	GaminggearMacroTreeStorePrivate *priv = GAMINGGEAR_MACRO_TREE_STORE_GET_PRIVATE(macro_tree_store);
	GType types[GAMINGGEAR_MACRO_TREE_STORE_N_COLUMNS] = {
		G_TYPE_STRING,
		G_TYPE_POINTER
	};

	macro_tree_store->priv = priv;

	priv->modified = FALSE;

	gtk_tree_store_set_column_types(GTK_TREE_STORE(macro_tree_store),
			GAMINGGEAR_MACRO_TREE_STORE_N_COLUMNS, types);

	gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(macro_tree_store),
			GAMINGGEAR_MACRO_TREE_STORE_NAME_COLUMN,
			gtk_gaminggear_tree_iter_string_case_compare_func,
			GINT_TO_POINTER(GAMINGGEAR_MACRO_TREE_STORE_NAME_COLUMN),
			NULL);

	gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(macro_tree_store),
			GAMINGGEAR_MACRO_TREE_STORE_NAME_COLUMN, GTK_SORT_ASCENDING);
}

static void gaminggear_macro_tree_store_class_init(GaminggearMacroTreeStoreClass *klass) {
	g_type_class_add_private(klass, sizeof(GaminggearMacroTreeStorePrivate));

	signals[MODIFIED] = g_signal_new("modified",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			G_STRUCT_OFFSET(GaminggearMacroTreeStoreClass, modified),
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}

guint gaminggear_macro_tree_store_name_column(void) {
	return GAMINGGEAR_MACRO_TREE_STORE_NAME_COLUMN;
}

static gboolean gaminggear_macro_tree_store_find_iter(GaminggearMacroTreeStore *macro_tree_store, GtkTreeIter *parent, GtkTreeIter *iter, gchar const *name) {
	gchar *name_on_test;
	gboolean test;
	GtkTreeModel *model;

	model = GTK_TREE_MODEL(macro_tree_store);

	if (!parent) {
		if (!gtk_tree_model_get_iter_first(model, iter))
			return FALSE;
	} else {
		if (!gtk_tree_model_iter_children(model, iter, parent))
			return FALSE;
	}
	do {
		gtk_tree_model_get(model, iter, GAMINGGEAR_MACRO_TREE_STORE_NAME_COLUMN, &name_on_test, -1);
		test = strcmp(name, name_on_test) == 0;
		g_free(name_on_test);
		if (test)
			return TRUE;
	} while (gtk_tree_model_iter_next(model, iter));

	return FALSE;
}

gboolean gaminggear_macro_tree_store_is_macroset_name_unique(GaminggearMacroTreeStore *macro_tree_store, gchar const *macroset_name) {
	GtkTreeIter iter;
	return !gaminggear_macro_tree_store_find_iter(macro_tree_store, NULL, &iter, macroset_name);
}

gboolean gaminggear_macro_tree_store_is_macro_name_unique(GaminggearMacroTreeStore *macro_tree_store, GtkTreeIter *macroset_iter, gchar const *macro_name) {
	GtkTreeIter iter;
	return !gaminggear_macro_tree_store_find_iter(macro_tree_store, macroset_iter, &iter, macro_name);
}

gboolean gaminggear_macro_tree_store_is_macroset_with_iter(GaminggearMacroTreeStore *macro_tree_store, GtkTreeIter *iter) {
	GtkTreeIter parent;
	return !gtk_tree_model_iter_parent(GTK_TREE_MODEL(macro_tree_store), &parent, iter);
}

gboolean gaminggear_macro_tree_store_is_macroset_with_path(GtkTreePath *path) {
	return (gtk_tree_path_get_depth(path) == 1) ? TRUE : FALSE;
}

static void add_macroset(GaminggearMacroTreeStore *macro_tree_store, GtkTreeIter *iter, gchar const *name) {
	gtk_gaminggear_tree_store_append_with_values(GTK_TREE_STORE(macro_tree_store), iter, NULL,
			GAMINGGEAR_MACRO_TREE_STORE_NAME_COLUMN, name,
			GAMINGGEAR_MACRO_TREE_STORE_POINTER_COLUMN, NULL,
			-1);
}

static void add_macro_with_keystrokes(GaminggearMacroTreeStore *macro_tree_store, GtkTreeIter *macroset_iter, GtkTreeIter *macro_iter, gchar const *macro_name, GaminggearMacroKeystrokes *keystrokes) {
	gtk_gaminggear_tree_store_append_with_values(GTK_TREE_STORE(macro_tree_store), macro_iter, macroset_iter,
			GAMINGGEAR_MACRO_TREE_STORE_NAME_COLUMN, macro_name,
			GAMINGGEAR_MACRO_TREE_STORE_POINTER_COLUMN, gaminggear_macro_keystrokes_dup(keystrokes),
			-1);
}

void gaminggear_macro_tree_store_add_macroset(GaminggearMacroTreeStore *macro_tree_store, GtkTreeIter *iter, gchar const *name) {
	add_macroset(macro_tree_store, iter, name);
	gaminggear_macro_tree_store_set_modified(macro_tree_store, TRUE);
}

void gaminggear_macro_tree_store_add_macro_with_keystrokes(GaminggearMacroTreeStore *macro_tree_store, GtkTreeIter *macroset_iter, GtkTreeIter *macro_iter, gchar const *macro_name, GaminggearMacroKeystrokes *keystrokes) {
	add_macro_with_keystrokes(macro_tree_store, macroset_iter, macro_iter, macro_name, keystrokes);
	gaminggear_macro_tree_store_set_modified(macro_tree_store, TRUE);
}

void gaminggear_macro_tree_store_add_macro(GaminggearMacroTreeStore *macro_tree_store, GtkTreeIter *macroset_iter, GtkTreeIter *macro_iter, gchar const *macro_name) {
	gtk_gaminggear_tree_store_append_with_values(GTK_TREE_STORE(macro_tree_store), macro_iter, macroset_iter,
			GAMINGGEAR_MACRO_TREE_STORE_NAME_COLUMN, macro_name,
			GAMINGGEAR_MACRO_TREE_STORE_POINTER_COLUMN, gaminggear_macro_keystrokes_new(),
			-1);
	gaminggear_macro_tree_store_set_modified(macro_tree_store, TRUE);
}

gboolean gaminggear_macro_tree_store_add_macros(GaminggearMacroTreeStore *macro_tree_store, GaminggearMacros *gaminggear_macros) {
	GtkTreeIter macroset_iter;
	gchar **macroset_names, **macroset_name, **macro_names, **macro_name;
	GError *error = NULL;
	GaminggearMacro *gaminggear_macro;
	gboolean retval = FALSE;

	macroset_names = gaminggear_macros_get_macrosets(gaminggear_macros, NULL);
	for (macroset_name = macroset_names; *macroset_name; ++macroset_name) {

		macro_names = gaminggear_macros_get_macros(gaminggear_macros, *macroset_name, NULL, &error);
		/* This happens only if macroset is not found */
		if (!macro_names)
			g_error(_("Could not find macroset %s: %s"), *macroset_name, error->message);

		/*
		 * Add macroset event if its empty.
		 * Don't create duplicates.
		 */
		if (gaminggear_macro_tree_store_is_macroset_name_unique(macro_tree_store, *macroset_name)) {
			add_macroset(macro_tree_store, &macroset_iter, *macroset_name);
			retval = TRUE;
		} else
			gaminggear_macro_tree_store_find_iter(macro_tree_store, NULL, &macroset_iter, *macroset_name);

		for (macro_name = macro_names; *macro_name; ++macro_name) {
			gaminggear_macro = gaminggear_macros_get(gaminggear_macros, *macroset_name, *macro_name, &error);
			if (error)
				g_error(_("Could not find macro %s/%s: %s"), *macroset_name, *macro_name, error->message);

			/* not adding already included macros */
			if (gaminggear_macro_tree_store_is_macro_name_unique(macro_tree_store, &macroset_iter, *macro_name)) {
				add_macro_with_keystrokes(macro_tree_store, &macroset_iter, NULL, *macro_name, gaminggear_macro_get_keystrokes(gaminggear_macro));
				retval = TRUE;
			}

			gaminggear_macro_free(gaminggear_macro);
		}
		g_strfreev(macro_names);
	}
	g_strfreev(macroset_names);
	gaminggear_macro_tree_store_set_modified(macro_tree_store, TRUE);

	return retval;
}

GaminggearMacros *gaminggear_macro_tree_store_get_macros(GaminggearMacroTreeStore *macro_tree_store) {
	GaminggearMacros *gaminggear_macros;
	GaminggearMacro *gaminggear_macro;
	GtkTreeIter macroset_iter, macro_iter;
	GtkTreeModel *model;

	model = GTK_TREE_MODEL(macro_tree_store);

	gaminggear_macros = gaminggear_macros_new();

	if (!gtk_tree_model_get_iter_first(model, &macroset_iter))
			return gaminggear_macros;

	do {
		if (!gtk_tree_model_iter_children(model, &macro_iter, &macroset_iter))
			continue;

		do {
			gaminggear_macro = gaminggear_macro_tree_store_get_macro(macro_tree_store, &macro_iter);
			gaminggear_macros_set(gaminggear_macros, gaminggear_macro);
			gaminggear_macro_free(gaminggear_macro);

		} while (gtk_tree_model_iter_next(model, &macro_iter));

	} while (gtk_tree_model_iter_next(model, &macroset_iter));

	return gaminggear_macros;
}

gboolean gaminggear_macro_tree_store_remove(GaminggearMacroTreeStore *macro_tree_store, GtkTreeIter *iter) {
	GtkTreeIter child_iter;
	GaminggearMacroKeystrokes *keystrokes;
	GtkTreeModel *model;
	gboolean retval;

	model = GTK_TREE_MODEL(macro_tree_store);

	while (gtk_tree_model_iter_children(model, &child_iter, iter))
		gaminggear_macro_tree_store_remove(macro_tree_store, &child_iter);

	gtk_tree_model_get(model, iter,
			GAMINGGEAR_MACRO_TREE_STORE_POINTER_COLUMN, &keystrokes,
			-1);
	gaminggear_macro_keystrokes_free(keystrokes);
	retval = gtk_tree_store_remove(GTK_TREE_STORE(model), iter);
	gaminggear_macro_tree_store_set_modified(macro_tree_store, TRUE);
	return retval;
}

gchar *gaminggear_macro_tree_store_get_name(GaminggearMacroTreeStore *macro_tree_store, GtkTreeIter *macro_iter) {
	gchar *name;
	gtk_tree_model_get(GTK_TREE_MODEL(macro_tree_store), macro_iter,
			GAMINGGEAR_MACRO_TREE_STORE_NAME_COLUMN, &name,
			-1);
	return name;
}

GaminggearMacroKeystrokes *gaminggear_macro_tree_store_get_keystrokes(GaminggearMacroTreeStore *macro_tree_store, GtkTreeIter *macro_iter) {
	GaminggearMacroKeystrokes *keystrokes;
	gtk_tree_model_get(GTK_TREE_MODEL(macro_tree_store), macro_iter,
			GAMINGGEAR_MACRO_TREE_STORE_POINTER_COLUMN, &keystrokes,
			-1);
	return gaminggear_macro_keystrokes_dup(keystrokes);
}

void gaminggear_macro_tree_store_set_keystrokes(GaminggearMacroTreeStore *macro_tree_store, GtkTreeIter *macro_iter, GaminggearMacroKeystrokes *keystrokes) {
	GaminggearMacroKeystrokes *old_keystrokes;

	gtk_tree_model_get(GTK_TREE_MODEL(macro_tree_store), macro_iter,
			GAMINGGEAR_MACRO_TREE_STORE_POINTER_COLUMN, &old_keystrokes,
			-1);

	gaminggear_macro_keystrokes_free(old_keystrokes);

	gtk_tree_store_set(GTK_TREE_STORE(macro_tree_store), macro_iter,
			GAMINGGEAR_MACRO_TREE_STORE_POINTER_COLUMN, gaminggear_macro_keystrokes_dup(keystrokes),
			-1);
	gaminggear_macro_tree_store_set_modified(macro_tree_store, TRUE);
}

GaminggearMacro *gaminggear_macro_tree_store_get_macro(GaminggearMacroTreeStore *macro_tree_store, GtkTreeIter *macro_iter) {
	GaminggearMacro *macro;
	gchar *macroset_name;
	gchar *macro_name;
	GaminggearMacroKeystrokes *keystrokes;
	GtkTreeIter macroset_iter;
	GtkTreeModel *model;

	if (gaminggear_macro_tree_store_is_macroset_with_iter(macro_tree_store, macro_iter))
		return NULL;

	model = GTK_TREE_MODEL(macro_tree_store);

	gtk_tree_model_iter_parent(model, &macroset_iter, macro_iter);
	gtk_tree_model_get(model, &macroset_iter,
			GAMINGGEAR_MACRO_TREE_STORE_NAME_COLUMN, &macroset_name,
			-1);
	gtk_tree_model_get(model, macro_iter,
			GAMINGGEAR_MACRO_TREE_STORE_NAME_COLUMN, &macro_name,
			GAMINGGEAR_MACRO_TREE_STORE_POINTER_COLUMN, &keystrokes,
			-1);
	macro = gaminggear_macro_new(macroset_name, macro_name, keystrokes);

	g_free(macroset_name);
	g_free(macro_name);

	return macro;
}

void gaminggear_macro_tree_store_clear(GaminggearMacroTreeStore *macro_tree_store) {
	GtkTreeIter iter;

	while (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(macro_tree_store), &iter))
		gaminggear_macro_tree_store_remove(macro_tree_store, &iter);
	gaminggear_macro_tree_store_set_modified(macro_tree_store, TRUE);
}

gchar *gaminggear_macro_tree_store_generate_unique_macro_name(GaminggearMacroTreeStore *macro_tree_store, GtkTreeIter *macroset_iter, gchar const *wanted_macro_name) {
	gchar *new_macro_name;
	guint count;
	count = 1;

	if (gaminggear_macro_tree_store_is_macro_name_unique(macro_tree_store, macroset_iter, wanted_macro_name))
		return g_strdup(wanted_macro_name);

	for (count = 1; count < G_MAXUINT8; ++count) {
		new_macro_name = g_strdup_printf("%s_%03i", wanted_macro_name, count);
		if (gaminggear_macro_tree_store_is_macro_name_unique(macro_tree_store, macroset_iter, new_macro_name))
			return new_macro_name;
		g_free(new_macro_name);
	}
	return NULL;
}

void gaminggear_macro_tree_store_rename(GaminggearMacroTreeStore *macro_tree_store, GtkTreeIter *iter, gchar const *new_name) {
	GtkTreeIter macroset_iter;

	if (gaminggear_macro_tree_store_is_macroset_with_iter(macro_tree_store, iter)) {
		if (!gaminggear_macro_tree_store_is_macroset_name_unique(macro_tree_store, new_name))
			return;
	} else {
		gtk_tree_model_iter_parent(GTK_TREE_MODEL(macro_tree_store), &macroset_iter, iter);
		if (!gaminggear_macro_tree_store_is_macro_name_unique(macro_tree_store, &macroset_iter, new_name))
			return;
	}

	gtk_tree_store_set(GTK_TREE_STORE(macro_tree_store), iter, GAMINGGEAR_MACRO_TREE_STORE_NAME_COLUMN, new_name, -1);
	gaminggear_macro_tree_store_set_modified(macro_tree_store, TRUE);
}

void gaminggear_macro_tree_store_dup_macro(GaminggearMacroTreeStore *macro_tree_store, GtkTreeIter *macro_iter) {
	GtkTreeIter macroset_iter;
	GaminggearMacroKeystrokes *keystrokes;
	gchar *macro_name;
	gchar *new_macro_name;
	GtkTreeModel *model;

	model = GTK_TREE_MODEL(macro_tree_store);

	if (!gtk_tree_model_iter_parent(model, &macroset_iter, macro_iter))
		return;

	gtk_tree_model_get(model, macro_iter,
			GAMINGGEAR_MACRO_TREE_STORE_NAME_COLUMN, &macro_name,
			GAMINGGEAR_MACRO_TREE_STORE_POINTER_COLUMN, &keystrokes,
			-1);

	new_macro_name = gaminggear_macro_tree_store_generate_unique_macro_name(macro_tree_store, &macroset_iter, macro_name);
	g_free(macro_name);

	if (!new_macro_name)
		return;

	add_macro_with_keystrokes(macro_tree_store, &macroset_iter, macro_iter, new_macro_name, keystrokes);
	g_free(new_macro_name);
	gaminggear_macro_tree_store_set_modified(macro_tree_store, TRUE);
}
