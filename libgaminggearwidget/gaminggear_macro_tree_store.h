#ifndef __GAMINGGEAR_MACRO_TREE_STORE_H__
#define __GAMINGGEAR_MACRO_TREE_STORE_H__

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

#include "gaminggear_macros.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GAMINGGEAR_MACRO_TREE_STORE_TYPE (gaminggear_macro_tree_store_get_type())
#define GAMINGGEAR_MACRO_TREE_STORE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GAMINGGEAR_MACRO_TREE_STORE_TYPE, GaminggearMacroTreeStore))
#define IS_GAMINGGEAR_MACRO_TREE_STORE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GAMINGGEAR_MACRO_TREE_STORE_TYPE))

typedef struct _GaminggearMacroTreeStore GaminggearMacroTreeStore;
typedef struct _GaminggearMacroTreeStorePrivate GaminggearMacroTreeStorePrivate;

struct _GaminggearMacroTreeStore {
	GtkTreeStore parent;
	GaminggearMacroTreeStorePrivate *priv;
};

GType gaminggear_macro_tree_store_get_type(void);

GaminggearMacroTreeStore *gaminggear_macro_tree_store_new(void);
guint gaminggear_macro_tree_store_name_column(void);
gboolean gaminggear_macro_tree_store_is_macroset_with_iter(GaminggearMacroTreeStore *store, GtkTreeIter *iter);
gboolean gaminggear_macro_tree_store_is_macroset_with_path(GtkTreePath *path);
void gaminggear_macro_tree_store_set_modified(GaminggearMacroTreeStore *macro_tree_store, gboolean state);
gboolean gaminggear_macro_tree_store_get_modified(GaminggearMacroTreeStore *macro_tree_store);
void gaminggear_macro_tree_store_add_macroset(GaminggearMacroTreeStore *macro_tree_store, GtkTreeIter *iter, gchar const *name);
void gaminggear_macro_tree_store_add_macro_with_keystrokes(GaminggearMacroTreeStore *macro_tree_store, GtkTreeIter *macroset_iter, GtkTreeIter *macro_iter, gchar const *macro_name, GaminggearMacroKeystrokes *keystrokes);
void gaminggear_macro_tree_store_add_macro(GaminggearMacroTreeStore *macro_tree_store, GtkTreeIter *macroset_iter, GtkTreeIter *macro_iter, gchar const *macro_name);

/* returns true if at least one macro was added */
gboolean gaminggear_macro_tree_store_add_macros(GaminggearMacroTreeStore *macro_tree_store, GaminggearMacros *gaminggear_macros);

GaminggearMacros *gaminggear_macro_tree_store_get_macros(GaminggearMacroTreeStore *store);
gboolean gaminggear_macro_tree_store_remove(GaminggearMacroTreeStore *store, GtkTreeIter *iter);
gchar *gaminggear_macro_tree_store_get_name(GaminggearMacroTreeStore *store, GtkTreeIter *macro_iter);
GaminggearMacroKeystrokes *gaminggear_macro_tree_store_get_keystrokes(GaminggearMacroTreeStore *store, GtkTreeIter *macro_iter);
void gaminggear_macro_tree_store_set_keystrokes(GaminggearMacroTreeStore *store, GtkTreeIter *macro_iter, GaminggearMacroKeystrokes *keystrokes);
GaminggearMacro *gaminggear_macro_tree_store_get_macro(GaminggearMacroTreeStore *store, GtkTreeIter *macro_iter);
void gaminggear_macro_tree_store_clear(GaminggearMacroTreeStore *store);
gboolean gaminggear_macro_tree_store_is_macroset_name_unique(GaminggearMacroTreeStore *store, gchar const *macroset_name);
gboolean gaminggear_macro_tree_store_is_macro_name_unique(GaminggearMacroTreeStore *store, GtkTreeIter *macroset_iter, gchar const *macro_name);
gchar *gaminggear_macro_tree_store_generate_unique_macro_name(GaminggearMacroTreeStore *store, GtkTreeIter *macroset_iter, gchar const *wanted_macro_name);
void gaminggear_macro_tree_store_rename(GaminggearMacroTreeStore *store, GtkTreeIter *iter, gchar const *new_name);
void gaminggear_macro_tree_store_dup_macro(GaminggearMacroTreeStore *store, GtkTreeIter *macro_iter);

G_END_DECLS

#endif
