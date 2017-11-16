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

#include "gaminggear_about_dialog.h"
#include "gaminggear/gaminggear.h"

void gaminggear_about_dialog(GtkWindow *parent, gchar const *program_name, gchar const *comments) {
	gchar const *authors[] = {
			"Stefan Achatz <erazor_de@users.sourceforge.net>",
			NULL
	};
	gtk_show_about_dialog(parent,
			"authors", authors,
			"comments", comments,
			"copyright", "Copyright \302\251 2015 Stefan Achatz",
			"license", "GPL 2 (GNU General Public License)\n\nsee http://www.gnu.org/licenses/old-licenses/gpl-2.0.html",
			"modal", TRUE,
			"program-name", program_name,
			"version", GAMINGGEAR_VERSION_STRING,
			"website", "http://gaminggear.sourceforge.net",
			"website-label", "gaminggear.sourceforge.net",
			NULL);
}
