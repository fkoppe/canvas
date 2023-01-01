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

#include "cnvx/event/event.h"
#include "cnvx/event/handler.h"
#include "cnvx/logger/logger.h"
#include "cnvx/renderer/renderer.h"
#include "cnvx/window/window.h"

#include "sprx/container/string.h"
#include "sprx/core/assert.h"
#include "sprx/core/core.h"

#include "GLFW/glfw3.h"

#include <string.h>

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
    void* handler;
    void* logger;
    void* renderer;
    CNVX_Window_Settings settings;
    GLFWwindow* handle;
} CNVX_Window_PRIVATE;

void canvas_window_move_callback(GLFWwindow* const window_, const int x_, const int y_)
{
    CNVX_Window_PRIVATE* const window = glfwGetWindowUserPointer(window_);

    CNVX_Event event;
    event.type = CNVX_EVENT_TYPE_WINDOW_MOVED;
    event.window = window_;
    event.pos.x = x_;
    event.pos.y = y_;

    canvas_handler_push(window->handler, event);
}

void canvas_window_resize_callback(GLFWwindow* const window_, const int width_, const int height_)
{
    CNVX_Window_PRIVATE* const window = glfwGetWindowUserPointer(window_);

    CNVX_Event event;
    event.type = CNVX_EVENT_TYPE_WINDOW_RESIZED;
    event.window = window_;
    event.size.width = width_;
    event.size.height = height_;

    window->width = event.size.width;
    window->height = event.size.height;

    canvas_renderer_resize(window->renderer);

    canvas_handler_push(window->handler, event);
}

void canvas_window_close_callback(GLFWwindow* const window_)
{
    CNVX_Window_PRIVATE* const window = glfwGetWindowUserPointer(window_);

    CNVX_Event event;
    event.type = CNVX_EVENT_TYPE_WINDOW_CLOSED;
    event.window = window_;

    canvas_handler_push(window->handler, event);
}

void canvas_window_refresh_callback(GLFWwindow* const window_)
{
    CNVX_Window_PRIVATE* const window = glfwGetWindowUserPointer(window_);

    CNVX_Event event;
    event.type = CNVX_EVENT_TYPE_WINDOW_REFRESHED;
    event.window = window_;

    canvas_handler_push(window->handler, event);
}

void canvas_window_focus_callback(GLFWwindow* const window_, const int focused_)
{
    CNVX_Window_PRIVATE* const window = glfwGetWindowUserPointer(window_);

    CNVX_Event event;
    event.window = window_;

    if (focused_)
    {
        event.type = CNVX_EVENT_TYPE_WINDOW_FOCUSED;
    }
    else
    {
        event.type = CNVX_EVENT_TYPE_WINDOW_DEFOCUSED;
    }

    canvas_handler_push(window->handler, event);
}

void canvas_window_iconify_callback(GLFWwindow* const window_, const int iconified_)
{
    CNVX_Window_PRIVATE* const window = glfwGetWindowUserPointer(window_);

    CNVX_Event event;
    event.window = window_;

    if (iconified_)
    {
        event.type = CNVX_EVENT_TYPE_WINDOW_ICONIFIED;
    }
    else
    {
        event.type = CNVX_EVENT_TYPE_WINDOW_UNICONIFIED;
    }

    canvas_handler_push(window->handler, event);
}

void canvas_framebuffer_size_callback(GLFWwindow* const window_, const int width_, const int height_)
{
    CNVX_Window_PRIVATE* const window = glfwGetWindowUserPointer(window_);

    CNVX_Event event;
    event.type = CNVX_EVENT_TYPE_FRAMEBUFFER_RESIZED;
    event.window = window_;
    event.size.width = width_;
    event.size.height = height_;

    canvas_handler_push(window->handler, event);
}

void canvas_mouse_button_callback(GLFWwindow* const window_, const int button_, const int action_, const int mods_)
{
    CNVX_Window_PRIVATE* const window = glfwGetWindowUserPointer(window_);

    CNVX_Event event;
    event.window = window_;
    event.mouse.button = button_;
    event.mouse.mods = mods_;

    if (action_ == GLFW_PRESS)
    {
        event.type = CNVX_EVENT_TYPE_BUTTON_PRESSED;
    }
    else if (action_ == GLFW_RELEASE)
    {
        event.type = CNVX_EVENT_TYPE_BUTTON_RELEASED;
    }
    else
    {
        CNVX_WINDOW_ERROR_GLFW("button action out of range");
    }

    canvas_handler_push(window->handler, event);
}

void canvas_cursor_move_callback(GLFWwindow* const window_, const double x_, const double y_)
{
    CNVX_Window_PRIVATE* const window = glfwGetWindowUserPointer(window_);

    CNVX_Event event;
    event.type = CNVX_EVENT_TYPE_CURSOR_MOVED;
    event.window = window_;
    event.pos.x = (int)x_;
    event.pos.y = (int)y_;

    canvas_handler_push(window->handler, event);
}

void canvas_cursor_enter_callback(GLFWwindow* const window_, const int entered_)
{
    CNVX_Window_PRIVATE* const window = glfwGetWindowUserPointer(window_);

    CNVX_Event event;
    event.window = window_;

    if (entered_)
    {
        event.type = CNVX_EVENT_TYPE_CURSOR_ENTERED;
    }
    else
    {
        event.type = CNVX_EVENT_TYPE_CURSOR_LEFT;
    }

    canvas_handler_push(window->handler, event);
}

void canvas_scroll_callback(GLFWwindow* const window_, const double x_, const double y_)
{
    CNVX_Window_PRIVATE* const window = glfwGetWindowUserPointer(window_);
    
    CNVX_Event event;
    event.type = CNVX_EVENT_TYPE_SCROLLED;
    event.window = window_;
    event.pos.x = x_;
    event.pos.y = y_;

    canvas_handler_push(window->handler, event);
}

void canvas_window_key_callback(GLFWwindow* const window_, const int key_, const int scancode_, const int action_, const int mods_)
{
    CNVX_Window_PRIVATE* const window = glfwGetWindowUserPointer(window_);

    CNVX_Event event;
    event.window = window_;
    event.keyboard.key = key_;
    event.keyboard.scancode = scancode_;
    event.keyboard.mods = mods_;

    if (action_ == GLFW_PRESS)
    {
        event.type = CNVX_EVENT_TYPE_KEY_PRESSED;
    }
    else if(action_ == GLFW_RELEASE)
    {
        event.type = CNVX_EVENT_TYPE_KEY_RELEASED;
    }
    else if (action_ == GLFW_REPEAT)
    {
        event.type = CNVX_EVENT_TYPE_KEY_REPEATED;
    }
    else
    {
        CNVX_WINDOW_ERROR_GLFW("key action out of range");
    }

    canvas_handler_push(window->handler, event);
}

void canvas_char_callback(GLFWwindow* const window_, const unsigned int codepoint_)
{
    CNVX_Window_PRIVATE* const window = glfwGetWindowUserPointer(window_);

    CNVX_Event event;
    event.type = CNVX_EVENT_TYPE_CODEPOINT_INPUT;
    event.window = window_;
    event.codepoint = codepoint_;

    canvas_handler_push(window->handler, event);
}

void canvas_file_drop_callback(GLFWwindow* const window_, int count_, const char** const paths_)
{
    CNVX_Window_PRIVATE* const window = glfwGetWindowUserPointer(window_);

    CNVX_Event event;
    event.type = CNVX_EVENT_TYPE_FILE_DROPPED;
    event.window = window_;
    event.file.paths = malloc(count_ * sizeof(*event.file.paths));
    event.file.count = count_;

    while (count_--)
    {
        const size_t size = strlen(paths_[count_]) + 1;

        event.file.paths[count_] = malloc(size * sizeof(*event.file.paths));
        SPRX_ASSERT(NULL != event.file.paths[count_], CNVX_WINDOW_ERROR_ALLOCATION);

        memcpy(event.file.paths[count_], paths_[count_], size);
    }

    canvas_handler_push(window->handler, event);
}

void canvas_window_maximize_callback(GLFWwindow* const window_, const int maximized_)
{
    CNVX_Window_PRIVATE* const window = glfwGetWindowUserPointer(window_);

    CNVX_Event event;
    event.window = window_;

    if (maximized_)
    {
        event.type = CNVX_EVENT_TYPE_WINDOW_MAXIMIZED;
    }
    else
    {
        event.type = CNVX_EVENT_TYPE_WINDOW_UNMAXIMIZED;
    }

    canvas_handler_push(window->handler, event);
}

void canvas_window_content_scale_callback(GLFWwindow* const window_, const float xscale_, const float yscale_)
{
    CNVX_Window_PRIVATE* const window = glfwGetWindowUserPointer(window_);

    CNVX_Event event;
    event.type = CNVX_EVENT_TYPE_WINDOW_SCALE_CHANGED;
    event.window = window_;
    event.scale.x = xscale_;
    event.scale.y = yscale_;

    canvas_handler_push(window->handler, event);
}

void* canvas_window_new(const CNVX_Window_Settings settings_, const size_t unique_id_, void* const handler_, void* const logger_)
{
    SPRX_ASSERT(NULL != handler_, CNVX_WINDOW_ERROR_NULL("handler"));
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
    window->handler = handler_;
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
    SPRX_ASSERT(NULL != window_, CNVX_WINDOW_ERROR_NULL("window"));
    SPRX_ASSERT(width_ > 0, CNVX_WINDOW_ERROR_ARGUMENT("width has to be >0"));
    SPRX_ASSERT(height_ > 0, CNVX_WINDOW_ERROR_ARGUMENT("height has to be >0"));
    SPRX_ASSERT(NULL != renderer_, CNVX_WINDOW_ERROR_NULL("renderer"));

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

    window->width = width_;
    if (window->settings.limits.width)
    {
        window->width = SPRX_MIN(window->width, window->settings.limits.width);
    }

    window->height = height_;
    if (window->settings.limits.height)
    {
        window->height = SPRX_MIN(window->height, window->settings.limits.height);
    }

    window->renderer = renderer_;

    window->handle = glfwCreateWindow(window->width, window->height, window->title, NULL, NULL);

    glfwSetWindowUserPointer(window->handle, window);

    glfwSetWindowPosCallback(window->handle, canvas_window_move_callback);
    glfwSetWindowSizeCallback(window->handle, canvas_window_resize_callback);
    glfwSetWindowCloseCallback(window->handle, canvas_window_close_callback);
    glfwSetWindowRefreshCallback(window->handle, canvas_window_refresh_callback);
    glfwSetWindowFocusCallback(window->handle, canvas_window_focus_callback);
    glfwSetWindowIconifyCallback(window->handle, canvas_window_iconify_callback);
    glfwSetFramebufferSizeCallback(window->handle, canvas_framebuffer_size_callback);
    glfwSetMouseButtonCallback(window->handle, canvas_mouse_button_callback);
    glfwSetCursorPosCallback(window->handle, canvas_cursor_move_callback);
    glfwSetCursorEnterCallback(window->handle, canvas_cursor_enter_callback);
    glfwSetScrollCallback(window->handle, canvas_scroll_callback);
    glfwSetKeyCallback(window->handle, canvas_window_key_callback);
    glfwSetCharCallback(window->handle, canvas_char_callback);
    glfwSetDropCallback(window->handle, canvas_file_drop_callback);
    glfwSetWindowMaximizeCallback(window->handle, canvas_window_maximize_callback);
    glfwSetWindowContentScaleCallback(window->handle, canvas_window_content_scale_callback);

    CNVX_NLOG(window->logger, CNVX_LOGGER_LEVEL_TRACE, spore_string_substr(window->name, 7), "finish initialisation");
}

void canvas_window_close(void* const window_)
{
    SPRX_ASSERT(NULL != window_, CNVX_WINDOW_ERROR_NULL("window"));

    CNVX_Window_PRIVATE* const window = window_;

    CNVX_NLOG(window->logger, CNVX_LOGGER_LEVEL_TRACE, spore_string_substr(window->name, 7), "start shutdown");

    glfwDestroyWindow(window->handle);

    CNVX_NLOG(window->logger, CNVX_LOGGER_LEVEL_TRACE, spore_string_substr(window->name, 7), "finish shutdown");
    CNVX_NLOG(window->logger, CNVX_LOGGER_LEVEL_INFO, spore_string_substr(window->name, 7), "shutdown");

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
