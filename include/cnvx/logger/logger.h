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

#ifndef ___CNVX___LOGGER_H
#define ___CNVX___LOGGER_H

#include "sprx/core/essentials.h"
#include "sprx/core/module.h"

#define CNVX_LOGGER_TIMESTAMP_SIZE (sizeof(char) * 8)
#define CNVX_LOGGER_TIMESTAMP_TERMINATED_SIZE (sizeof(char) * 9)

#ifndef ___CNVX_NOLOG
#define CNVX_LOG(logger, level, message) canvas_logger_log(logger, level, canvas_logger_actor_get(SPRX_MODULE), message)
#define CNVX_LOGF(logger, level, format, ...) canvas_logger_logf(logger, level, canvas_logger_actor_get(SPRX_MODULE), format, __VA_ARGS__)
#define CNVX_LOGV(logger, level, format, args) canvas_logger_logf(logger, level, canvas_logger_actor_get(SPRX_MODULE), format, args)
#define CNVX_NLOG(logger, level, name, message) canvas_logger_nlog(logger, level, canvas_logger_actor_get(SPRX_MODULE), name, message)
#define CNVX_NLOGF(logger, level, name, format, ...) canvas_logger_nlogf(logger, level, canvas_logger_actor_get(SPRX_MODULE), name, format, __VA_ARGS__)
#define CNVX_NLOGV(logger, level, name, format, args) canvas_logger_nlogf(logger, level, canvas_logger_actor_get(SPRX_MODULE), name, format, args)
#else
#define CNVX_LOG(logger, level, message)
#define CNVX_LOGF(logger, level, format, ...)
#define CNVX_LOGV(logger, level, format, args)
#define CNVX_NLOG(logger, level, name, message)
#define CNVX_NLOGF(logger, level, name, format, ...)
#define CNVX_NLOGV(logger, level, name, format, args)
#endif // !___CNVX_NOLOG

typedef enum CNVX_Logger_Level
{
    CNVX_LOGGER_LEVEL_DEBUG,
    CNVX_LOGGER_LEVEL_TRACE,
    CNVX_LOGGER_LEVEL_INFO,
    CNVX_LOGGER_LEVEL_RELEVANT,
    CNVX_LOGGER_LEVEL_WARN,
    CNVX_LOGGER_LEVEL_ERROR,
    CNVX_LOGGER_LEVEL_CRITICAL,
    ___CNVX_LOGGER_LEVEL_MAX,
} CNVX_Logger_Level;

typedef enum CNVX_Logger_Actor
{
    CNVX_LOGGER_ACTOR_CORE,
    CNVX_LOGGER_ACTOR_CLIENT,
    ___CNVX_LOGGER_ACTOR_MAX,
} CNVX_Logger_Actor;

typedef struct CNVX_Logger_Settings
{
    bool disabled_is;
    CNVX_Logger_Level level_min;
    struct
    {
        bool timestamp_is;
        bool thread_id_is;
        bool thread_id_hex_is;
        bool color_is;
    } apperance;
} CNVX_Logger_Settings;

void* canvas_logger_new(const CNVX_Logger_Settings settings, void* const output, void* const mutex, const char* const app_name);
void canvas_logger_delete(void* const logger);

void canvas_logger_timestamp_get(void* const logger, char* destination);
void canvas_logger_timestamp_terminated_get(void* const logger, char* const destination);

void canvas_logger_log(void* const logger, const CNVX_Logger_Level level, const CNVX_Logger_Actor actor, const char* const message);
void canvas_logger_logf(void* const logger, const CNVX_Logger_Level level, const CNVX_Logger_Actor actor, const char* const format, ...);
void canvas_logger_logv(void* const logger, const CNVX_Logger_Level level, const CNVX_Logger_Actor actor, const char* const format, const va_list args);
void canvas_logger_nlog(void* const logger, const CNVX_Logger_Level level, const CNVX_Logger_Actor actor, const char* const name, const char* const message);
void canvas_logger_nlogf(void* const logger, const CNVX_Logger_Level level, const CNVX_Logger_Actor actor, const char* const name, const char* const format, ...);
void canvas_logger_nlogv(void* const logger, const CNVX_Logger_Level level, const CNVX_Logger_Actor actor, const char* const name, const char* const format, const va_list args);

CNVX_Logger_Actor canvas_logger_actor_get(const SPRX_Module* const module);

#endif // ___CNVX___LOGGER_H
