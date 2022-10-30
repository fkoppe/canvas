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
#include "cnvx/window/window.h"

#include "sprx/container/string.h"
#include "sprx/core/assert.h"
#include "sprx/core/core.h"

#include "GLFW/glfw3.h"

#define CNVX_WINDOW_ERROR_ALLOCATION SPRX_ERROR_ALLOCATION("window", NULL, NULL)
#define CNVX_WINDOW_ERROR_LOGIC(what, info, care) SPRX_ERROR_LOGIC(what, "window", info, care)
#define CNVX_WINDOW_ERROR_ARGUMENT(care) SPRX_ERROR_ARGUMENT("window", NULL, care)
#define CNVX_WINDOW_ERROR_NULL(info) SPRX_ERROR_NULL("window", info)
#define CNVX_WINDOW_ERROR_ENUM(info) SPRX_ERROR_ENUM("window", info, NULL)
#define CNVX_WINDOW_ERROR_GLFW(info) SPRX_ERROR_API("glfw", "window", info, NULL)

typedef struct CNVX_Window_PRIVATE
{
    size_t unique_id;
    void* name;
    const char* title;
    bool visible_is;
    size_t width;
    size_t height;
    size_t x;
    size_t y;
    void* logger;
    void* renderer;
    CNVX_Window_Settings settings;
    GLFWwindow* handle;
} CNVX_Window_PRIVATE;

void* canvas_window_new(const CNVX_Window_Settings settings_, const size_t unique_id_, void* const logger_)
{
    SPRX_ASSERT(___CNVX_WINDOW_POSITION_MAX > settings_.position, CNVX_WINDOW_ERROR_ENUM("invalid value of window position"));

    CNVX_Window_PRIVATE* const window = malloc(sizeof(*window));
    SPRX_ASSERT(NULL != window, CNVX_WINDOW_ERROR_ALLOCATION);

    window->unique_id = unique_id_;
    window->name = spore_string_new_f("canvas_window_%llu", window->unique_id);
    window->title = spore_string_cstr(window->name);
    window->visible_is = false;
    window->width = 0;
    window->height = 0;
    window->x = 0;
    window->y = 0;
    window->logger = logger_;
    window->renderer = NULL;
    window->settings = settings_;
    window->handle = NULL;

    SPRX_ASSERT(GLFW_TRUE == glfwInit(), CNVX_WINDOW_ERROR_GLFW("glfwInit failed"));

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    return window;
}

void canvas_window_delete(void* const window_)
{
    SPRX_ASSERT(NULL != window_, CNVX_WINDOW_ERROR_NULL("window"));

    CNVX_Window_PRIVATE* const window = window_;

    glfwTerminate();

    spore_string_delete(window->name);

    free(window);
}

CNVX_Window_Settings canvas_window_settings_get(void* const window_)
{
    SPRX_ASSERT(NULL != window_, CNVX_WINDOW_ERROR_NULL("window"));

    CNVX_Window_PRIVATE* const window = window_;

    return window->settings;
}

void canvas_window_settings_set(void* const window_, const CNVX_Window_Settings settings_)
{
    SPRX_ASSERT(NULL != window_, CNVX_WINDOW_ERROR_NULL("window"));
    SPRX_ASSERT(___CNVX_WINDOW_POSITION_MAX > settings_.position, CNVX_WINDOW_ERROR_ENUM("invalid value of window position"));

    CNVX_Window_PRIVATE* const window = window_;

    if (settings_.position == CNVX_WINDOW_POSITION_CUSTOM)
    {
        if (window->x != settings_.custom.position.x || window->y != settings_.custom.position.y)
        {
            canvas_window_reposition(window, settings_.custom.position.x, settings_.custom.position.y);
        }
    }

    window->settings = settings_;
}

void canvas_window_open(void* const window_, const char* const title_, const size_t width_, const size_t height_, bool visible_is_, void* const renderer_)
{
    //title and renderer are allowed to be =NULL

    SPRX_ASSERT(NULL != window_, CNVX_WINDOW_ERROR_NULL("window"));
    SPRX_ASSERT(width_ > 0, CNVX_WINDOW_ERROR_ARGUMENT("width has to be >0"));
    SPRX_ASSERT(height_ > 0, CNVX_WINDOW_ERROR_ARGUMENT("height has to be >0"));

    CNVX_Window_PRIVATE* const window = window_;

    CNVX_NLOG(window->logger, CNVX_LOGGER_LEVEL_INFO, spore_string_substr(window->name, 7), "initialisation");
    CNVX_NLOG(window->logger, CNVX_LOGGER_LEVEL_TRACE, spore_string_substr(window->name, 7), "start initialisation");

    if (NULL != title_)
    {
        window->title = title_;
    }
    else
    {
        window->title = spore_string_cstr(window->name);
    }

    window->visible_is = visible_is_;

    if (window->settings.limits.width)
    {
        window->width = SPRX_MIN(width_, window->settings.limits.width);
    }

    if (window->settings.limits.width)
    {
        window->height = SPRX_MIN(height_, window->settings.limits.height);
    }

    window->renderer = renderer_;

    window->handle = glfwCreateWindow(window->width, window->height, window->title, NULL, NULL);

    CNVX_NLOG(window->logger, CNVX_LOGGER_LEVEL_TRACE, spore_string_substr(window->name, 7), "finish initialisation");
}

void canvas_window_close(void* const window_)
{
    SPRX_ASSERT(NULL != window_, CNVX_WINDOW_ERROR_NULL("window"));

    CNVX_Window_PRIVATE* const window = window_;

    CNVX_NLOG(window->logger, CNVX_LOGGER_LEVEL_TRACE, spore_string_substr(window->name, 7), "start shutdown");

    glfwDestroyWindow(window->handle);

    CNVX_NLOG(window->logger,CNVX_LOGGER_LEVEL_TRACE, spore_string_substr(window->name, 7), "finish shutdown");
    CNVX_NLOG(window->logger,CNVX_LOGGER_LEVEL_INFO, spore_string_substr(window->name, 7), "shutdown");

    window->handle = NULL;
}

bool canvas_window_open_is(void* const window_)
{
    SPRX_ASSERT(NULL != window_, CNVX_WINDOW_ERROR_NULL("window"));

    CNVX_Window_PRIVATE* const window = window_;

    return window->handle;
}

void canvas_window_update(void* const window_)
{
    SPRX_ASSERT(NULL != window_, CNVX_WINDOW_ERROR_NULL("window"));

    CNVX_Window_PRIVATE* const window = window_;

    glfwPollEvents();
}

void canvas_window_hide(void* const window_)
{
    SPRX_ASSERT(NULL != window_, CNVX_WINDOW_ERROR_NULL("window"));

    CNVX_Window_PRIVATE* const window = window_;

    glfwHideWindow(window->handle);
}

void canvas_window_show(void* const window_)
{
    SPRX_ASSERT(NULL != window_, CNVX_WINDOW_ERROR_NULL("window"));

    CNVX_Window_PRIVATE* const window = window_;

    glfwShowWindow(window->handle);
}

bool canvas_window_visible_is(void* const window_)
{
    SPRX_ASSERT(NULL != window_, CNVX_WINDOW_ERROR_NULL("window"));

    CNVX_Window_PRIVATE* const window = window_;

    return glfwGetWindowAttrib(window->handle, GLFW_VISIBLE);
}

void canvas_window_size_get(void* const window_, size_t* const width_dest_, size_t* const height_dest_)
{
    SPRX_ASSERT(NULL != window_, CNVX_WINDOW_ERROR_NULL("window"));

    CNVX_Window_PRIVATE* const window = window_;

    if (NULL != width_dest_)
    {
        *width_dest_ = window->width;
    }

    if (NULL != height_dest_)
    {
        *height_dest_ = window->height;
    }
}

void canvas_window_resize(void* const window_, const size_t width_, const size_t height_)
{
    SPRX_ASSERT(NULL != window_, CNVX_WINDOW_ERROR_NULL("window"));

    CNVX_Window_PRIVATE* const window = window_;

    if (window->settings.limits.width)
    {
        window->width = SPRX_MIN(width_, window->settings.limits.width);
    }

    if (window->settings.limits.width)
    {
        window->height = SPRX_MIN(height_, window->settings.limits.height);
    }

    glfwSetWindowSize(window->handle, window->width, window->height);
}

void canvas_window_framebuffer_size_get(void* const window_, size_t* const width_dest_, size_t* const height_dest_)
{
    SPRX_ASSERT(NULL != window_, CNVX_WINDOW_ERROR_NULL("window"));

    CNVX_Window_PRIVATE* const window = window_;

    int w = 0;
    int h = 0;
    glfwGetFramebufferSize(window->handle, &w, &h);

    if (NULL != width_dest_)
    {
        *width_dest_ = abs(w);
    }

    if (NULL != height_dest_)
    {
        *height_dest_ = abs(h);
    }
}

void canvas_window_position_get(void* const window_, size_t* const x_dest_, size_t* const y_dest_)
{
    SPRX_ASSERT(NULL != window_, CNVX_WINDOW_ERROR_NULL("window"));

    CNVX_Window_PRIVATE* const window = window_;

    if (NULL != x_dest_)
    {
        *x_dest_ = window->x;
    }

    if (NULL != y_dest_)
    {
        *y_dest_ = window->y;
    }
}

void canvas_window_reposition(void* const window_, const size_t x_, const size_t y_)
{
    SPRX_ASSERT(NULL != window_, CNVX_WINDOW_ERROR_NULL("window"));

    CNVX_Window_PRIVATE* const window = window_;

    window->x = x_;
    window->y = y_;

    glfwSetWindowPos(window->handle, SPRX_MIN(INT32_MAX, window->x), SPRX_MIN(INT32_MAX, window->y));
}

void canvas_window_title_set(void* const window_, const char* const title_)
{
    SPRX_ASSERT(NULL != window_, CNVX_WINDOW_ERROR_NULL("window"));

    CNVX_Window_PRIVATE* const window = window_;

    if (NULL != title_)
    {
        window->title = title_;
    }
    else
    {
        window->title = spore_string_cstr(window->name);
    }

    glfwSetWindowTitle(window->handle, window->title);
}

void* canvas_window_handle_get(void* const window_)
{
    SPRX_ASSERT(NULL != window_, CNVX_WINDOW_ERROR_NULL("window"));

    CNVX_Window_PRIVATE* const window = window_;

    return window->handle;
}
