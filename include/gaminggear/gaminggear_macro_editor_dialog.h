#ifndef __GAMINGGEAR_MACRO_EDITOR_DIALOG_H__
#define __GAMINGGEAR_MACRO_EDITOR_DIALOG_H__

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

/*! \file gaminggear/gaminggear_macro_editor_dialog.h
 *  \brief Macro editor dialog.
 */

#include <gaminggear/macro.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GAMINGGEAR_MACRO_EDITOR_DIALOG_TYPE (gaminggear_macro_editor_dialog_get_type())
#define GAMINGGEAR_MACRO_EDITOR_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GAMINGGEAR_MACRO_EDITOR_DIALOG_TYPE, GaminggearMacroEditorDialog))
#define IS_GAMINGGEAR_MACRO_EDITOR_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GAMINGGEAR_MACRO_EDITOR_DIALOG_TYPE))

typedef struct _GaminggearMacroEditorDialog GaminggearMacroEditorDialog;
typedef struct _GaminggearMacroEditorDialogPrivate GaminggearMacroEditorDialogPrivate;

struct _GaminggearMacroEditorDialog {
	GtkDialog dialog;
	GaminggearMacroEditorDialogPrivate *priv;
};

GType gaminggear_macro_editor_dialog_get_type(void);

/*! \brief
 *  \param parent Parent window or \c NULL.
 *  \retval widget A macro editor dialog.
 *  \since 1.0
 */
GtkWidget *gaminggear_macro_editor_dialog_new(GtkWindow *parent);

/*! \brief
 *
 *  Convenience function that creates, runs and destroys a macro editor.
 *
 *  \param parent Parent window or \c NULL.
 *  \since 1.0
 */
void gaminggear_macro_editor_dialog(GtkWindow *parent);

/*! \brief Runs an existing dialog.
 *
 *  Reloads macro data and runs dialog.
 *
 *  \param dialog A macro editor dialog.
 *  \since 1.0
 */
void gaminggear_macro_editor_dialog_run(GaminggearMacroEditorDialog *dialog);

G_END_DECLS

#endif
