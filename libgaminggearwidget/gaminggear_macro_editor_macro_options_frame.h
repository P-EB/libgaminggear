#ifndef __GAMINGGEAR_MACRO_EDITOR_MACRO_OPTIONS_FRAME_H__
#define __GAMINGGEAR_MACRO_EDITOR_MACRO_OPTIONS_FRAME_H__

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

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GAMINGGEAR_MACRO_EDITOR_MACRO_OPTIONS_FRAME_TYPE (gaminggear_macro_editor_macro_options_frame_get_type())
#define GAMINGGEAR_MACRO_EDITOR_MACRO_OPTIONS_FRAME(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GAMINGGEAR_MACRO_EDITOR_MACRO_OPTIONS_FRAME_TYPE, GaminggearMacroEditorMacroOptionsFrame))
#define IS_GAMINGGEAR_MACRO_EDITOR_MACRO_OPTIONS_FRAME(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GAMINGGEAR_MACRO_EDITOR_MACRO_OPTIONS_FRAME_TYPE))

typedef struct _GaminggearMacroEditorMacroOptionsFrame GaminggearMacroEditorMacroOptionsFrame;
typedef struct _GaminggearMacroEditorMacroOptionsFramePrivate GaminggearMacroEditorMacroOptionsFramePrivate;

struct _GaminggearMacroEditorMacroOptionsFrame {
	GtkFrame parent;
	GaminggearMacroEditorMacroOptionsFramePrivate *priv;
};

GType gaminggear_macro_editor_macro_options_frame_get_type(void);

GtkWidget *gaminggear_macro_editor_macro_options_frame_new(void);

gint gaminggear_macro_editor_macro_options_frame_get_loop(GaminggearMacroEditorMacroOptionsFrame *macro_options_frame);
void gaminggear_macro_editor_macro_options_frame_set_loop(GaminggearMacroEditorMacroOptionsFrame *macro_options_frame, gint value);

G_END_DECLS

#endif
