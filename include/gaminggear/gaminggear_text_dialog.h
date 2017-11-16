#ifndef __GAMINGGEAR_TEXT_DIALOG_H__
#define __GAMINGGEAR_TEXT_DIALOG_H__

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

/*! \file gaminggear/gaminggear_text_dialog.h
 *  \brief Dialog for entering single line text.
 *
 *  Pressing enter in text field behaves like pressing OK button.
 */

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GAMINGGEAR_TEXT_DIALOG_TYPE (gaminggear_text_dialog_get_type())
#define GAMINGGEAR_TEXT_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GAMINGGEAR_TEXT_DIALOG_TYPE, GaminggearTextDialog))
#define IS_GAMINGGEAR_TEXT_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GAMINGGEAR_TEXT_DIALOG_TYPE))

typedef struct _GaminggearTextDialog GaminggearTextDialog;
typedef struct _GaminggearTextDialogPrivate GaminggearTextDialogPrivate;

struct _GaminggearTextDialog {
	GtkDialog dialog;
	GaminggearTextDialogPrivate *priv;
};

GType gaminggear_text_dialog_get_type(void);

/*! \brief Get user text.
 *
 *  Convenience function that creates, runs and destroys a text dialog.
 *
 *  \param parent Parent window or \c NULL.
 *  \param title Title of dialog.
 *  \param caption Caption of dialog.
 *  \param text Text shown in entry field or \c NULL for empty field.
 *  \retval text User entered text that has to be freed with \c g_free() or \c NULL.
 *  \since 1.0
 */
gchar *gaminggear_text_dialog(GtkWindow *parent, gchar const *title, gchar const *caption, gchar const *text);

/*! \brief Run a text dialog.
 *  \param text_dialog A text dialog.
 *  \retval text User entered text that has to be freed with \c g_free() or \c NULL.
 *  \since 1.0
 */
gchar *gaminggear_text_dialog_run(GaminggearTextDialog *text_dialog);

/*! \brief Creates new text dialog.
 *  \param parent Parent window or \c NULL.
 *  \param title Title of dialog.
 *  \param caption Caption of dialog.
 *  \retval widget The text dialog.
 *  \since 1.0
 */
GtkWidget *gaminggear_text_dialog_new(GtkWindow *parent, gchar const *title, gchar const *caption);

/*! \brief Gets user entered text.
 *  \param text_dialog A text dialog.
 *  \retval text User entered text that has to be freed with \c g_free() or \c NULL.
 *  \since 1.0
 */
gchar *gaminggear_text_dialog_get_text(GaminggearTextDialog *text_dialog);

/*! \brief Set text of text dialog.
 *
 *  Presets the text shown in the entry field.
 *
 *  \param text_dialog A text dialog.
 *  \param text The new text shown in entry field.
 *  \since 1.0
 */
void gaminggear_text_dialog_set_text(GaminggearTextDialog *text_dialog, gchar const *text);

/*! \brief Set title of text dialog.
 *  \param text_dialog A text dialog.
 *  \param title The new title text.
 *  \since 1.0
 */
void gaminggear_text_dialog_set_title(GaminggearTextDialog *text_dialog, gchar const *title);

/*! \brief Set caption of text dialog.
 *  \param text_dialog A text dialog.
 *  \param caption The new caption text.
 *  \since 1.0
 */
void gaminggear_text_dialog_set_caption(GaminggearTextDialog *text_dialog, gchar const *caption);

G_END_DECLS

#endif
