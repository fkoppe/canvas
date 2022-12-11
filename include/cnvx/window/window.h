/************************************************************************************
*                                                                                   *
*   canvas - https://github.com/fkoppe/canvas                                       *
*   ************************************************************************        *
*                                                                                   *
*   Copyright (C) 2022 Felix Koppe <fkoppe@web.de>                                  *
*                                                                                   *
*   This program is free software: you can redistribute it and/or modify            *
*   it under the terms of the GNU Affero General Public License as published        *
*   by the Free Software Foundation, either version 3 of the License, or            *
*   (at your option) any later version.                                             *
*                                                                                   *
*   This program is distributed in the hope that it will be useful,                 *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of                  *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                   *
*   GNU Affero General Public License for more details.                             *
*                                                                                   *
*   You should have received a copy of the GNU Affero General Public License        *
*   along with this program.  If not, see <https://www.gnu.org/licenses/>.          *
*                                                                                   *
************************************************************************************/

#ifndef ___CNVX___WINDOW_H
#define ___CNVX___WINDOW_H

#include "sprx/core/essentials.h"

typedef enum CNVX_Window_Position
{
    CNVX_WINDOW_POSITION_DEFAULT,
    CNVX_WINDOW_POSITION_CUSTOM,
    ___CNVX_WINDOW_POSITION_MAX,
} CNVX_Window_Position;

typedef struct CNVX_Window_Settings
{
    struct
    {
        size_t width;
        size_t height;
    } limits;
    CNVX_Window_Position position;
    struct
    {
        struct
        {
            size_t x;
            size_t y;
        } position;
    } custom;
} CNVX_Window_Settings;

void* canvas_window_new(const CNVX_Window_Settings settings, const size_t unique_id, void* const handler, void* const logger);
void canvas_window_delete(void* const window);

CNVX_Window_Settings canvas_window_settings_get(void* const window);
void canvas_window_settings_set(void* const window, const CNVX_Window_Settings settings);

void canvas_window_open(void* const window, const char* const title, const size_t width, const size_t height, bool visible_is, void* const renderer);
void canvas_window_close(void* const window);
bool canvas_window_open_is(void* const window);

void canvas_window_update(void* const window);

void canvas_window_hide(void* const window);
void canvas_window_show(void* const window);
bool canvas_window_visible_is(void* const window);

void canvas_window_size_get(void* const window, size_t* const width_dest, size_t* const height_dest);
void canvas_window_resize(void* const window, const size_t width, const size_t height);

void canvas_window_framebuffer_size_get(void* const window, size_t* const width_dest, size_t* const height_dest);

void canvas_window_position_get(void* const window, size_t* const x_dest, size_t* const y_dest);
void canvas_window_reposition(void* const window, const size_t x, const size_t y);

void canvas_window_title_set(void* const window, const char* const title);

void* canvas_window_handle_get(void* const window);

#endif // ___CNVX___WINDOW_H
