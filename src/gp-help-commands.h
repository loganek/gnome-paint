/* gp-help-commands.h
 *
 * Copyright (C) 2017 Marcin Kolny
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GP_HELP_COMMANDS_H_
#define GP_HELP_COMMANDS_H_

#include <gtk/gtk.h>

G_BEGIN_DECLS

void gp_help_commands_keyboard_shortcuts (GSimpleAction *action,
                                          GVariant *parameter,
                                          gpointer user_data);

void gp_help_commands_about (GSimpleAction *action,
                             GVariant *parameter,
                             gpointer user_data);

void gp_help_commands_help (GSimpleAction *action,
                            GVariant *parameter,
                            gpointer user_data);

G_END_DECLS

#endif /* GP_HELP_COMMANDS_H_ */
