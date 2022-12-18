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

#ifndef ___CNVX___EVENT_H
#define ___CNVX___EVENT_H

#include "sprx/core/essentials.h"

//#include "../include/GLFW/glfw3.h"
#include "C:/Users/flixk/Desktop/Sandbox/canvas/library/glfw/include/GLFW/glfw3.h"

typedef enum CNVX_Key
{
    //CNVX_KEY_UNKNOWN        = GLFW_KEY_UNKNOWN,
    //CNVX_KEY_SPACE          = GLFW_KEY_SPACE,
    //CNVX_KEY_APOSTROPHE     = GLFW_KEY_APOSTROPHE,
    //CNVX_KEY_COMMA          = GLFW_KEY_COMMA,
    //CNVX_KEY_MINUS          = GLFW_KEY_MINUS,
    //CNVX_KEY_PERIOD         = GLFW_KEY_PERIOD,
    //CNVX_KEY_SLASH          = GLFW_KEY_SLASH,
    //CNVX_KEY_0              = GLFW_KEY_0,
    //CNVX_KEY_1              = GLFW_KEY_1,
    //CNVX_KEY_2              = GLFW_KEY_2,
    //CNVX_KEY_3              = GLFW_KEY_3,
    //CNVX_KEY_4              = GLFW_KEY_4,
    //CNVX_KEY_5              = GLFW_KEY_5,
    //CNVX_KEY_6              = GLFW_KEY_6,
    //CNVX_KEY_7              = GLFW_KEY_7,
    //CNVX_KEY_8              = GLFW_KEY_8,
    //CNVX_KEY_9              = GLFW_KEY_9,
    //CNVX_KEY_SEMICOLON      = GLFW_KEY_SEMICOLON,
    //CNVX_KEY_EQUAL          = GLFW_KEY_EQUAL,
    //CNVX_KEY_A              = GLFW_KEY_A,
    //CNVX_KEY_B              = GLFW_KEY_B,
    //CNVX_KEY_C              = GLFW_KEY_C,
    //CNVX_KEY_D              = GLFW_KEY_D,
    //CNVX_KEY_E              = GLFW_KEY_E,
    //CNVX_KEY_F              = GLFW_KEY_F,
    //CNVX_KEY_G              = GLFW_KEY_G,
    //CNVX_KEY_H              = GLFW_KEY_H,
    //CNVX_KEY_I              = GLFW_KEY_I,
    //CNVX_KEY_J              = GLFW_KEY_J,
    //CNVX_KEY_K              = GLFW_KEY_K,
    //CNVX_KEY_L              = GLFW_KEY_L,
    //CNVX_KEY_M              = GLFW_KEY_M,
    //CNVX_KEY_N              = GLFW_KEY_N,
    //CNVX_KEY_O              = GLFW_KEY_O,
    //CNVX_KEY_P              = GLFW_KEY_P,
    //CNVX_KEY_Q              = GLFW_KEY_Q,
    //CNVX_KEY_R              = GLFW_KEY_R,
    //CNVX_KEY_S              = GLFW_KEY_S,
    //CNVX_KEY_T              = GLFW_KEY_T,
    //CNVX_KEY_U              = GLFW_KEY_U,
    //CNVX_KEY_V              = GLFW_KEY_V,
    //CNVX_KEY_W              = GLFW_KEY_W,
    //CNVX_KEY_X              = GLFW_KEY_X,
    //CNVX_KEY_Y              = GLFW_KEY_Y,
    //CNVX_KEY_Z              = GLFW_KEY_Z,
    //CNVX_KEY_LEFT_BRACKET   = GLFW_KEY_LEFT_BRACKET,
    //CNVX_KEY_BACKSLASH      = GLFW_KEY_BACKSLASH,
    //CNVX_KEY_RIGHT_BRACKET  = GLFW_KEY_RIGHT_BRACKET,
    //CNVX_KEY_GRAVE_ACCENT   = GLFW_KEY_GRAVE_ACCENT,
    //CNVX_KEY_WORLD_1        = GLFW_KEY_WORLD_1,
    //CNVX_KEY_WORLD_2        = GLFW_KEY_WORLD_2,
    //CNVX_KEY_ESCAPE         = GLFW_KEY_ESCAPE,
    //CNVX_KEY_ENTER          = GLFW_KEY_ENTER,
    //CNVX_KEY_TAB            = GLFW_KEY_TAB,
    //CNVX_KEY_BACKSPACE      = GLFW_KEY_BACKSPACE,
    //CNVX_KEY_INSERT         = GLFW_KEY_INSERT,
    //CNVX_KEY_DELETE         = GLFW_KEY_DELETE,
    //CNVX_KEY_RIGHT          = GLFW_KEY_RIGHT,
    //CNVX_KEY_LEFT           = GLFW_KEY_LEFT,
    //CNVX_KEY_DOWN           = GLFW_KEY_DOWN,
    //CNVX_KEY_UP             = GLFW_KEY_UP,
    //CNVX_KEY_PAGE_UP        = GLFW_KEY_PAGE_UP,
    //CNVX_KEY_PAGE_DOWN      = GLFW_KEY_PAGE_DOWN,
    //CNVX_KEY_HOME           = GLFW_KEY_HOME,
    //CNVX_KEY_END            = GLFW_KEY_END,
    //CNVX_KEY_CAPS_LOCK      = GLFW_KEY_CAPS_LOCK,
    //CNVX_KEY_SCROLL_LOCK    = GLFW_KEY_SCROLL_LOCK,
    //CNVX_KEY_NUM_LOCK       = GLFW_KEY_NUM_LOCK,
    //CNVX_KEY_PRINT_SCREEN   = GLFW_KEY_PRINT_SCREEN,
    //CNVX_KEY_PAUSE          = GLFW_KEY_PAUSE,
    //CNVX_KEY_F1             = GLFW_KEY_F1,
    //CNVX_KEY_F2             = GLFW_KEY_F2,
    //CNVX_KEY_F3             = GLFW_KEY_F3,
    //CNVX_KEY_F4             = GLFW_KEY_F4,
    //CNVX_KEY_F5             = GLFW_KEY_F5,
    //CNVX_KEY_F6             = GLFW_KEY_F6,
    //CNVX_KEY_F7             = GLFW_KEY_F7,
    //CNVX_KEY_F8             = GLFW_KEY_F8,
    //CNVX_KEY_F9             = GLFW_KEY_F9,
    //CNVX_KEY_F10            = GLFW_KEY_F10,
    //CNVX_KEY_F11            = GLFW_KEY_F11,
    //CNVX_KEY_F12            = GLFW_KEY_F12,
    //CNVX_KEY_F13            = GLFW_KEY_F13,
    //CNVX_KEY_F14            = GLFW_KEY_F14,
    //CNVX_KEY_F15            = GLFW_KEY_F15,
    //CNVX_KEY_F16            = GLFW_KEY_F16,
    //CNVX_KEY_F17            = GLFW_KEY_F17,
    //CNVX_KEY_F18            = GLFW_KEY_F18,
    //CNVX_KEY_F19            = GLFW_KEY_F19,
    //CNVX_KEY_F20            = GLFW_KEY_F20,
    //CNVX_KEY_F21            = GLFW_KEY_F21,
    //CNVX_KEY_F22            = GLFW_KEY_F22,
    //CNVX_KEY_F23            = GLFW_KEY_F23,
    //CNVX_KEY_F24            = GLFW_KEY_F24,
    //CNVX_KEY_F25            = GLFW_KEY_F25,
    //CNVX_KEY_KP_0           = GLFW_KEY_KP_0,
    //CNVX_KEY_KP_1           = GLFW_KEY_KP_1,
    //CNVX_KEY_KP_2           = GLFW_KEY_KP_2,
    //CNVX_KEY_KP_3           = GLFW_KEY_KP_3,
    //CNVX_KEY_KP_4           = GLFW_KEY_KP_4,
    //CNVX_KEY_KP_5           = GLFW_KEY_KP_5,
    //CNVX_KEY_KP_6           = GLFW_KEY_KP_6,
    //CNVX_KEY_KP_7           = GLFW_KEY_KP_7,
    //CNVX_KEY_KP_8           = GLFW_KEY_KP_8,
    //CNVX_KEY_KP_9           = GLFW_KEY_KP_9,
    //CNVX_KEY_KP_DECIMAL     = GLFW_KEY_KP_DECIMAL,
    //CNVX_KEY_KP_DIVIDE      = GLFW_KEY_KP_DIVIDE,
    //CNVX_KEY_KP_MULTIPLY    = GLFW_KEY_KP_MULTIPLY,
    //CNVX_KEY_KP_SUBTRACT    = GLFW_KEY_KP_SUBTRACT,
    //CNVX_KEY_KP_ADD         = GLFW_KEY_KP_ADD,
    //CNVX_KEY_KP_ENTER       = GLFW_KEY_KP_ENTER,
    //CNVX_KEY_KP_EQUAL       = GLFW_KEY_KP_EQUAL,
    //CNVX_KEY_LEFT_SHIFT     = GLFW_KEY_LEFT_SHIFT,
    //CNVX_KEY_LEFT_CONTROL   = GLFW_KEY_LEFT_CONTROL,
    //CNVX_KEY_LEFT_ALT       = GLFW_KEY_LEFT_ALT,
    //CNVX_KEY_LEFT_SUPER     = GLFW_KEY_LEFT_SUPER,
    //CNVX_KEY_RIGHT_SHIFT    = GLFW_KEY_RIGHT_SHIFT,
    //CNVX_KEY_RIGHT_CONTROL  = GLFW_KEY_RIGHT_CONTROL,
    //CNVX_KEY_RIGHT_ALT      = GLFW_KEY_RIGHT_ALT,
    //CNVX_KEY_RIGHT_SUPER    = GLFW_KEY_RIGHT_SUPER,
    //CNVX_KEY_MENU           = GLFW_KEY_MENU,
    ___CNVX_KEY_MAX,
} CNVX_Key;

typedef enum CNVX_Event_Type
{
    CNVX_EVENT_TYPE_NONE,
    CNVX_EVENT_TYPE_WINDOW_MOVED,
    CNVX_EVENT_TYPE_WINDOW_RESIZED,
    CNVX_EVENT_TYPE_WINDOW_CLOSED,
    CNVX_EVENT_TYPE_WINDOW_REFRESHED,
    CNVX_EVENT_TYPE_WINDOW_FOCUSED,
    CNVX_EVENT_TYPE_WINDOW_DEFOCUSED,
    CNVX_EVENT_TYPE_WINDOW_ICONIFIED,
    CNVX_EVENT_TYPE_WINDOW_UNICONIFIED,
    CNVX_EVENT_TYPE_FRAMEBUFFER_RESIZED,
    CNVX_EVENT_TYPE_BUTTON_PRESSED,
    CNVX_EVENT_TYPE_BUTTON_RELEASED,
    CNVX_EVENT_TYPE_CURSOR_MOVED,
    CNVX_EVENT_TYPE_CURSOR_ENTERED,
    CNVX_EVENT_TYPE_CURSOR_LEFT,
    CNVX_EVENT_TYPE_SCROLLED,
    CNVX_EVENT_TYPE_KEY_PRESSED,
    CNVX_EVENT_TYPE_KEY_REPEATED,
    CNVX_EVENT_TYPE_KEY_RELEASED,
    CNVX_EVENT_TYPE_CODEPOINT_INPUT,
    CNVX_EVENT_TYPE_MONITOR_CONNECTED,
    CNVX_EVENT_TYPE_MONITOR_DISCONNECTED,
    CNVX_EVENT_TYPE_FILE_DROPPED,
    CNVX_EVENT_TYPE_JOYSTICK_CONNECTED,
    CNVX_EVENT_TYPE_JOYSTICK_DISCONNECTED,
    CNVX_EVENT_TYPE_WINDOW_MAXIMIZED,
    CNVX_EVENT_TYPE_WINDOW_UNMAXIMIZED,
    CNVX_EVENT_TYPE_WINDOW_SCALE_CHANGED,
    ___CNVX_EVENT_TYPE_MAX,

} CNVX_Event_Type;

typedef struct CNVX_Event
{
    CNVX_Event_Type type;
    union {
        GLFWwindow* window;
        GLFWmonitor* monitor;
        int joystick;
    };
    union {
        struct {
            int x;
            int y;
        } pos;
        struct {
            int width;
            int height;
        } size;
        struct {
            double x;
            double y;
        } scroll;
        struct {
            CNVX_Key key;
            int scancode;
            int mods;
        } keyboard;
        struct {
            int button;
            int mods;
        } mouse;
        unsigned int codepoint;
        struct {
            const char** paths;
            int count;
        } file;
        struct {
            float x;
            float y;
        } scale;
    };
} CNVX_Event;

#endif // ___CNVX___EVENT_H
