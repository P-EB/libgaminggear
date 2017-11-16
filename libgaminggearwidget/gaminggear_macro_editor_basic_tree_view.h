#ifndef __GAMINGGEAR_MACRO_EDITOR_BASIC_TREE_VIEW_H__
#define __GAMINGGEAR_MACRO_EDITOR_BASIC_TREE_VIEW_H__

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
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GAMINGGEAR_MACRO_EDITOR_BASIC_TREE_VIEW_TYPE (gaminggear_macro_editor_basic_tree_view_get_type())
#define GAMINGGEAR_MACRO_EDITOR_BASIC_TREE_VIEW(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GAMINGGEAR_MACRO_EDITOR_BASIC_TREE_VIEW_TYPE, GaminggearMacroEditorBasicTreeView))
#define IS_GAMINGGEAR_MACRO_EDITOR_BASIC_TREE_VIEW(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GAMINGGEAR_MACRO_EDITOR_BASIC_TREE_VIEW_TYPE))

typedef struct _GaminggearMacroEditorBasicTreeView GaminggearMacroEditorBasicTreeView;
typedef struct _GaminggearMacroEditorBasicTreeViewPrivate GaminggearMacroEditorBasicTreeViewPrivate;

struct _GaminggearMacroEditorBasicTreeView {
	GtkTreeView parent;
	GaminggearMacroEditorBasicTreeViewPrivate *priv;
};

GType gaminggear_macro_editor_basic_tree_view_get_type(void);
GtkWidget *gaminggear_macro_editor_basic_tree_view_new(GaminggearMacroEditorBasicListStore *list_store);

void gaminggear_macro_editor_basic_tree_view_add_keystroke(GaminggearMacroEditorBasicTreeView *tree_view, guint key, guint action, glong rel_time);

G_END_DECLS

#endif
