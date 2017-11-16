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

#include "gaminggear/gaminggear_profile_data.h"

#define GAMINGGEAR_PROFILE_DATA_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), GAMINGGEAR_PROFILE_DATA_TYPE, GaminggearProfileDataPrivate))

struct _GaminggearProfileDataPrivate {
	gboolean valid;
};

G_DEFINE_TYPE(GaminggearProfileData, gaminggear_profile_data, G_TYPE_OBJECT);

static void gaminggear_profile_data_init(GaminggearProfileData *profile_data) {
	GaminggearProfileDataPrivate *priv = GAMINGGEAR_PROFILE_DATA_GET_PRIVATE(profile_data);
	profile_data->priv = priv;

	priv->valid = TRUE;
}

static void copy(GaminggearProfileData *self, GaminggearProfileData *other) {
	self->priv->valid = other->priv->valid;
}

static void discriminate(GaminggearProfileData *self) {}

static void gaminggear_profile_data_class_init(GaminggearProfileDataClass *klass) {
	g_type_class_add_private(klass, sizeof(GaminggearProfileDataPrivate));

	klass->copy = copy;
	klass->discriminate = discriminate;
}

gchar const *gaminggear_profile_data_get_name(GaminggearProfileData *self) {
	if (!self->priv->valid)
		return NULL;
	return GAMINGGEAR_PROFILE_DATA_GET_CLASS(self)->get_name(self);
}

void gaminggear_profile_data_set_name(GaminggearProfileData *self, gchar const *new_name) {
	if (!self->priv->valid)
		return;
	GAMINGGEAR_PROFILE_DATA_GET_CLASS(self)->set_name(self, new_name);
}

gchar const *gaminggear_profile_data_get_gamefile(GaminggearProfileData *self, guint index) {
	if (!self->priv->valid)
		return NULL;
	return GAMINGGEAR_PROFILE_DATA_GET_CLASS(self)->get_gamefile(self, index);
}

void gaminggear_profile_data_set_gamefile(GaminggearProfileData *self, guint index, gchar const *new_gamefile) {
	if (!self->priv->valid)
		return;
	GAMINGGEAR_PROFILE_DATA_GET_CLASS(self)->set_gamefile(self, index, new_gamefile);
}

gint gaminggear_profile_data_get_hardware_index(GaminggearProfileData *self) {
	if (!self->priv->valid)
		return GAMINGGEAR_PROFILE_DATA_GAMEFILE_PROFILE_INVALID;
	return GAMINGGEAR_PROFILE_DATA_GET_CLASS(self)->get_hardware_index(self);
}

void gaminggear_profile_data_set_hardware_index(GaminggearProfileData *self, gint new_index) {
	if (!self->priv->valid)
		return;
	GAMINGGEAR_PROFILE_DATA_GET_CLASS(self)->set_hardware_index(self, new_index);
}

gboolean gaminggear_profile_data_store(GaminggearProfileData *self, GaminggearDevice *device, GError **error) {
	if (!self->priv->valid)
		return TRUE;

	if (!GAMINGGEAR_PROFILE_DATA_GET_CLASS(self)->save(self, error))
		return FALSE;

	if (!device)
		return TRUE;

	if (gaminggear_profile_data_get_hardware_index(self) == GAMINGGEAR_PROFILE_DATA_GAMEFILE_PROFILE_INVALID)
		return TRUE;

	return GAMINGGEAR_PROFILE_DATA_GET_CLASS(self)->write(self, device, error);
}

gboolean gaminggear_profile_data_load(GaminggearProfileData *self, gchar const *path, GError **error) {
	GError *local_error = NULL;

	if (!self->priv->valid)
		return TRUE;

	if (!GAMINGGEAR_PROFILE_DATA_GET_CLASS(self)->load(self, path, &local_error)) {
		if (g_error_matches(local_error, G_FILE_ERROR, G_FILE_ERROR_NOENT)) {
			g_error_free(local_error);
			return TRUE;
		} else {
			g_propagate_error(error, local_error);
			return FALSE;
		}
	}
	return TRUE;
}

gboolean gaminggear_profile_data_remove(GaminggearProfileData *self, GError **error) {
	gboolean retval;

	if (!self->priv->valid)
		return TRUE;

	retval = GAMINGGEAR_PROFILE_DATA_GET_CLASS(self)->remove(self, error);
	self->priv->valid = FALSE;
	return retval;
}

gboolean gaminggear_profile_data_read(GaminggearProfileData *self, GaminggearDevice *device, GError **error) {
	if (!self->priv->valid)
		return TRUE;

	if (!device)
		return TRUE;

	if (gaminggear_profile_data_get_hardware_index(self) == GAMINGGEAR_PROFILE_DATA_GAMEFILE_PROFILE_INVALID)
		return TRUE;

	return GAMINGGEAR_PROFILE_DATA_GET_CLASS(self)->read(self, device, error);
}

gboolean gaminggear_profile_data_get_modified(GaminggearProfileData *self) {
	if (!self->priv->valid)
		return FALSE;
	return GAMINGGEAR_PROFILE_DATA_GET_CLASS(self)->get_modified(self);
}

void gaminggear_profile_data_reset(GaminggearProfileData *self) {
	if (!self->priv->valid)
		return;
	return GAMINGGEAR_PROFILE_DATA_GET_CLASS(self)->reset(self);
}

gboolean gaminggear_profile_data_get_valid(GaminggearProfileData *self) {
	return self->priv->valid;
}

GaminggearProfileData *gaminggear_profile_data_dup(GaminggearProfileData *other) {
	GaminggearProfileData *new_object;

	if (!other->priv->valid)
		return NULL;

	new_object = GAMINGGEAR_PROFILE_DATA(g_object_new(G_OBJECT_TYPE(other), NULL));
	GAMINGGEAR_PROFILE_DATA_GET_CLASS(new_object)->copy(new_object, other);
	return new_object;
}

gboolean gaminggear_profile_data_equal(GaminggearProfileData *self, GaminggearProfileData *other) {
	if (!self->priv->valid || !other->priv->valid)
		return FALSE;
	return GAMINGGEAR_PROFILE_DATA_GET_CLASS(self)->equal(self, other);
}

void gaminggear_profile_data_discriminate(GaminggearProfileData *self) {
	if (!self->priv->valid)
		return;
	GAMINGGEAR_PROFILE_DATA_GET_CLASS(self)->discriminate(self);
}
