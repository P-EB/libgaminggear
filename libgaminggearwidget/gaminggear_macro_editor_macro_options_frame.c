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

#include "gaminggear_macro_editor_macro_options_frame.h"
#include "gaminggear_macro_editor_advanced_list_store.h"
#include "i18n-lib.h"

#define GAMINGGEAR_MACRO_EDITOR_MACRO_OPTIONS_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GAMINGGEAR_MACRO_EDITOR_MACRO_OPTIONS_FRAME_TYPE, GaminggearMacroEditorMacroOptionsFrameClass))
#define IS_GAMINGGEAR_MACRO_EDITOR_MACRO_OPTIONS_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GAMINGGEAR_MACRO_EDITOR_MACRO_OPTIONS_FRAME_TYPE))
#define GAMINGGEAR_MACRO_EDITOR_MACRO_OPTIONS_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), GAMINGGEAR_MACRO_EDITOR_MACRO_OPTIONS_FRAME_TYPE, GaminggearMacroEditorMacroOptionsFramePrivate))

typedef struct _GaminggearMacroEditorMacroOptionsFrameClass GaminggearMacroEditorMacroOptionsFrameClass;
typedef struct _AbsoluteKeystroke AbsoluteKeystroke;

struct _GaminggearMacroEditorMacroOptionsFrameClass {
	GtkFrameClass parent_class;
	void (*modified)(GaminggearMacroEditorMacroOptionsFrame *macro_options_frame);
};

struct _GaminggearMacroEditorMacroOptionsFramePrivate {
	GtkSpinButton *loop_button;
};

G_DEFINE_TYPE(GaminggearMacroEditorMacroOptionsFrame, gaminggear_macro_editor_macro_options_frame, GTK_TYPE_FRAME);

enum {
	MODIFIED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

GtkWidget *gaminggear_macro_editor_macro_options_frame_new(void) {
	GaminggearMacroEditorMacroOptionsFrame *macro_options_frame;

	macro_options_frame = g_object_new(GAMINGGEAR_MACRO_EDITOR_MACRO_OPTIONS_FRAME_TYPE,
			"label", _("Macro options"),
			NULL);

	return GTK_WIDGET(macro_options_frame);
}

static void modified(GaminggearMacroEditorMacroOptionsFrame *macro_options_frame) {
	g_signal_emit((gpointer)macro_options_frame, signals[MODIFIED], 0);
}

static void value_changed_cb(GtkSpinButton *spinbutton, gpointer user_data) {
	GaminggearMacroEditorMacroOptionsFrame *macro_options_frame = GAMINGGEAR_MACRO_EDITOR_MACRO_OPTIONS_FRAME(user_data);
	modified(macro_options_frame);
}

static void gaminggear_macro_editor_macro_options_frame_init(GaminggearMacroEditorMacroOptionsFrame *macro_options_frame) {
	GaminggearMacroEditorMacroOptionsFramePrivate *priv = GAMINGGEAR_MACRO_EDITOR_MACRO_OPTIONS_FRAME_GET_PRIVATE(macro_options_frame);
	GtkWidget *vbox, *hbox, *loop_label;

	macro_options_frame->priv = priv;

	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(macro_options_frame), vbox);

	hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

	loop_label = gtk_label_new(_("Loop"));
	gtk_box_pack_start(GTK_BOX(hbox), loop_label, FALSE, FALSE, 0);

	priv->loop_button = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(1, 255, 1));
	g_object_ref_sink(priv->loop_button);
	gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(priv->loop_button), TRUE, TRUE, 0);
	g_signal_connect(G_OBJECT(priv->loop_button), "value-changed", G_CALLBACK(value_changed_cb), macro_options_frame);

	gtk_widget_show_all(GTK_WIDGET(vbox));
}

static void gaminggear_macro_editor_macro_options_frame_finalize(GObject *object) {
	GaminggearMacroEditorMacroOptionsFramePrivate *priv = GAMINGGEAR_MACRO_EDITOR_MACRO_OPTIONS_FRAME(object)->priv;
	g_object_unref(priv->loop_button);
	G_OBJECT_CLASS(gaminggear_macro_editor_macro_options_frame_parent_class)->finalize(object);
}

static void gaminggear_macro_editor_macro_options_frame_class_init(GaminggearMacroEditorMacroOptionsFrameClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->finalize = gaminggear_macro_editor_macro_options_frame_finalize;

	g_type_class_add_private(klass, sizeof(GaminggearMacroEditorMacroOptionsFramePrivate));

	signals[MODIFIED] = g_signal_new("modified",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			G_STRUCT_OFFSET(GaminggearMacroEditorMacroOptionsFrameClass, modified),
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}

gint gaminggear_macro_editor_macro_options_frame_get_loop(GaminggearMacroEditorMacroOptionsFrame *macro_options_frame) {
	return gtk_spin_button_get_value_as_int(macro_options_frame->priv->loop_button);
}

void gaminggear_macro_editor_macro_options_frame_set_loop(GaminggearMacroEditorMacroOptionsFrame *macro_options_frame, gint value) {
	gtk_spin_button_set_value(macro_options_frame->priv->loop_button, value);
}
