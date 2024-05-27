/* tp-2a-s4-window.h
 *
 * Copyright 2024 Adrien
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <adwaita.h>

#include <shumate/shumate.h>

G_BEGIN_DECLS

#define TP_2A_S4_TYPE_WINDOW (tp_2a_s4_window_get_type())

G_DECLARE_FINAL_TYPE (Tp2aS4Window, tp_2a_s4_window, TP_2A_S4, WINDOW, AdwApplicationWindow)

Tp2aS4Window *shumate_demo_window_new (AdwApplication *app);

void onAddressChange(GtkEntryCompletion *completion, Tp2aS4Window *self);

static gboolean always_match_func(GtkEntryCompletion *completion, const gchar *key, GtkTreeIter *iter, gpointer user_data);

static void onGenerationTour(GtkWidget *widget, Tp2aS4Window *self);

G_END_DECLS
