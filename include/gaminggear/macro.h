#ifndef __GAMINGGEAR_MACRO_H__
#define __GAMINGGEAR_MACRO_H__

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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libgaminggear. If not, see <http://www.gnu.org/licenses/>.
 */

/*! \file gaminggear/macro.h
 *  \brief Macros.
 */

#include <glib.h>

G_BEGIN_DECLS

typedef struct _GaminggearMacro GaminggearMacro;
typedef struct _GaminggearMacroKeystroke GaminggearMacroKeystroke;
typedef struct _GaminggearMacroKeystrokes GaminggearMacroKeystrokes;

enum {
	GAMINGGEAR_MACRO_KEYSTROKES_NUM = 512,
	GAMINGGEAR_MACRO_KEYSTROKE_PERIOD_MAX = G_MAXUINT16,
};

struct _GaminggearMacroKeystroke {
	guint8 key;
	guint8 action;
	guint16 period; /*!< Stored little endian. */
} __attribute__ ((packed));

typedef enum {
	GAMINGGEAR_MACRO_KEYSTROKE_KEY_BUTTON_LEFT = 0xf0,
	GAMINGGEAR_MACRO_KEYSTROKE_KEY_BUTTON_RIGHT = 0xf1,
	GAMINGGEAR_MACRO_KEYSTROKE_KEY_BUTTON_MIDDLE = 0xf2,
} GaminggearMacroKeystrokeKey;

typedef enum {
	GAMINGGEAR_MACRO_KEYSTROKE_ACTION_PRESS = 0,
	GAMINGGEAR_MACRO_KEYSTROKE_ACTION_RELEASE = 1
} GaminggearMacroKeystrokeAction;

/*! \brief Get period from GaminggearMacroKeystroke.
 *  \param keystroke A keystroke.
 *  \retval period
 *  \since 1.0
 */
static inline guint16 gaminggear_macro_keystroke_get_period(GaminggearMacroKeystroke const *keystroke) {
	return GUINT16_FROM_LE(keystroke->period);
}

/*! \brief Set period of GaminggearMacroKeystroke.
 *  \param keystroke A keystroke.
 *  \param new_value
 *  \since 1.0
 */
static inline void gaminggear_macro_keystroke_set_period(GaminggearMacroKeystroke *keystroke, guint16 new_value) {
	keystroke->period = GUINT16_TO_LE(new_value);
}

struct _GaminggearMacroKeystrokes {
	guint16 count; /*!< Stored little endian. */
	GaminggearMacroKeystroke keystrokes[GAMINGGEAR_MACRO_KEYSTROKES_NUM];
	guint8 loop;
} __attribute__ ((packed));

/*! \brief Get count from GaminggearMacroKeystrokes.
 *  \param keystrokes Keystrokes.
 *  \retval count
 *  \since 1.0
 */
static inline guint16 gaminggear_macro_keystrokes_get_count(GaminggearMacroKeystrokes const *keystrokes) {
	return GUINT16_FROM_LE(keystrokes->count);
}

/*! \brief Set count of GaminggearMacroKeystrokes.
 *  \param keystrokes Keystrokes.
 *  \param new_value
 *  \since 1.0
 */
static inline void gaminggear_macro_keystrokes_set_count(GaminggearMacroKeystrokes *keystrokes, guint16 new_value) {
	keystrokes->count = GUINT16_TO_LE(new_value);
}

/*! \brief Reset keystrokes.
 *  \param keystrokes Keystrokes.
 *  \since 1.0
 */
void gaminggear_macro_keystrokes_init(GaminggearMacroKeystrokes *keystrokes);

/*! \brief
 *  \param key_file
 *  \param macroset_name
 *  \param macro_name
 *  \param error
 *  \retval keystrokes
 *  \since 1.0
 */
GaminggearMacroKeystrokes *gaminggear_key_file_get_macro_keystrokes(GKeyFile *key_file,
		gchar const *macroset_name, gchar const *macro_name,
		GError **error);

/*! \brief Check if macro contains mouse events.
 *  \param keystrokes Keystrokes.
 *  \retval boolean \c TRUE if macro contains mouse events.
 *  \since 1.0
 */
gboolean gaminggear_macro_keystrokes_have_mouse_keystroke(GaminggearMacroKeystrokes const *keystrokes);

/*! \brief
 *  \retval keystrokes Keystrokes that have to be freed with gaminggear_macro_keystrokes_free().
 *  \since 1.0
 */
GaminggearMacroKeystrokes *gaminggear_macro_keystrokes_new(void);

/*! \brief
 *  \param keystrokes Keystrokes.
 *  \since 1.0
 */
void gaminggear_macro_keystrokes_free(GaminggearMacroKeystrokes *keystrokes);

/*! \brief
 *  \param from
 *  \param to
 *  \since 1.0
 */
void gaminggear_macro_keystrokes_copy(GaminggearMacroKeystrokes const *from, GaminggearMacroKeystrokes *to);

/*! \brief
 *  \param old
 *  \retval keystrokes Keystrokes that have to be freed with gaminggear_macro_keystrokes_free().
 *  \since 1.0
 */
GaminggearMacroKeystrokes *gaminggear_macro_keystrokes_dup(GaminggearMacroKeystrokes const *old);

/*! \brief
 *  \param keystrokes Keystrokes.
 *  \param keystroke Keystroke.
 *  \retval boolean If successful \c TRUE is returned.
 *          \c FALSE is returned if number of keystrokes is exceeded and keystroke was not added.
 *  \since 1.0
 */
gboolean gaminggear_macro_keystrokes_add(GaminggearMacroKeystrokes *keystrokes, GaminggearMacroKeystroke const *keystroke);

struct _GaminggearMacro {
	gchar *macroset, *macro;
	GaminggearMacroKeystrokes keystrokes;
};

/*! \brief Sets macroset name.
 *  \param macro
 *  \param new_name
 *  \since 1.0
 */
void gaminggear_macro_set_macroset_name(GaminggearMacro *macro, gchar const *new_name);

/*! \brief Sets macro name.
 *  \param macro
 *  \param new_name
 *  \since 1.0
 */
void gaminggear_macro_set_macro_name(GaminggearMacro *macro, gchar const *new_name);

/*! \brief Get complete name in format macroset/macro.
 *  \param macro
 *  \retval string That has to be freed with \c g_free().
 *  \since 1.0
 */
gchar *gaminggear_macro_get_name_joined(GaminggearMacro const *macro);

/*! \brief Get keystrokes of a macro.
 *  \param macro
 *  \retval keystrokes The returned struct is part of macro and should not be freed alone.
 *  \since 1.0
 */
GaminggearMacroKeystrokes *gaminggear_macro_get_keystrokes(GaminggearMacro *macro);

/*! \brief Add keystroke to macro.
 *  \param macro
 *  \param keystroke
 *  \retval boolean If successful \c TRUE is returned.
 *          \c FALSE is returned if number of keystrokes is exceeded and keystroke was not added.
 *  \since 1.0
 */
gboolean gaminggear_macro_add_keystroke(GaminggearMacro *macro, GaminggearMacroKeystroke const *keystroke);

/*! \brief Creates new macro.
 *
 *  Keystrokes can be \c NULL. In this case an empty macro is created.
 *
 *  \param macroset_name
 *  \param macro_name
 *  \param keystrokes
 *  \retval macro GaminggearMacro that has to be freed with \c gaminggear_macro_free().
 *  \since 1.0
 */
GaminggearMacro *gaminggear_macro_new(gchar const *macroset_name, gchar const *macro_name, GaminggearMacroKeystrokes const *keystrokes);

/*! \brief Duplicates a GaminggearMacro.
 *  \param gaminggear_macro
 *  \retval macro GaminggearMacro that has to be freed with \c gaminggear_macro_free().
 *  \since 1.0
*/
GaminggearMacro *gaminggear_macro_dup(GaminggearMacro const *gaminggear_macro);

/*! \brief Test if GaminggearMacro contains mouse events.
 *  \param gaminggear_macro
 *  \retval boolean \c TRUE if gaminggear_macro has mouse keystroke, \c FALSE else.
 *  \since 1.0
 */
gboolean gaminggear_macro_has_mouse_keystroke(GaminggearMacro const *gaminggear_macro);

/*! \brief Frees macro.
 *
 *  Frees the memory allocated for macro.
 *  \param macro A GaminggearMacro.
 *  \since 1.0
 */
void gaminggear_macro_free(GaminggearMacro *macro);

/*! \brief Stores macro in macros database.
 *
 *  Used to store macros generated by macro live recording.
 *
 *  \param gaminggear_macro The GaminggearMacro to be stored.
 *  \param error Return location for error or \c NULL.
 *  \retval boolean \c TRUE on success, \c FALSE on error with error set.
 *  \since 1.0
 */
gboolean gaminggear_macros_store_macro(GaminggearMacro const *gaminggear_macro, GError **error);

G_END_DECLS

#endif
