#ifndef __GAMINGGEAR_DEVICE_H__
#define __GAMINGGEAR_DEVICE_H__

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

/*! \file gaminggear/gaminggear_device.h
 *  \brief Device
 */

#include <glib-object.h>

G_BEGIN_DECLS

#define GAMINGGEAR_DEVICE_TYPE (gaminggear_device_get_type())
#define GAMINGGEAR_DEVICE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GAMINGGEAR_DEVICE_TYPE, GaminggearDevice))
#define IS_GAMINGGEAR_DEVICE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GAMINGGEAR_DEVICE_TYPE))
#define GAMINGGEAR_DEVICE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GAMINGGEAR_DEVICE_TYPE, GaminggearDeviceClass))
#define IS_GAMINGGEAR_DEVICE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GAMINGGEAR_DEVICE_TYPE))

typedef struct _GaminggearDevice GaminggearDevice;
typedef struct _GaminggearDeviceClass GaminggearDeviceClass;
typedef struct _GaminggearDevicePrivate GaminggearDevicePrivate;

struct _GaminggearDevice {
	GObject parent;
	GaminggearDevicePrivate *priv;
};

struct _GaminggearDeviceClass {
	GObjectClass parent_class;
};

GType gaminggear_device_get_type(void);

/*! \brief Creates new GaminggearDevice.
 *  \param identifier The unique identifier of the device.
 *  \param vendor_id The vendor id of the device.
 *  \param product_id The product id of the device.
 *  \param num_interfaces The number of interfaces of the device.
 *  \retval device The new GaminggearDevice which should be freed with g_object_unref().
 *  \since 1.0
 */
GaminggearDevice *gaminggear_device_new(gchar const *identifier, guint vendor_id, guint product_id, guint num_interfaces);

/*! \brief Get identifier.
 *  \param gaminggear_dev A GaminggearDevice.
 *  \retval identifier An identifier that was given at construction time and is unique for the usb device.
 *  \since 1.0
 */
gchar const *gaminggear_device_get_identifier(GaminggearDevice const *gaminggear_dev);

/*! \brief Get product id.
 *  \param gaminggear_dev A GaminggearDevice.
 *  \retval product_id The product id given at construction time.
 *  \since 1.0
 */
guint gaminggear_device_get_product_id(GaminggearDevice const *gaminggear_dev);

/*! \brief Get vendor id.
 *  \param gaminggear_dev A GaminggearDevice.
 *  \retval vendor_id The vendor id given at construction time.
 *  \since 1.0
 */
guint gaminggear_device_get_vendor_id(GaminggearDevice const *gaminggear_dev);

/*! \brief Get number of interfaces.
 *  \param gaminggear_dev A GaminggearDevice.
 *  \retval num_interfaces The number of interfaces of the device given at construction time.
 *  \since 1.0
 */
guint gaminggear_device_get_num_interfaces(GaminggearDevice const *gaminggear_dev);

/*! \brief Match device agains vendor/product id
 *  \param gaminggear_dev A GaminggearDevice.
 *  \param vendor_id The vendor id to match against.
 *  \param product_id The product id to match against.
 *  \retval bool \c TRUE if vendor and product ids match, else \c FALSE.
 *  \since 1.0
 */
gboolean gaminggear_device_matches(GaminggearDevice const *gaminggear_dev, guint vendor_id, guint product_id);

/*! \brief Open file.
 *
 *  Once successfully opened, the file descriptor is stored and just returned on
 *  subsequent calls. File should be closed with gaminggear_device_close() instead
 *  of close(). The using code is responsible for closing all files prior to unref
 *  of GaminggearDevice.
 *
 *  \param gaminggear_device A GaminggearDevice.
 *  \param key A key to identify path.
 *  \param flags Flags for open.
 *  \param error Return location for error or \c NULL.
 *  \retval fd File descriptor.
 *  \since 1.0
 */
/* returns value < 0 on error */
int gaminggear_device_open(GaminggearDevice *gaminggear_device, gchar const *key, gint flags, GError **error);

/*! \brief Close file.
 *  \param gaminggear_device A GaminggearDevice.
 *  \param key A key to identify path.
 *  \param error Return location for error or \c NULL.
 *  \retval condition \c TRUE on success, \c FALSE else.
 *  \since 1.0
 */
gboolean gaminggear_device_close(GaminggearDevice *gaminggear_device, gchar const *key, GError **error);

/*! \brief Store a path.
 *  \param gaminggear_device A GaminggearDevice.
 *  \param key A key to identify path.
 *  \param path The path to be stored.
 *  \since 1.0
 */
void gaminggear_device_set_path(GaminggearDevice *gaminggear_device, gchar const *key, gchar const *path);

/*! \brief Get a previously stored path.
 *  \param gaminggear_device A GaminggearDevice.
 *  \param key A key to identify path.
 *  \retval path or \c NULL. This path is property of the device and should no be freed!
 *  \since 1.0
 */
gchar const *gaminggear_device_get_path(GaminggearDevice *gaminggear_device, gchar const *key);

/*! \brief Lock recursive lock
 *  \param gaminggear_dev A GaminggearDevice.
 *  \since 1.0
 */
void gaminggear_device_lock(GaminggearDevice *gaminggear_dev);

/*! \brief Unlock recursive lock
 *  \param gaminggear_dev A GaminggearDevice.
 *  \since 1.0
 */
void gaminggear_device_unlock(GaminggearDevice *gaminggear_dev);

/*! \brief Tests if two GaminggearDevices point to the same device.
 *  \param one A GaminggearDevice.
 *  \param other Another GaminggearDevice.
 *  \retval bool \c TRUE if devices are equal, \c FALSE else.
 *  \since 1.0
 */
gboolean gaminggear_device_equal(GaminggearDevice const *one, GaminggearDevice const *other);

G_END_DECLS

#endif

