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

#include "gaminggear_macro_editor_record_options_frame.h"
#include "gaminggear_macro_editor_advanced_list_store.h"
#include "gaminggear_helper.h"
#include "i18n-lib.h"

#define GAMINGGEAR_MACRO_EDITOR_RECORD_OPTIONS_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GAMINGGEAR_MACRO_EDITOR_RECORD_OPTIONS_FRAME_TYPE, GaminggearMacroEditorRecordOptionsFrameClass))
#define IS_GAMINGGEAR_MACRO_EDITOR_RECORD_OPTIONS_FRAME_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GAMINGGEAR_MACRO_EDITOR_RECORD_OPTIONS_FRAME_TYPE))
#define GAMINGGEAR_MACRO_EDITOR_RECORD_OPTIONS_FRAME_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), GAMINGGEAR_MACRO_EDITOR_RECORD_OPTIONS_FRAME_TYPE, GaminggearMacroEditorRecordOptionsFramePrivate))

typedef struct _GaminggearMacroEditorRecordOptionsFrameClass GaminggearMacroEditorRecordOptionsFrameClass;
typedef struct _AbsoluteKeystroke AbsoluteKeystroke;

struct _GaminggearMacroEditorRecordOptionsFrameClass {
	GtkFrameClass parent_class;
};

struct _GaminggearMacroEditorRecordOptionsFramePrivate {
	GtkToggleButton *record_button;
	GtkButton *paste_button;
	GtkSpinButton *delay_value;
	GtkRadioButton *delay_as_recorded;
	GtkRadioButton *delay_fixed;
};

G_DEFINE_TYPE(GaminggearMacroEditorRecordOptionsFrame, gaminggear_macro_editor_record_options_frame, GTK_TYPE_FRAME);

enum {
	RECORD_EVENT,
	PASTE,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

GtkWidget *gaminggear_macro_editor_record_options_frame_new(void) {
	GaminggearMacroEditorRecordOptionsFrame *record_options_frame;

	record_options_frame = g_object_new(GAMINGGEAR_MACRO_EDITOR_RECORD_OPTIONS_FRAME_TYPE,
			"label", _("Record options"),
			NULL);

	return GTK_WIDGET(record_options_frame);
}

static void record_event(GaminggearMacroEditorRecordOptionsFrame *record_options_frame) {
	g_signal_emit((gpointer)record_options_frame, signals[RECORD_EVENT], 0);
}

static void paste(GaminggearMacroEditorRecordOptionsFrame *record_options_frame) {
	g_signal_emit((gpointer)record_options_frame, signals[PASTE], 0);
}

static void record_button_clicked_cb(GtkButton *button, gpointer user_data) {
	GaminggearMacroEditorRecordOptionsFrame *record_options_frame = GAMINGGEAR_MACRO_EDITOR_RECORD_OPTIONS_FRAME(user_data);
	record_event(record_options_frame);
}

static void paste_button_clicked_cb(GtkButton *button, gpointer user_data) {
	GaminggearMacroEditorRecordOptionsFrame *record_options_frame = GAMINGGEAR_MACRO_EDITOR_RECORD_OPTIONS_FRAME(user_data);
	paste(record_options_frame);
}

static void delay_value_changed_cb(GtkSpinButton *spin, gpointer user_data) {
	GaminggearMacroEditorRecordOptionsFrame *record_options_frame = GAMINGGEAR_MACRO_EDITOR_RECORD_OPTIONS_FRAME(user_data);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(record_options_frame->priv->delay_fixed), TRUE);
}

static void gaminggear_macro_editor_record_options_frame_init(GaminggearMacroEditorRecordOptionsFrame *record_options_frame) {
	GaminggearMacroEditorRecordOptionsFramePrivate *priv = GAMINGGEAR_MACRO_EDITOR_RECORD_OPTIONS_FRAME_GET_PRIVATE(record_options_frame);
	GtkWidget *vbox1;
	GtkWidget *vbox2;
	GtkWidget *hbox;
	GtkWidget *delay_frame;

	record_options_frame->priv = priv;

	vbox1 = gtk_vbox_new(FALSE, 0);
	vbox2 = gtk_vbox_new(FALSE, 0);
	hbox = gtk_hbox_new(FALSE, 0);
	delay_frame = gtk_frame_new(_("Delay"));

	priv->record_button = GTK_TOGGLE_BUTTON(gtk_toggle_button_new_with_label(_("Record")));
	g_object_ref_sink(priv->record_button);
	priv->paste_button = GTK_BUTTON(gtk_button_new_with_label(_("Paste")));
	g_object_ref_sink(priv->paste_button);
	priv->delay_as_recorded = GTK_RADIO_BUTTON(gtk_radio_button_new_with_label(NULL, _("As recorded")));
	g_object_ref_sink(priv->delay_as_recorded);
	priv->delay_fixed = GTK_RADIO_BUTTON(gtk_radio_button_new_with_label_from_widget(priv->delay_as_recorded, _("Fixed")));
	g_object_ref_sink(priv->delay_fixed);
	priv->delay_value = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0, G_GAMINGGEAR_MSEC_PER_SEC, 1));
	g_object_ref_sink(priv->delay_value);

	g_signal_connect(G_OBJECT(priv->record_button), "clicked", G_CALLBACK(record_button_clicked_cb), record_options_frame);
	g_signal_connect(G_OBJECT(priv->paste_button), "clicked", G_CALLBACK(paste_button_clicked_cb), record_options_frame);
	g_signal_connect(G_OBJECT(priv->delay_value), "value-changed", G_CALLBACK(delay_value_changed_cb), record_options_frame);

	gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(priv->delay_fixed), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(priv->delay_value), FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(vbox2), GTK_WIDGET(priv->delay_as_recorded), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, FALSE, 0);

	gtk_container_add(GTK_CONTAINER(delay_frame), vbox2);

	gtk_box_pack_start(GTK_BOX(vbox1), GTK_WIDGET(priv->record_button), FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox1), GTK_WIDGET(priv->paste_button), FALSE, FALSE, 10);
	gtk_box_pack_start(GTK_BOX(vbox1), delay_frame, FALSE, FALSE, 0);

	gtk_container_add(GTK_CONTAINER(record_options_frame), vbox1);

	gtk_widget_show_all(GTK_WIDGET(vbox1));
}

static void gaminggear_macro_editor_record_options_frame_finalize(GObject *object) {
	GaminggearMacroEditorRecordOptionsFramePrivate *priv = GAMINGGEAR_MACRO_EDITOR_RECORD_OPTIONS_FRAME(object)->priv;
	g_object_unref(priv->record_button);
	g_object_unref(priv->delay_value);
	g_object_unref(priv->delay_as_recorded);
	g_object_unref(priv->delay_fixed);
	G_OBJECT_CLASS(gaminggear_macro_editor_record_options_frame_parent_class)->finalize(object);
}

static void gaminggear_macro_editor_record_options_frame_class_init(GaminggearMacroEditorRecordOptionsFrameClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->finalize = gaminggear_macro_editor_record_options_frame_finalize;

	g_type_class_add_private(klass, sizeof(GaminggearMacroEditorRecordOptionsFramePrivate));

	signals[RECORD_EVENT] = g_signal_new("record-event",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

	signals[PASTE] = g_signal_new("paste",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}

gboolean gaminggear_macro_editor_record_options_frame_delay_as_recorded(GaminggearMacroEditorRecordOptionsFrame *record_options_frame) {
	return gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(record_options_frame->priv->delay_as_recorded));
}

guint gaminggear_macro_editor_record_options_frame_get_delay(GaminggearMacroEditorRecordOptionsFrame *record_options_frame) {
	return (guint)gtk_spin_button_get_value(record_options_frame->priv->delay_value);
}

gboolean gaminggear_macro_editor_record_options_frame_is_record_on(GaminggearMacroEditorRecordOptionsFrame *record_options_frame) {
	return gtk_toggle_button_get_active(record_options_frame->priv->record_button);
}
