#ifndef __GAMINGGEAR_TIMEOUT_BAR_H__
#define __GAMINGGEAR_TIMEOUT_BAR_H__

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

/*! \file gaminggear/gaminggear_timeout_bar.h
 *  \brief A progression bar counting down in an amount of seconds.
 */
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GAMINGGEAR_TIMEOUT_BAR_TYPE (gaminggear_timeout_bar_get_type())
#define GAMINGGEAR_TIMEOUT_BAR(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GAMINGGEAR_TIMEOUT_BAR_TYPE, GaminggearTimeoutBar))
#define IS_GAMINGGEAR_TIMEOUT_BAR(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GAMINGGEAR_TIMEOUT_BAR_TYPE))

typedef struct _GaminggearTimeoutBar GaminggearTimeoutBar;

GType gaminggear_timeout_bar_get_type(void);

/*! \brief Creates new timeout bar.
 *  \retval widget The timeout bar.
 *  \since 1.0
 */
GtkWidget *gaminggear_timeout_bar_new(void);

/*! \brief Starts a timeout bar.
 *
 *  The progress bar starts full and will decrease in timeout seconds to 0.0.
 *  A "timeout" signal will be emitted if 0.0 is reached.
 *
 *  \param timeout_bar A timeout bar.
 *  \param interval The update interval in milliseconds.
 *  \param timeout The time in seconds.
 *  \since 1.0
 */
void gaminggear_timeout_bar_start(GaminggearTimeoutBar *timeout_bar, guint interval, guint timeout);

/*! \brief Stops a timeout bar.
 *  \param timeout_bar A timeout bar.
 *  \since 1.0
 */
void gaminggear_timeout_bar_stop(GaminggearTimeoutBar *timeout_bar);

G_END_DECLS

#endif
