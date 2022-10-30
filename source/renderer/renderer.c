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

#include "cnvx/logger/logger.h"
#include "cnvx/renderer/Private/renderer_PRIVATE.h"
#include "cnvx/renderer/Private/vulkan_PRIVATE.h"
#include "cnvx/window/window.h"

#include "sprx/container/string.h"
#include "sprx/container/vector.h"
#include "sprx/core/assert.h"
#include "sprx/core/core.h"
#include "sprx/file/file.h"

#define CNVX_RENDERER_ERROR_ALLOCATION SPRX_ERROR_ALLOCATION("renderer", NULL, NULL)
#define CNVX_RENDERER_ERROR_RUNTIME(what, info, care) SPRX_ERROR_RUNTIME(what, "renderer", info, care)
#define CNVX_RENDERER_ERROR_LOGIC(what, info, care) SPRX_ERROR_LOGIC(what, "renderer", info, care)
#define CNVX_RENDERER_ERROR_ARGUMENT(care) SPRX_ERROR_ARGUMENT("renderer", NULL, care)
#define CNVX_RENDERER_ERROR_NULL(info) SPRX_ERROR_NULL("renderer", info)
#define CNVX_RENDERER_ERROR_ENUM(info) SPRX_ERROR_ENUM("renderer", info, NULL)

void* canvas_renderer_new(const CNVX_Renderer_Settings settings_, const char* const app_name_, const SPRX_VERSION app_version_, const char* const engine_name_, const SPRX_VERSION engine_version_, const size_t id_, void* const logger_)
{
    //logger is allowed to be =NULL

    SPRX_ASSERT(NULL != app_name_, CNVX_RENDERER_ERROR_NULL("app_name"));
    SPRX_ASSERT(NULL != engine_name_, CNVX_RENDERER_ERROR_NULL("engine_name"));

    CNVX_Renderer_PRIVATE* const renderer = malloc(sizeof(*renderer));
    SPRX_ASSERT(NULL != renderer, CNVX_RENDERER_ERROR_ALLOCATION);

    renderer->id = id_;
    renderer->name = spore_string_new_f("canvas_renderer_%llu", renderer->id);
    renderer->app_name = app_name_;
    renderer->app_version = app_version_;
    renderer->engine_name = engine_name_;
    renderer->engine_version = engine_version_;
    renderer->width = 0;
    renderer->height = 0;
    renderer->shader_vec = spore_vector_new(sizeof(CNVX_Renderer_Shader_PRIVATE));
    renderer->logger = logger_;
    renderer->window = NULL;
    renderer->started_is = false;
    renderer->settings = settings_;

    canvas_vulkan_instance_create(renderer);
    canvas_vulkan_physical_devices_enumerate(renderer);
    canvas_vulkan_device_create(renderer);

    return renderer;
}

void canvas_renderer_delete(void* const renderer_)
{
    SPRX_ASSERT(NULL != renderer_, CNVX_RENDERER_ERROR_NULL("renderer"));

    CNVX_Renderer_PRIVATE* const renderer = renderer_;

    canvas_vulkan_device_destroy(renderer);
    canvas_vulkan_physical_devices_denumerate(renderer);
    canvas_vulkan_instance_destroy(renderer);

    spore_vector_delete(renderer->shader_vec);
    spore_string_delete(renderer->name);

    free(renderer);
}

void canvas_renderer_start(void* const renderer_, void* const window_)
{
    SPRX_ASSERT(NULL != renderer_, CNVX_RENDERER_ERROR_NULL("renderer"));
    SPRX_ASSERT(NULL != window_, CNVX_RENDERER_ERROR_NULL("window"));

    CNVX_Renderer_PRIVATE* const renderer = renderer_;
    renderer->window = window_;

    if (!renderer->started_is)
    {
        renderer->started_is = true;

        SPRX_ASSERT(canvas_window_open_is(renderer->window), CNVX_RENDERER_ERROR_LOGIC("failed to start renderer", "window is not opened", NULL));

        canvas_vulkan_surface_create(renderer);

        canvas_window_framebuffer_size_get(renderer->window, &renderer->width, &renderer->height);
        renderer->width = SPRX_MIN(SPRX_MAX(SPRX_MIN(renderer->width, renderer->vk.surface_capabilities.maxImageExtent.width), renderer->vk.surface_capabilities.minImageExtent.width), UINT32_MAX);
        renderer->height = SPRX_MIN(SPRX_MAX(SPRX_MIN(renderer->height, renderer->vk.surface_capabilities.maxImageExtent.height), renderer->vk.surface_capabilities.minImageExtent.height), UINT32_MAX);

        canvas_vulkan_swapchain_create(renderer);
        canvas_vulkan_imageviews_create(renderer);
        canvas_vulkan_shader_create(renderer);
        canvas_vulkan_pipeline_create(renderer);
        canvas_vulkan_framebuffer_create(renderer);
        canvas_vulkan_commandpool_create(renderer);
        canvas_vulkan_commandbuffer_create(renderer);
        canvas_vulkan_commandbuffer_record(renderer);
        canvas_vulkan_semaphore_create(renderer);
    }
}

void canvas_renderer_stop(void* const renderer_)
{
    SPRX_ASSERT(NULL != renderer_, CNVX_RENDERER_ERROR_NULL("renderer"));

    CNVX_Renderer_PRIVATE* const renderer = renderer_;

    if (renderer->started_is)
    {
        vkDeviceWaitIdle(renderer->vk.device);

        canvas_vulkan_semaphore_destroy(renderer);
        canvas_vulkan_commandbuffer_destroy(renderer);
        canvas_vulkan_commandpool_destroy(renderer);
        canvas_vulkan_framebuffer_destroy(renderer);
        canvas_vulkan_pipeline_destroy(renderer);
        canvas_vulkan_shader_destroy(renderer);
        canvas_vulkan_imageviews_destroy(renderer);
        canvas_vulkan_swapchain_destroy(renderer);
        canvas_vulkan_surface_destroy(renderer);

        renderer->started_is = false;
    }
}

void canvas_renderer_update(void* const renderer_)
{
    SPRX_ASSERT(NULL != renderer_, CNVX_RENDERER_ERROR_NULL("renderer"));

    CNVX_Renderer_PRIVATE* const renderer = renderer_;

    canvas_vulkan_frame_draw(renderer);
}

void canvas_renderer_resize(void* const renderer_, const size_t width_, const size_t height_)
{
    SPRX_ASSERT(NULL != renderer_, CNVX_RENDERER_ERROR_NULL("renderer"));

    CNVX_Renderer_PRIVATE* const renderer = renderer_;
}

void canvas_renderer_shader_load(void* const renderer_, const CNVX_Renderer_Shader_Type shader_type_, const char* const path_)
{
    SPRX_ASSERT(NULL != renderer_, CNVX_RENDERER_ERROR_NULL("renderer"));
    SPRX_ASSERT(___CNVX_RENDERER_SHADER_TYPE_MAX > shader_type_, CNVX_RENDERER_ERROR_ENUM("invalid value of shader type"));
    SPRX_ASSERT(NULL != path_, CNVX_RENDERER_ERROR_NULL("path"));

    CNVX_Renderer_PRIVATE* const renderer = renderer_;

    SPRX_ASSERT(!renderer->started_is, CNVX_RENDERER_ERROR_LOGIC("failed to load shader", "render must not be started", NULL));

    CNVX_NLOGF(renderer->logger, CNVX_LOGGER_LEVEL_TRACE, spore_string_substr(renderer->name, 7), "loading shader_%llu at %s", spore_vector_size(renderer->shader_vec), path_);

    void* file = spore_file_new();

    spore_file_open(file, path_, SPRX_FILE_MODE_READ, SPRX_FILE_FLAG_NONE);

    CNVX_Renderer_Shader_PRIVATE shader;
    shader.type = shader_type_;

    size_t size = 0;
    SPRX_ASSERT(SPRX_FILE_RESULT_SUCCESS == spore_file_size_get(file, &size), CNVX_RENDERER_ERROR_RUNTIME("failed to load shader", "could not get file size", NULL));
    shader.size = SPRX_MIN(size, UINT32_MAX);

    SPRX_ASSERT(SPRX_FILE_RESULT_SUCCESS == spore_file_mmap(file, &shader.data), CNVX_RENDERER_ERROR_RUNTIME("failed to load shader", "could not mmap file", NULL));

    SPRX_ASSERT(SPRX_FILE_RESULT_SUCCESS == spore_file_close(file), CNVX_RENDERER_ERROR_RUNTIME("failed to load shader", "could not close file", NULL));

    spore_file_delete(file);

    spore_vector_push_back(renderer->shader_vec, &shader);

    CNVX_NLOGF(renderer->logger, CNVX_LOGGER_LEVEL_DEBUG, spore_string_substr(renderer->name, 7), "successfully loaded shader_%llu", spore_vector_size(renderer->shader_vec) - 1);
}
