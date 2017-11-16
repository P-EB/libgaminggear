#ifndef __GAMINGGEAR_MACRO_EDITOR_KEY_SEQUENCE_FRAME_H__
#define __GAMINGGEAR_MACRO_EDITOR_KEY_SEQUENCE_FRAME_H__

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

#define GAMINGGEAR_MACRO_EDITOR_KEY_SEQUENCE_FRAME_TYPE (gaminggear_macro_editor_key_sequence_frame_get_type())
#define GAMINGGEAR_MACRO_EDITOR_KEY_SEQUENCE_FRAME(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GAMINGGEAR_MACRO_EDITOR_KEY_SEQUENCE_FRAME_TYPE, GaminggearMacroEditorKeySequenceFrame))
#define IS_GAMINGGEAR_MACRO_EDITOR_KEY_SEQUENCE_FRAME(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GAMINGGEAR_MACRO_EDITOR_KEY_SEQUENCE_FRAME_TYPE))

typedef struct _GaminggearMacroEditorKeySequenceFrame GaminggearMacroEditorKeySequenceFrame;
typedef struct _GaminggearMacroEditorKeySequenceFramePrivate GaminggearMacroEditorKeySequenceFramePrivate;

struct _GaminggearMacroEditorKeySequenceFrame {
	GtkFrame parent;
	GaminggearMacroEditorKeySequenceFramePrivate *priv;
};

GType gaminggear_macro_editor_key_sequence_frame_get_type(void);

GtkWidget *gaminggear_macro_editor_key_sequence_frame_new(void);
GtkWidget *gaminggear_macro_editor_key_sequence_get_action_area(GaminggearMacroEditorKeySequenceFrame *key_sequence_frame);

void gaminggear_macro_editor_key_sequence_frame_set_keystrokes(GaminggearMacroEditorKeySequenceFrame *key_sequence_frame, GaminggearMacroKeystrokes *keystrokes);
GaminggearMacroKeystrokes *gaminggear_macro_editor_key_sequence_frame_get_keystrokes(GaminggearMacroEditorKeySequenceFrame *key_sequence_frame);
void gaminggear_macro_editor_key_sequence_frame_add_keystroke(GaminggearMacroEditorKeySequenceFrame *key_sequence_frame, guint key, guint action, glong rel_time);
void gaminggear_macro_editor_key_sequence_frame_clear(GaminggearMacroEditorKeySequenceFrame *key_sequence_frame);
gboolean gaminggear_macro_editor_key_sequence_frame_empty(GaminggearMacroEditorKeySequenceFrame *key_sequence_frame);
void gaminggear_macro_editor_key_sequence_record_mode(GaminggearMacroEditorKeySequenceFrame *key_sequence_frame, gboolean on);

G_END_DECLS

#endif
