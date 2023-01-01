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

#ifndef ___CNVX___VULKAN_PRIVATE_H
#define ___CNVX___VULKAN_PRIVATE_H

#include "sprx/core/essentials.h"
#include "sprx/core/terminate.h"

#ifndef ___CNVX_NOASSERT
    #ifdef ___CNVX_DEBUG
        #define CNVX_VULKAN_ASSERT(renderer, result, what) canvas_vulkan_assert(renderer, false, result, __FILE__, __func__, __LINE__, __DATE__, __TIME__, what)
        #define CNVX_VULKAN_ASSERTF(renderer, result, format, ...) canvas_vulkan_assertf(renderer, false, result, __FILE__, __func__, __LINE__, __DATE__, __TIME__, format, __VA_ARGS__)
        #define CNVX_VULKAN_QASSERT(renderer, result, what) canvas_vulkan_assert(renderer, true, result, __FILE__, __func__, __LINE__, __DATE__, __TIME__, what)
        #define CNVX_VULKAN_QASSERTF(renderer, result, format, ...) canvas_vulkan_assertf(renderer, true, result, what, __FILE__, __func__, __LINE__, __DATE__, __TIME__, format, __VA_ARGS__)
    #else
    #define CNVX_VULKAN_ASSERT(renderer, result, what) if(VK_SUCCESS != result) SPRX_ABORT
    #define CNVX_VULKAN_ASSERTF(renderer, result, format, ...) if(VK_SUCCESS != result) SPRX_ABORT
    #define CNVX_VULKAN_QASSERT(renderer, result, what) if(VK_SUCCESS != result) SPRX_ABORT
    #define CNVX_VULKAN_QASSERTF(renderer, result, format, ...) if(VK_SUCCESS != result) SPRX_ABORT
    #endif // !___CNVX_DEBUG
#else
    #define CNVX_VULKAN_ASSERT(renderer, result, what)
    #define CNVX_VULKAN_ASSERTF(renderer, result, format, ...)
    #define CNVX_VULKAN_QASSERT(renderer, result, what)
    #define CNVX_VULKAN_QASSERTF(renderer, result, format, ...)
#endif // ___CNVX_NOASSERT

void canvas_vulkan_assert(void* const renderer, bool suppress_is, const VkResult result, const char* const file, const char* const func, const int line, const char* const date, const char* const time, const char* const what);
void canvas_vulkan_assertf(void* const renderer, bool suppress_is, const VkResult result, const char* const file, const char* const func, const int line, const char* const date, const char* const time, const char* const format, ...);

//initialisation/shutdown
void canvas_vulkan_instance_create(void* const renderer);
void canvas_vulkan_instance_destroy(void* const renderer);

void canvas_vulkan_physical_devices_enumerate(void* const renderer);
void canvas_vulkan_physical_devices_denumerate(void* const renderer);

void canvas_vulkan_device_create(void* const renderer);
void canvas_vulkan_device_destroy(void* const renderer);

//start/stop
void canvas_vulkan_surface_create(void* const renderer);
void canvas_vulkan_surface_destroy(void* const renderer);

void canvas_vulkan_swapchain_create(void* const renderer);
void canvas_vulkan_swapchain_destroy(void* const renderer);

void canvas_vulkan_imageviews_create(void* const renderer);
void canvas_vulkan_imageviews_destroy(void* const renderer);

void canvas_vulkan_shader_create(void* const renderer);
void canvas_vulkan_shader_destroy(void* const renderer);

void canvas_vulkan_pipeline_create(void* const renderer);
void canvas_vulkan_pipeline_destroy(void* const renderer);

void canvas_vulkan_framebuffer_create(void* const renderer);
void canvas_vulkan_framebuffer_destroy(void* const renderer);

void canvas_vulkan_commandpool_create(void* const renderer);
void canvas_vulkan_commandpool_destroy(void* const renderer);

void canvas_vulkan_commandbuffer_create(void* const renderer);
void canvas_vulkan_commandbuffer_destroy(void* const renderer);

void canvas_vulkan_commandbuffer_record(void* const renderer);

void canvas_vulkan_semaphore_create(void* const renderer);
void canvas_vulkan_semaphore_destroy(void* const renderer);

//update
void canvas_vulkan_frame_draw(void* const renderer);

#endif // ___CNVX___VULKAN_PRIVATE_H
