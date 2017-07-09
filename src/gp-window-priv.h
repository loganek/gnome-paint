/* gp-window-priv.h
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

#ifndef GP_WINDOW_PRIV_H_
#define GP_WINDOW_PRIV_H_

#include "gp-imageeditor.h"
#include "gp-toolbox.h"
#include "gp-colorselectorbox.h"
#include "gp-headerbar.h"
#include "gp-window.h"

G_BEGIN_DECLS

typedef struct
{
    GPImageEditor *image_editor;
    GPToolBox *tool_box;
    GPColorSelectorBox *color_selector_box;
    GPHeaderBar *header_bar;
} GPWindowPrivate;

GPWindowPrivate * gp_window_get_priv (GPWindow *window);

#define GP_WINDOW_PRIV(window) gp_window_get_priv (GP_WINDOW (window))

G_END_DECLS

#endif /* GP_WINDOW_PRIV_H_ */
