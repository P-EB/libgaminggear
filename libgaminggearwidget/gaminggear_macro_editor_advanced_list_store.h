#ifndef __GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_H__
#define __GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_H__

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

#define GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_TYPE (gaminggear_macro_editor_advanced_list_store_get_type())
#define GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_TYPE, GaminggearMacroEditorAdvancedListStore))
#define IS_GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GAMINGGEAR_MACRO_EDITOR_ADVANCED_LIST_STORE_TYPE))

typedef struct _GaminggearMacroEditorAdvancedListStore GaminggearMacroEditorAdvancedListStore;
typedef struct _GaminggearMacroEditorAdvancedListStorePrivate GaminggearMacroEditorAdvancedListStorePrivate;

struct _GaminggearMacroEditorAdvancedListStore {
	GtkListStore parent;
	GaminggearMacroEditorAdvancedListStorePrivate *priv;
};

GType gaminggear_macro_editor_advanced_list_store_get_type(void);

GaminggearMacroEditorAdvancedListStore *gaminggear_macro_editor_advanced_list_store_new(void);

void gaminggear_macro_editor_advanced_list_store_set_modified(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store, gboolean state);
gboolean gaminggear_macro_editor_advanced_list_store_get_modified(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store);

GtkWidget *gaminggear_macro_editor_advanced_list_store_get_label_widget(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store, GtkTreeIter *iter);
GtkWidget *gaminggear_macro_editor_advanced_list_store_get_lower_widget(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store, GtkTreeIter *iter);
GtkWidget *gaminggear_macro_editor_advanced_list_store_get_upper_widget(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store, GtkTreeIter *iter);
GtkWidget *gaminggear_macro_editor_advanced_list_store_get_scale_widget(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store, GtkTreeIter *iter);
gdouble gaminggear_macro_editor_advanced_list_store_get_all_max(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store);
void gaminggear_macro_editor_advanced_list_store_set_all_max(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store, gdouble value);

GaminggearMacroKeystrokes *gaminggear_macro_editor_advanced_list_store_get_keystrokes(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store);
void gaminggear_macro_editor_advanced_list_store_set_keystrokes(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store, GaminggearMacroKeystrokes const *macro_keystrokes);
void gaminggear_macro_editor_advanced_list_store_add_keystroke(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store, guint key, guint action, glong rel_time);

void gaminggear_macro_editor_advanced_list_store_clear(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store);
gboolean gaminggear_macro_editor_advanced_list_store_empty(GaminggearMacroEditorAdvancedListStore *macro_editor_advanced_list_store);

G_END_DECLS

#endif
