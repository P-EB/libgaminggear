#ifndef __GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_H__
#define __GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_H__

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

#include "gaminggear/macro.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_TYPE (gaminggear_macro_editor_basic_list_store_get_type())
#define GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_TYPE, GaminggearMacroEditorBasicListStore))
#define IS_GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GAMINGGEAR_MACRO_EDITOR_BASIC_LIST_STORE_TYPE))

typedef struct _GaminggearMacroEditorBasicListStore GaminggearMacroEditorBasicListStore;
typedef struct _GaminggearMacroEditorBasicListStorePrivate GaminggearMacroEditorBasicListStorePrivate;

struct _GaminggearMacroEditorBasicListStore {
	GtkListStore parent;
	GaminggearMacroEditorBasicListStorePrivate *priv;
};

GType gaminggear_macro_editor_basic_list_store_get_type(void);

GaminggearMacroEditorBasicListStore *gaminggear_macro_editor_basic_list_store_new(void);

void gaminggear_macro_editor_basic_list_store_insert_single_action_key_before(GaminggearMacroEditorBasicListStore *macro_editor_basic_list_store, GtkTreeIter *sibling, guint key, guint action);

/* Only inserts period if previous entry is key.
 * Returns FALSE if nothing was inserted.
 */
gboolean gaminggear_macro_editor_basic_list_store_insert_period_before(GaminggearMacroEditorBasicListStore *macro_editor_basic_list_store, GtkTreeIter *sibling, glong rel_time);

void gaminggear_macro_editor_basic_list_store_insert_keystroke_before(GaminggearMacroEditorBasicListStore *macro_editor_basic_list_store, GtkTreeIter *sibling, guint key, guint action, glong rel_time);

gboolean gaminggear_macro_editor_basic_list_store_remove(GaminggearMacroEditorBasicListStore *macro_editor_basic_list_store, GtkTreeIter *iter);

GaminggearMacroKeystrokes *gaminggear_macro_editor_basic_list_store_get_keystrokes(GaminggearMacroEditorBasicListStore *macro_editor_basic_list_store);
void gaminggear_macro_editor_basic_list_store_set_keystrokes(GaminggearMacroEditorBasicListStore *macro_editor_basic_list_store, GaminggearMacroKeystrokes const *keystrokes);

void gaminggear_macro_editor_basic_list_store_set_value(GaminggearMacroEditorBasicListStore *macro_editor_basic_list_store, GtkTreeIter *iter, gchar const *text);

guint gaminggear_macro_editor_basic_list_store_icon_column(void);
guint gaminggear_macro_editor_basic_list_store_text_column(void);
guint gaminggear_macro_editor_basic_list_store_is_key_column(void);
guint gaminggear_macro_editor_basic_list_store_is_not_key_column(void);

void gaminggear_macro_editor_basic_list_store_set_modified(GaminggearMacroEditorBasicListStore *macro_editor_basic_list_store, gboolean state);
gboolean gaminggear_macro_editor_basic_list_store_get_modified(GaminggearMacroEditorBasicListStore *macro_editor_basic_list_store);
void gaminggear_macro_editor_basic_list_store_clear(GaminggearMacroEditorBasicListStore *macro_editor_basic_list_store);
gboolean gaminggear_macro_editor_basic_list_store_empty(GaminggearMacroEditorBasicListStore *macro_editor_basic_list_store);

G_END_DECLS

#endif
