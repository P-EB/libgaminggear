/*
 * This file is part of gaminggear-tools.
 *
 * gaminggear-tools is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * gaminggear-tools is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with gaminggear-tools. If not, see <http://www.gnu.org/licenses/>.
 */

#include "gaminggear/gaminggear_profile_page.h"
#include "gaminggear_helper.h"
#include "i18n-lib.h"

#define GAMINGGEAR_PROFILE_PAGE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), GAMINGGEAR_PROFILE_PAGE_TYPE, GaminggearProfilePagePrivate))

struct _GaminggearProfilePagePrivate {
	GaminggearProfileData *profile_data;
};

G_DEFINE_TYPE(GaminggearProfilePage, gaminggear_profile_page, GTK_TYPE_VBOX);

enum {
	IMPORT,
	EXPORT,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

static void import_cb(GtkButton *button, gpointer user_data) {
	GaminggearProfilePage *profile_page = GAMINGGEAR_PROFILE_PAGE(user_data);
	g_signal_emit((gpointer)profile_page, signals[IMPORT], 0);
}

static void export_cb(GtkButton *button, gpointer user_data) {
	GaminggearProfilePage *profile_page = GAMINGGEAR_PROFILE_PAGE(user_data);
	g_signal_emit((gpointer)profile_page, signals[EXPORT], 0);
}

static void reset_cb(GtkButton *button, gpointer user_data) {
	GaminggearProfilePage *profile_page = GAMINGGEAR_PROFILE_PAGE(user_data);
	gaminggear_profile_data_reset(profile_page->priv->profile_data);
	GAMINGGEAR_PROFILE_PAGE_GET_CLASS(profile_page)->set_from_data(profile_page, profile_page->priv->profile_data);
}

static GtkWidget *buttons_new(GaminggearProfilePage *profile_page) {
	GtkWidget *hbox1, *hbox2, *button;

	hbox1 = gtk_hbox_new(FALSE, 0);
	hbox2 = gtk_hbox_new(TRUE, 0);

	gtk_box_pack_start(GTK_BOX(hbox1), hbox2, FALSE, FALSE, 0);

	button = gtk_button_new_with_label(_("Import"));
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(import_cb), profile_page);
	gtk_box_pack_start(GTK_BOX(hbox2), button, TRUE, TRUE, 0);

	button = gtk_button_new_with_label(_("Export"));
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(export_cb), profile_page);
	gtk_box_pack_start(GTK_BOX(hbox2), button, TRUE, TRUE, 0);

	button = gtk_button_new_with_label(_("Reset"));
	g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(reset_cb), profile_page);
	gtk_box_pack_start(GTK_BOX(hbox2), button, TRUE, TRUE, 0);

	return hbox1;
}

static void gaminggear_profile_page_init(GaminggearProfilePage *profile_page) {
	GaminggearProfilePagePrivate *priv = GAMINGGEAR_PROFILE_PAGE_GET_PRIVATE(profile_page);

	profile_page->priv = priv;

	priv->profile_data = NULL;
	gtk_widget_set_sensitive(GTK_WIDGET(profile_page), FALSE);
}

static GObject *constructor(GType gtype, guint n_properties, GObjectConstructParam *properties) {
	GObject *obj;
	GtkBox *box;
	GaminggearProfilePage *profile_page;
	GtkWidget *buttons;

	obj = G_OBJECT_CLASS(gaminggear_profile_page_parent_class)->constructor(gtype, n_properties, properties);
	box = GTK_BOX(obj);
	profile_page = GAMINGGEAR_PROFILE_PAGE(obj);

	gtk_box_set_homogeneous(box, FALSE);
	gtk_box_set_spacing(box, 0);

	buttons = buttons_new(profile_page);
	gtk_box_pack_end(box, buttons, FALSE, FALSE, 0);

	gtk_widget_show_all(GTK_WIDGET(profile_page));

	return obj;
}

static void finalize(GObject *object) {
	GaminggearProfilePagePrivate *priv = GAMINGGEAR_PROFILE_PAGE(object)->priv;

	g_clear_object(&priv->profile_data);

	G_OBJECT_CLASS(gaminggear_profile_page_parent_class)->finalize(object);
}

static void gaminggear_profile_page_class_init(GaminggearProfilePageClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->constructor = constructor;
	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(GaminggearProfilePagePrivate));

	signals[IMPORT] = g_signal_new("import",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION, 0,
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

	signals[EXPORT] = g_signal_new("export",
			G_TYPE_FROM_CLASS(klass),
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION, 0,
			NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}

void gaminggear_profile_page_update_data(GaminggearProfilePage *self) {
	GaminggearProfilePagePrivate *priv = self->priv;
	GtkWidget *dialog;
	GaminggearProfileData *temp;
	gboolean equal;

	if (!priv->profile_data)
		return;

	if (!gaminggear_profile_data_get_valid(priv->profile_data))
		return;

	temp = gaminggear_profile_data_dup(priv->profile_data);
	GAMINGGEAR_PROFILE_PAGE_GET_CLASS(self)->update_data(self, temp);

	equal = gaminggear_profile_data_equal(priv->profile_data, temp);
	g_object_unref(temp);

	if (equal)
		return;

	dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, _("Would you like to store actual modifications?"));
	gtk_window_set_title(GTK_WINDOW(dialog), _("Store modifications"));

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_YES)
		GAMINGGEAR_PROFILE_PAGE_GET_CLASS(self)->update_data(self, priv->profile_data);

	gtk_widget_destroy(dialog);
}

void gaminggear_profile_page_set_profile_data(GaminggearProfilePage *self, GaminggearProfileData *profile_data) {
	GaminggearProfilePagePrivate *priv = self->priv;

	if (profile_data != priv->profile_data) {
		if (priv->profile_data) {
			gaminggear_profile_page_update_data(self);
			g_object_unref(G_OBJECT(priv->profile_data));
		}

		if (profile_data)
			g_object_ref(G_OBJECT(profile_data));

		priv->profile_data = profile_data;
	}

	if (priv->profile_data) {
		GAMINGGEAR_PROFILE_PAGE_GET_CLASS(self)->set_from_data(self, priv->profile_data);
		gtk_widget_set_sensitive(GTK_WIDGET(self), TRUE);
	} else {
		gtk_widget_set_sensitive(GTK_WIDGET(self), FALSE);
	}
}

GaminggearProfileData *gaminggear_profile_page_get_profile_data(GaminggearProfilePage *self) {
	GaminggearProfilePagePrivate *priv = self->priv;
	if (priv->profile_data)
		g_object_ref(priv->profile_data);
	return priv->profile_data;
}
