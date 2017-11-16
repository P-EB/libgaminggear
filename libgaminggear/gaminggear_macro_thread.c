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

#include "gaminggear_macro_thread.h"
#include "gaminggear/key_translations.h"
#include "gaminggear_helper.h"
#include "gaminggear/threads.h"
#include "gaminggear/input_events.h"
#include "i18n-lib.h"

#define GAMINGGEAR_MACRO_THREAD_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GAMINGGEAR_MACRO_THREAD_TYPE, GaminggearMacroThreadClass))
#define IS_GAMINGGEAR_MACRO_THREAD_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GAMINGGEAR_MACRO_THREAD_TYPE))
#define GAMINGGEAR_MACRO_THREAD_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), GAMINGGEAR_MACRO_THREAD_TYPE, GaminggearMacroThreadPrivate))

typedef struct _GaminggearMacroThreadClass GaminggearMacroThreadClass;

struct _GaminggearMacroThreadClass {
	GObjectClass parent_class;
};

struct _GaminggearMacroThreadPrivate {
	GThread *thread;

	GaminggearMutex mutex;
	GaminggearCond condition;
	gboolean cancelled;
	gboolean paused;

	gint running;

	GaminggearMacro *macro;
};

G_DEFINE_TYPE(GaminggearMacroThread, gaminggear_macro_thread, G_TYPE_OBJECT);

void gaminggear_macro_thread_set_paused(GaminggearMacroThread *gaminggear_macro_thread, gboolean value) {
	GaminggearMacroThreadPrivate *priv = gaminggear_macro_thread->priv;
	gaminggear_mutex_lock(&priv->mutex);
	priv->paused = value;
	if (!value)
		gaminggear_cond_signal(&priv->condition);
	gaminggear_mutex_unlock(&priv->mutex);
}

void gaminggear_macro_thread_toggle_paused(GaminggearMacroThread *gaminggear_macro_thread) {
	GaminggearMacroThreadPrivate *priv = gaminggear_macro_thread->priv;
	gaminggear_mutex_lock(&priv->mutex);
	priv->paused = priv->paused ? FALSE : TRUE;
	if (!priv->paused)
		gaminggear_cond_signal(&priv->condition);
	gaminggear_mutex_unlock(&priv->mutex);
}

gboolean gaminggear_macro_thread_get_paused(GaminggearMacroThread *gaminggear_macro_thread) {
	GaminggearMacroThreadPrivate *priv = gaminggear_macro_thread->priv;
	gboolean retval;
	gaminggear_mutex_lock(&priv->mutex);
	retval = priv->paused;
	gaminggear_mutex_unlock(&priv->mutex);
	return retval;
}

static void gaminggear_macro_thread_set_running(GaminggearMacroThread *gaminggear_macro_thread, gboolean value) {
	g_atomic_int_set(&gaminggear_macro_thread->priv->running, value);
}

gboolean gaminggear_macro_thread_get_running(GaminggearMacroThread *gaminggear_macro_thread) {
	return g_atomic_int_get(&gaminggear_macro_thread->priv->running);
}

void gaminggear_macro_thread_set_cancelled(GaminggearMacroThread *gaminggear_macro_thread) {
	GaminggearMacroThreadPrivate *priv = gaminggear_macro_thread->priv;
	gaminggear_mutex_lock(&priv->mutex);
	priv->cancelled = TRUE;
	gaminggear_cond_signal(&priv->condition);
	gaminggear_mutex_unlock(&priv->mutex);
}

gboolean gaminggear_macro_thread_get_cancelled(GaminggearMacroThread *gaminggear_macro_thread) {
	GaminggearMacroThreadPrivate *priv = gaminggear_macro_thread->priv;
	gboolean retval;
	gaminggear_mutex_lock(&priv->mutex);
	retval = priv->cancelled;
	gaminggear_mutex_unlock(&priv->mutex);
	return retval;
}

static guint8 gaminggear_keystroke_action_to_event_action(guint8 action) {
	return (action == GAMINGGEAR_MACRO_KEYSTROKE_ACTION_PRESS) ? GAMINGGEAR_INPUT_EVENT_VALUE_PRESS : GAMINGGEAR_INPUT_EVENT_VALUE_RELEASE;
}

/* IDEA invent save macro option
 * keep track of pressed buttons/keys and release on cancel
 */
static gpointer thread(gpointer user_data) {
	GaminggearMacroThread *macro_thread = GAMINGGEAR_MACRO_THREAD(user_data);
	GaminggearMacroThreadPrivate *priv = macro_thread->priv;
	guint key_count, key;
	guint loop_count, loop;
	GaminggearMacroKeystroke const *keystroke;

	g_debug(_("Playing macro %s/%s"), priv->macro->macroset, priv->macro->macro);

	key_count = gaminggear_macro_keystrokes_get_count(&priv->macro->keystrokes);
	loop_count = priv->macro->keystrokes.loop;
	
	for (loop = 0; loop < loop_count; ++loop) {
		for (key = 0; key < key_count; ++key) {
			
			gaminggear_mutex_lock(&priv->mutex);

			if (priv->paused && !priv->cancelled)
				g_debug(_("Pausing macro %s/%s"), priv->macro->macroset, priv->macro->macro);
			
			while (priv->paused && !priv->cancelled) {
				gaminggear_cond_wait(&priv->condition, &priv->mutex);
				if (!priv->paused && !priv->cancelled)
					g_debug(_("Continuing macro %s/%s"), priv->macro->macroset, priv->macro->macro);
			}
			
			if (priv->cancelled) {
				g_debug(_("Cancelling macro %s/%s"), priv->macro->macroset, priv->macro->macro);
				gaminggear_mutex_unlock(&priv->mutex);
				goto exit;
			}

			gaminggear_mutex_unlock(&priv->mutex);

			keystroke = &priv->macro->keystrokes.keystrokes[key];
			if (gaminggear_hid_is_mouse_button(keystroke->key))
				gaminggear_input_event_write_button(keystroke->key, gaminggear_keystroke_action_to_event_action(keystroke->action));
			else
				gaminggear_input_event_write_keyboard(keystroke->key, gaminggear_keystroke_action_to_event_action(keystroke->action));
			
			/* Skip wait of last key in last loop to improve cancel/restart behaviour. */
			if (key != key_count - 1 || loop != loop_count - 1)
				g_usleep(gaminggear_macro_keystroke_get_period(keystroke) * G_GAMINGGEAR_USEC_PER_MSEC);
		}
	}

	g_debug(_("Finished macro %s/%s"), priv->macro->macroset, priv->macro->macro);
exit:
	gaminggear_macro_thread_set_running(macro_thread, FALSE);
	return NULL;
}

GaminggearMacroThread *gaminggear_macro_thread_new(GaminggearMacro const *macro) {
	GObject *object;
	GaminggearMacroThread *macro_thread;
	GaminggearMacroThreadPrivate *priv;
	GError *error = NULL;
	
	object = g_object_new(GAMINGGEAR_MACRO_THREAD_TYPE, NULL);
	macro_thread = GAMINGGEAR_MACRO_THREAD(object);
	priv = macro_thread->priv;
	
	priv->macro = gaminggear_macro_dup(macro);
	priv->cancelled = FALSE;
	priv->paused = FALSE;
	gaminggear_macro_thread_set_running(macro_thread, TRUE);

	priv->thread = gaminggear_thread_try_new(thread, macro_thread, &error);

	if (error) {
		g_warning(_("Could not create macro thread: %s"), error->message);
		g_clear_error(&error);
	}
	
	return macro_thread;
}

static void gaminggear_macro_thread_init(GaminggearMacroThread *macro_thread_data) {
	GaminggearMacroThreadPrivate *priv = GAMINGGEAR_MACRO_THREAD_GET_PRIVATE(macro_thread_data);
	macro_thread_data->priv = priv;

	gaminggear_mutex_init(&priv->mutex);
	gaminggear_cond_init(&priv->condition);
}

static void gaminggear_macro_thread_finalize(GObject *object) {
	GaminggearMacroThreadPrivate *priv = GAMINGGEAR_MACRO_THREAD(object)->priv;
	
	if (priv->thread)
		g_thread_join(priv->thread);

	gaminggear_macro_free(priv->macro);

	gaminggear_mutex_clear(&priv->mutex);
	gaminggear_cond_clear(&priv->condition);
	
	G_OBJECT_CLASS(gaminggear_macro_thread_parent_class)->finalize(object);
}

static void gaminggear_macro_thread_class_init(GaminggearMacroThreadClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = (GObjectClass*)klass;

	gobject_class->finalize = gaminggear_macro_thread_finalize;

	g_type_class_add_private(klass, sizeof(GaminggearMacroThreadPrivate));
}
