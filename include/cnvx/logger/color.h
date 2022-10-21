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

#ifndef ___CNVX___COLOR_H
#define ___CNVX___COLOR_H

#include "sprx/core/essentials.h"

#ifndef ___CNVX_NOCOLOR
static const char* const CNVX_COLOR_RESET = "\033[0m";
static const char* const CNVX_COLOR_FG_RED = "\033[31m";
static const char* const CNVX_COLOR_FG_GREEN = "\033[32m";
static const char* const CNVX_COLOR_FG_YELLOW = "\033[33m";
static const char* const CNVX_COLOR_FG_BLUE = "\033[34m";
static const char* const CNVX_COLOR_FG_PURPLE = "\033[35m";
static const char* const CNVX_COLOR_FG_CYAN = "\033[36m";
static const char* const CNVX_COLOR_BG_RED = "\033[41m";
static const char* const CNVX_COLOR_BG_GREEN = "\033[42m";
static const char* const CNVX_COLOR_BG_YELLOW = "\033[43m";
static const char* const CNVX_COLOR_BG_BLUE = "\033[44m";
static const char* const CNVX_COLOR_BG_PURPLE = "\033[45m";
static const char* const CNVX_COLOR_BG_CYAN = "\033[46m";

    #ifndef ___CNVX_NOEXTENDEDCOLOR
    static const char* const CNVX_COLOR_EXTENDED_FG_ORANGE = "\033[38;5;202m";
    #else
    static const char* const CNVX_COLOR_EXTENDED_FG_ORANGE = "";
    #endif // !___CNVX_NOEXTENDEDCOLOR
#else
static const char* const CNVX_COLOR_RESET = "";
static const char* const CNVX_COLOR_FG_RED = "";
static const char* const CNVX_COLOR_FG_GREEN = "";
static const char* const CNVX_COLOR_FG_YELLOW = "";
static const char* const CNVX_COLOR_FG_BLUE = "";
static const char* const CNVX_COLOR_FG_PURPLE = "";
static const char* const CNVX_COLOR_FG_CYAN = "";
static const char* const CNVX_COLOR_BG_RED = "";
static const char* const CNVX_COLOR_BG_GREEN = "";
static const char* const CNVX_COLOR_BG_YELLOW = "";
static const char* const CNVX_COLOR_BG_BLUE = "";
static const char* const CNVX_COLOR_BG_PURPLE = "";
static const char* const CNVX_COLOR_BG_CYAN = "";

static const char* const CNVX_COLOR_EXTENDED_FG_ORANGE = "";
#endif // !___CNVX_NOCOLOR

#endif // ___CNVX___COLOR_H
