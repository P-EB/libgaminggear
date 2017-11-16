#ifndef __GAMINGGEAR_MACRO_DIALOG_H__
#define __GAMINGGEAR_MACRO_DIALOG_H__

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

/*! \file gaminggear/gaminggear_macro_dialog.h
 *  \brief Macro selection dialog.
 */

#include <gaminggear/macro.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GAMINGGEAR_MACRO_DIALOG_TYPE (gaminggear_macro_dialog_get_type())
#define GAMINGGEAR_MACRO_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GAMINGGEAR_MACRO_DIALOG_TYPE, GaminggearMacroDialog))
#define IS_GAMINGGEAR_MACRO_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GAMINGGEAR_MACRO_DIALOG_TYPE))

typedef struct _GaminggearMacroDialog GaminggearMacroDialog;
typedef struct _GaminggearMacroDialogPrivate GaminggearMacroDialogPrivate;

struct _GaminggearMacroDialog {
	GtkDialog dialog;
	GaminggearMacroDialogPrivate *priv;
};

/*! \brief Let user select macro.
 *
 *  Convenience function that creates, runs and destroys a macro dialog.
 *
 *  \param parent Parent window or \c NULL.
 *  \retval macro Selected macro or \c NULL.
 *  \since 1.0
 */
GaminggearMacro *gaminggear_macro_dialog(GtkWindow *parent);

/*! \brief Run a macro dialog.
 *
 *  Reloads macro data and runs dialog.
 *
 *  \param macro_dialog The macro dialog.
 *  \retval macro Selected macro or \c NULL.
 *  \since 1.0
 */
GaminggearMacro *gaminggear_macro_dialog_run(GaminggearMacroDialog *macro_dialog);

/*! \brief Creates new macro dialog.
 *  \param parent Parent window or \c NULL.
 *  \retval widget The macro dialog.
 *  \since 1.0
 */
GtkWidget *gaminggear_macro_dialog_new(GtkWindow *parent);

G_END_DECLS

#endif
