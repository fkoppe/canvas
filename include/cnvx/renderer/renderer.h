/************************************************************************************
*                                                                                   *
*   canvas - https://github.com/fkoppe/canvas                                       *
*   ************************************************************************        *
*                                                                                   *
*   Copyright (C) 2022 - 2023 Felix Koppe <fkoppe@web.de>                           *
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

#ifndef ___CNVX___RENDERER_H
#define ___CNVX___RENDERER_H

#include "sprx/core/essentials.h"
#include "sprx/core/info.h"

typedef enum CNVX_Renderer_Shader_Type
{
    CNVX_RENDERER_SHADER_TYPE_FRAGMENT,
    CNVX_RENDERER_SHADER_TYPE_VERTEX,
    ___CNVX_RENDERER_SHADER_TYPE_MAX,
} CNVX_Renderer_Shader_Type;

typedef struct CNVX_Renderer_Settings
{
    bool vsync_is;
} CNVX_Renderer_Settings;

void* canvas_renderer_new(const CNVX_Renderer_Settings settings, const char* const app_name, const SPRX_VERSION app_version, const char* const engine_name, const SPRX_VERSION engine_version, const size_t id, void* const logger);
void canvas_renderer_delete(void* const renderer);

void canvas_renderer_start(void* const renderer, void* const window);
void canvas_renderer_stop(void* const renderer);

void canvas_renderer_update(void* const renderer);

void canvas_renderer_resize(void* const renderer);

void canvas_renderer_shader_load(void* const renderer, const CNVX_Renderer_Shader_Type shader_type, const char* const path);

#endif // ___CNVX___RENDERER_H
