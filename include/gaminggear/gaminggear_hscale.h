#ifndef __GAMINGGEAR_HSCALE_H__
#define __GAMINGGEAR_HSCALE_H__

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

/*! \file gaminggear/gaminggear_hscale.h
 *  \brief Horizontal scale with connected spinbutton and clamping.
 *
 *  Properties
 *  - digits\n
 *    Number of digits to display in spinbutton.
 *  - value\n
 *    Actual value.
 *  - min\n
 *    Minimal value.
 *  - max\n
 *    Maximum value.
 *  - step\n
 *    Step increment.
 *
 *  Signals
 *  - value-changed\n
 *    Emitted when value is changed.
 */

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GAMINGGEAR_HSCALE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GAMINGGEAR_HSCALE_TYPE, GaminggearHScaleClass))
#define IS_GAMINGGEAR_HSCALE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GAMINGGEAR_HSCALE_TYPE))
#define GAMINGGEAR_HSCALE_TYPE (gaminggear_hscale_get_type())
#define GAMINGGEAR_HSCALE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GAMINGGEAR_HSCALE_TYPE, GaminggearHScale))
#define IS_GAMINGGEAR_HSCALE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GAMINGGEAR_HSCALE_TYPE))

typedef struct _GaminggearHScaleClass GaminggearHScaleClass;
typedef struct _GaminggearHScale GaminggearHScale;
typedef struct _GaminggearHScalePrivate GaminggearHScalePrivate;

struct _GaminggearHScaleClass {
	GtkHBoxClass parent_class;
};

struct _GaminggearHScale {
	GtkHBox hbox;
	GaminggearHScalePrivate *priv;
};

GType gaminggear_hscale_get_type(void);

/*! \brief Creates new hscale.
 *  \param min Minimum value.
 *  \param max Maximum value.
 *  \param step Step increment.
 *  \retval widget A new GaminggearHScale.
 */
GtkWidget *gaminggear_hscale_new_with_range(gdouble min, gdouble max, gdouble step);

/*! \brief Gets the current value of the scale.
 *  \param hscale A GaminggearHScale.
 *  \retval value Actual value.
 */
gdouble gaminggear_hscale_get_value(GaminggearHScale *hscale);

/*! \brief Sets current value of scale.
 *
 *  The scale emits "value-changed" signal after changing the value.
 *
 *  \param hscale A GaminggearHScale.
 *  \param value The new value.
 */
void gaminggear_hscale_set_value(GaminggearHScale *hscale, gdouble value);

/*! \brief Adds a tick mark to the scale.
 *
 *  Marks can only be removed in whole by gaminggear_hscale_clear_marks().
 *
 *  \param hscale A GaminggearHScale.
 *  \param value Between limits of hscale.
 *  \param position Where to draw the mark. \c GTK_POS_TOP or \c GTK_POS_BOTTOM.
 *  \param markup Text for the mark or \c NULL.
 */
void gaminggear_hscale_add_mark(GaminggearHScale *hscale, gdouble value, GtkPositionType position, gchar const *markup);

/*! \brief Removes all marks from the scale.
 *
 *  Removes all marks that have been added by gaminggear_hscale_add_mark().
 *
 *  \param hscale A GaminggearHScale.
 */
void gaminggear_hscale_clear_marks(GaminggearHScale *hscale);

/*! \brief Gets number of decimal places displayed in the spinbutton.
 *  \param hscale A GaminggearHScale.
 *  \retval digits Actual digits.
 */
guint gaminggear_hscale_get_digits(GaminggearHScale *hscale);

/*! \brief Sets number of decimal places displayed in the spinbutton.
 *  \param hscale A GaminggearHScale.
 *  \param digits Number of decimal places.
 */
void gaminggear_hscale_set_digits(GaminggearHScale *hscale, guint digits);

/*! \brief Sets range of scale.
 *  \param hscale A GaminggearHScale.
 *  \param min Minimum value.
 *  \param max Maximum value.
 */
void gaminggear_hscale_set_range(GaminggearHScale *hscale, gdouble min, gdouble max);

/*! \brief Sets increment size.
 *
 *  GaminggearHScale clamps its value to this increment. This means value is always:
 *  value = min + x * step
 *
 *  \param hscale A GaminggearHScale.
 *  \param step Step increment.
 */
void gaminggear_hscale_set_increment(GaminggearHScale *hscale, gdouble step);

/*! \brief Sets wether to draw the spinbutton.
 *  \param hscale A GaminggearHScale.
 *  \param draw_spin \c TRUE or \c FALSE.
 */
void gaminggear_hscale_set_draw_spin(GaminggearHScale *hscale, gboolean draw_spin);

G_END_DECLS

#endif
