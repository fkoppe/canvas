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

#include "cnvx/logger/color.h"
#include "cnvx/logger/logger.h"

#include "sprx/container/string.h"
#include "sprx/core/assert.h"
#include "sprx/core/digit.h"
#include "sprx/core/printf.h"
#include "sprx/core/sprintf.h"
#include "sprx/core/terminate.h"
#include "sprx/file/file.h"
#include "sprx/thread/mutex.h"

#include <time.h>

#define CNVX_LOGGER_ERROR_ALLOCATION SPRX_ERROR_ALLOCATION("logger", NULL, NULL)
#define CNVX_LOGGER_ERROR_LOGIC(what, info, care) SPRX_ERROR_LOGIC(what, "logger", info, care)
#define CNVX_LOGGER_ERROR_NULL(info) SPRX_ERROR_NULL("logger", info)
#define CNVX_LOGGER_ERROR_ENUM(info) SPRX_ERROR_ENUM("logger", info, NULL)

typedef struct CNVX_Logger_PRVATE
{
    const char* app_name;
    time_t now;
    bool recent_is;
    char* timestamp;
    void* string;
    void* mutex;
    void* output;
    void* output_mutex;
    CNVX_Logger_Settings settings;
} CNVX_Logger_PRVATE;

void canvas_logger_level_match_PRIVATE(const CNVX_Logger_Level level_, const char** const name_, const char** const color_)
{
    SPRX_ASSERT(___CNVX_LOGGER_LEVEL_MAX > level_, CNVX_LOGGER_ERROR_ENUM("invalid value of logger level"));
    SPRX_ASSERT(NULL != name_, CNVX_LOGGER_ERROR_NULL("name"));
    SPRX_ASSERT(NULL != color_, CNVX_LOGGER_ERROR_NULL("color"));

    switch (level_)
    {
    case CNVX_LOGGER_LEVEL_DEBUG:
        *name_ = "debug";
        *color_ = "";
        break;
    case CNVX_LOGGER_LEVEL_TRACE:
        *name_ = "trace";
        *color_ = CNVX_COLOR_FG_PURPLE;
        break;
    case CNVX_LOGGER_LEVEL_INFO:
        *name_ = "info";
        *color_ = CNVX_COLOR_FG_CYAN;
        break;
    case CNVX_LOGGER_LEVEL_RELEVANT:
        *name_ = "relevant";
        *color_ = CNVX_COLOR_FG_YELLOW;
        break;
    case CNVX_LOGGER_LEVEL_WARN:
        *name_ = "warn";
        *color_ = CNVX_COLOR_EXTENDED_FG_ORANGE;
        break;
    case CNVX_LOGGER_LEVEL_ERROR:
        *name_ = "error";
        *color_ = CNVX_COLOR_FG_RED;
        break;
    case CNVX_LOGGER_LEVEL_CRITICAL:
        *name_ = "critical";
        *color_ = CNVX_COLOR_BG_RED;
        break;
    default:
        SPRX_ABORT_ERROR(SPRX_ERROR_BOUNDS("logger", "invalid level", NULL));
        break;
    }
}

void canvas_logger_actor_match_PRIVATE(void* const logger_, const CNVX_Logger_Level actor_, const char** const name_, const char** const color_)
{
    SPRX_ASSERT(NULL != logger_, CNVX_LOGGER_ERROR_NULL("logger"));
    SPRX_ASSERT(___CNVX_LOGGER_ACTOR_MAX > actor_, CNVX_LOGGER_ERROR_ENUM("invalid value of logger actor"));
    SPRX_ASSERT(NULL != name_, CNVX_LOGGER_ERROR_NULL("name"));
    SPRX_ASSERT(NULL != color_, CNVX_LOGGER_ERROR_NULL("color"));

    switch (actor_)
    {
    case CNVX_LOGGER_ACTOR_CORE:
        *name_ = "core";
        *color_ = CNVX_COLOR_FG_GREEN;
        break;
    case CNVX_LOGGER_ACTOR_CLIENT:
        *name_ = "client";
        *color_ = CNVX_COLOR_FG_BLUE;
        break;
    default:
        SPRX_ABORT_ERROR(SPRX_ERROR_BOUNDS("logger", "invalid actor", NULL));
        break;
    }
}

void canvas_logger_timestamp_recent_make_PRIVATE(void* const logger_)
{
    SPRX_ASSERT(NULL != logger_, CNVX_LOGGER_ERROR_NULL("logger"));

    CNVX_Logger_PRVATE* const logger = logger_;

    if (!logger->recent_is)
    {
        time(&logger->now);

        const struct tm tm_now = *localtime(&logger->now);

        spore_digits_to_char_array(tm_now.tm_hour, 2, logger->timestamp + 0);
        spore_digits_to_char_array(tm_now.tm_min, 2, logger->timestamp + 3);
        spore_digits_to_char_array_terminated(tm_now.tm_sec, 2, logger->timestamp + 6);
    }
}

void* canvas_logger_new(const CNVX_Logger_Settings settings_, void* const output_, void* const mutex_, const char* const app_name_)
{
    SPRX_ASSERT(___CNVX_LOGGER_LEVEL_MAX > settings_.level_min, CNVX_LOGGER_ERROR_ENUM("invalid value of logger settings.level_min"));
    SPRX_ASSERT(NULL != mutex_, CNVX_LOGGER_ERROR_NULL("mutex"));
    SPRX_ASSERT(NULL != app_name_, CNVX_LOGGER_ERROR_NULL("app_name"));

    CNVX_Logger_PRVATE* const logger = malloc(sizeof(*logger));
    SPRX_ASSERT(NULL != logger, CNVX_LOGGER_ERROR_ALLOCATION);

    logger->app_name = app_name_;
    logger->now = 0;
    logger->recent_is = false;
    logger->string = spore_string_new_c(SPRX_SPRINTF_BUFFER_SIZE_MIN);
    logger->mutex = spore_mutex_new();
    logger->output = output_;
    logger->output_mutex = mutex_;
    logger->settings = settings_;

    logger->timestamp = malloc(CNVX_LOGGER_TIMESTAMP_TERMINATED_SIZE);
    SPRX_ASSERT(NULL != logger->timestamp, CNVX_LOGGER_ERROR_ALLOCATION);

    logger->timestamp[2] = ':';
    logger->timestamp[5] = ':';

    return logger;
}

void canvas_logger_delete(void* const logger_)
{
    SPRX_ASSERT(NULL != logger_, CNVX_LOGGER_ERROR_NULL("logger"));

    CNVX_Logger_PRVATE* const logger = logger_;

    spore_mutex_delete(logger->mutex);
    spore_string_delete(logger->string);

    free(logger);
}

void canvas_logger_timestamp_get(void* const logger_, char* destination_)
{
    SPRX_ASSERT(NULL != logger_, CNVX_LOGGER_ERROR_NULL("logger"));
    SPRX_ASSERT(NULL != destination_, CNVX_LOGGER_ERROR_NULL("destination"));

    CNVX_Logger_PRVATE* const logger = logger_;

    spore_mutex_lock(logger->mutex);

    canvas_logger_timestamp_recent_make_PRIVATE(logger);

    for (size_t i = 0; i < CNVX_LOGGER_TIMESTAMP_SIZE; i++)
    {
        destination_[i] = logger->timestamp[i];
    }

    spore_mutex_unlock(logger->mutex);
}

void canvas_logger_timestamp_terminated_get(void* const logger_, char* const destination_)
{
    SPRX_ASSERT(NULL != logger_, CNVX_LOGGER_ERROR_NULL("logger"));
    SPRX_ASSERT(NULL != destination_, CNVX_LOGGER_ERROR_NULL("destination"));

    CNVX_Logger_PRVATE* const logger = logger_;

    spore_mutex_lock(logger->mutex);

    canvas_logger_timestamp_recent_make_PRIVATE(logger);

    for (size_t i = 0; i < CNVX_LOGGER_TIMESTAMP_SIZE; i++)
    {
        destination_[i] = logger->timestamp[i];
    }

    destination_[CNVX_LOGGER_TIMESTAMP_SIZE] = '\0';

    spore_mutex_unlock(logger->mutex);
}

void canvas_logger_log(void* const logger_, const CNVX_Logger_Level level_, const CNVX_Logger_Actor actor_, const char* const message_)
{
    //logger is allowed to be =NULL

    SPRX_ASSERT(___CNVX_LOGGER_LEVEL_MAX > level_, CNVX_LOGGER_ERROR_ENUM("invalid value of logger level"));
    SPRX_ASSERT(___CNVX_LOGGER_ACTOR_MAX > actor_, CNVX_LOGGER_ERROR_ENUM("invalid value of logger actor"));
    SPRX_ASSERT(NULL != message_, CNVX_LOGGER_ERROR_NULL("message"));

    if (NULL == logger_)
    {
        return;
    }

    CNVX_Logger_PRVATE* const logger = logger_;

    spore_mutex_lock(logger->mutex);

    spore_string_clear(logger->string);

    if (!logger->settings.disabled_is)
    {
        if (level_ >= logger->settings.level_min)
        {
            if (logger->settings.apperance.timestamp_is)
            {
                canvas_logger_timestamp_recent_make_PRIVATE(logger);

                spore_string_append_f(logger->string, "[%s]", logger->timestamp);
            }

            if (logger->settings.apperance.thread_id_is)
            {
                if (logger->settings.apperance.thread_id_hex_is)
                {
                    spore_string_append_f(logger->string, "[0x%llx]", canvas_thread_id_current_get());
                }
                else
                {
                    spore_string_append_f(logger->string, "[%llu]", canvas_thread_id_current_get());
                }
            }

            const char* level_name = "";
            const char* level_color = "";
            const char* actor_name = "";
            const char* actor_color = "";

            canvas_logger_level_match_PRIVATE(level_, &level_name, &level_color);
            canvas_logger_actor_match_PRIVATE(logger, actor_, &actor_name, &actor_color);

#ifndef ___CNVX_NOCOLOR
            if (logger->settings.apperance.color_is)
            {
                spore_string_append_f(logger->string, "[%s%s%s][%s%s%s] %s\n", actor_color, actor_name, CNVX_COLOR_RESET, level_color, level_name, CNVX_COLOR_RESET, message_);
            }
            else
            {
                spore_string_append_f(logger->string, "[%s][%s] %s\n", actor_name, level_name, message_);
            }
#else
            spore_string_append_f(logger->string, "[%s][%s] %s\n", actor_name, level_name, message_);
#endif // !___CNVX_NOCOLOR

            if (NULL != logger->output)
            {
                if (spore_file_open_is(logger->output))
                {
                    spore_mutex_lock(logger->output_mutex);
                    spore_file_write_cstr(logger->output, canvas_string_cstr(logger->string));
                    spore_mutex_unlock(logger->output_mutex);
                }
                else
                {
                    SPRX_EXIT_ERROR(-1, CNVX_LOGGER_ERROR_LOGIC("failed to log", "output file not opened", NULL));
                }
            }
            else
            {
                spore_mutex_lock(logger->output_mutex);
                SPRX_PRINTF(canvas_string_cstr(logger->string));
                spore_mutex_unlock(logger->output_mutex);
            }
        }
    }

    spore_mutex_unlock(logger->mutex);
}

void canvas_logger_logf(void* const logger_, const CNVX_Logger_Level level_, const CNVX_Logger_Actor actor_, const char* const format_, ...)
{
    //logger is allowed to be =NULL

    SPRX_ASSERT(___CNVX_LOGGER_LEVEL_MAX > level_, CNVX_LOGGER_ERROR_ENUM("invalid value of logger level"));
    SPRX_ASSERT(___CNVX_LOGGER_ACTOR_MAX > actor_, CNVX_LOGGER_ERROR_ENUM("invalid value of logger actor"));
    SPRX_ASSERT(NULL != format_, CNVX_LOGGER_ERROR_NULL("format"));

    if (NULL == logger_)
    {
        return;
    }

    CNVX_Logger_PRVATE* const logger = logger_;

    spore_mutex_lock(logger->mutex);

    spore_string_clear(logger->string);

    if (!logger->settings.disabled_is)
    {
        if (level_ >= logger->settings.level_min)
        {
            if (logger->settings.apperance.timestamp_is)
            {
                canvas_logger_timestamp_recent_make_PRIVATE(logger);

                spore_string_append_f(logger->string, "[%s]", logger->timestamp);
            }

            if (logger->settings.apperance.thread_id_is)
            {
                if (logger->settings.apperance.thread_id_hex_is)
                {
                    spore_string_append_f(logger->string, "[0x%llx]", canvas_thread_id_current_get());
                }
                else
                {
                    spore_string_append_f(logger->string, "[%llu]", canvas_thread_id_current_get());
                }
            }

            const char* level_name = "";
            const char* level_color = "";
            const char* actor_name = "";
            const char* actor_color = "";

            canvas_logger_level_match_PRIVATE(level_, &level_name, &level_color);
            canvas_logger_actor_match_PRIVATE(logger, actor_, &actor_name, &actor_color);

#ifndef ___CNVX_NOCOLOR
            if (logger->settings.apperance.color_is)
            {
                spore_string_append_f(logger->string, "[%s%s%s][%s%s%s] ", actor_color, actor_name, CNVX_COLOR_RESET, level_color, level_name, CNVX_COLOR_RESET);
            }
            else
            {
                spore_string_append_f(logger->string, "[%s][%s] ", actor_name, level_name);
            }
#else
            spore_string_append_f(logger->string, "[%s][%s] ", actor_name, level_name);
#endif // !___CNVX_NOCOLOR

            va_list list;
            va_start(list, format_);

            spore_string_append_v(logger->string, format_, list);

            va_end(list);

            spore_string_push_back(logger->string, '\n');

            if (NULL != logger->output)
            {
                if (spore_file_open_is(logger->output))
                {
                    spore_mutex_lock(logger->output_mutex);
                    spore_file_write_cstr(logger->output, canvas_string_cstr(logger->string));
                    spore_mutex_unlock(logger->output_mutex);
                }
                else
                {
                    SPRX_EXIT_ERROR(-1, CNVX_LOGGER_ERROR_LOGIC("failed to log", "output file not opened", NULL));
                }
            }
            else
            {
                spore_mutex_lock(logger->output_mutex);
                SPRX_PRINTF(spore_string_cstr(logger->string));
                spore_mutex_unlock(logger->output_mutex);
            }
        }
    }

    spore_mutex_unlock(logger->mutex);
}

void canvas_logger_logv(void* const logger_, const CNVX_Logger_Level level_, const CNVX_Logger_Actor actor_, const char* const format_, const va_list args_)
{
    //logger is allowed to be =NULL

    SPRX_ASSERT(___CNVX_LOGGER_LEVEL_MAX > level_, CNVX_LOGGER_ERROR_ENUM("invalid value of logger level"));
    SPRX_ASSERT(___CNVX_LOGGER_ACTOR_MAX > actor_, CNVX_LOGGER_ERROR_ENUM("invalid value of logger actor"));
    SPRX_ASSERT(NULL != format_, CNVX_LOGGER_ERROR_NULL("format"));

    if (NULL == logger_)
    {
        return;
    }

    CNVX_Logger_PRVATE* const logger = logger_;

    spore_mutex_lock(logger->mutex);

    if (logger->settings.apperance.timestamp_is)
    {
        canvas_logger_timestamp_recent_make_PRIVATE(logger);

        spore_string_append_f(logger->string, "[%s]", logger->timestamp);
    }

    if (logger->settings.apperance.thread_id_is)
    {
        if (logger->settings.apperance.thread_id_hex_is)
        {
            spore_string_append_f(logger->string, "[0x%llx]", canvas_thread_id_current_get());
        }
        else
        {
            spore_string_append_f(logger->string, "[%llu]", canvas_thread_id_current_get());
        }
    }

    const char* level_name = "";
    const char* level_color = "";
    const char* actor_name = "";
    const char* actor_color = "";

    canvas_logger_level_match_PRIVATE(level_, &level_name, &level_color);
    canvas_logger_actor_match_PRIVATE(logger, actor_, &actor_name, &actor_color);

#ifndef ___CNVX_NOCOLOR
    if (logger->settings.apperance.color_is)
    {
        spore_string_append_f(logger->string, "[%s%s%s][%s%s%s] ", actor_color, actor_name, CNVX_COLOR_RESET, level_color, level_name, CNVX_COLOR_RESET);
    }
    else
    {
        spore_string_append_f(logger->string, "[%s][%s] ", actor_name, level_name);
    }
#else
    spore_string_append_f(logger->string, "[%s][%s] ", actor_name, level_name);
#endif // !___CNVX_NOCOLOR

    spore_string_append_v(logger->string, format_, args_);

    if (NULL != logger->output)
    {
        if (spore_file_open_is(logger->output))
        {
            spore_mutex_lock(logger->output_mutex);
            spore_file_write_cstr(logger->output, canvas_string_cstr(logger->string));
            spore_mutex_unlock(logger->output_mutex);
        }
        else
        {
            SPRX_EXIT_ERROR(-1, CNVX_LOGGER_ERROR_LOGIC("failed to log", "output file not opened", NULL));
        }
    }
    else
    {
        spore_mutex_lock(logger->output_mutex);
        SPRX_PRINTF(canvas_string_cstr(logger->string));
        spore_mutex_unlock(logger->output_mutex);
    }

    spore_mutex_unlock(logger->mutex);
}

void canvas_logger_nlog(void* const logger_, const CNVX_Logger_Level level_, const CNVX_Logger_Actor actor_, const char* const name_, const char* const message_)
{
    //logger is allowed to be =NULL

    SPRX_ASSERT(___CNVX_LOGGER_LEVEL_MAX > level_, CNVX_LOGGER_ERROR_ENUM("invalid value of logger level"));
    SPRX_ASSERT(___CNVX_LOGGER_ACTOR_MAX > actor_, CNVX_LOGGER_ERROR_ENUM("invalid value of logger actor"));
    SPRX_ASSERT(NULL != name_, CNVX_LOGGER_ERROR_NULL("name"));
    SPRX_ASSERT(NULL != message_, CNVX_LOGGER_ERROR_NULL("message"));

    if (NULL == logger_)
    {
        return;
    }

    CNVX_Logger_PRVATE* const logger = logger_;

    spore_mutex_lock(logger->mutex);

    spore_string_clear(logger->string);

    if (!logger->settings.disabled_is)
    {
        if (level_ >= logger->settings.level_min)
        {
            if (logger->settings.apperance.timestamp_is)
            {
                canvas_logger_timestamp_recent_make_PRIVATE(logger);

                spore_string_append_f(logger->string, "[%s]", logger->timestamp);
            }

            if (logger->settings.apperance.thread_id_is)
            {
                if (logger->settings.apperance.thread_id_hex_is)
                {
                    spore_string_append_f(logger->string, "[0x%llx]", canvas_thread_id_current_get());
                }
                else
                {
                    spore_string_append_f(logger->string, "[%llu]", canvas_thread_id_current_get());
                }
            }

            const char* level_name = "";
            const char* level_color = "";
            const char* actor_name = "";
            const char* actor_color = "";

            canvas_logger_level_match_PRIVATE(level_, &level_name, &level_color);
            canvas_logger_actor_match_PRIVATE(logger, actor_, &actor_name, &actor_color);

#ifndef ___CNVX_NOCOLOR
            if (logger->settings.apperance.color_is)
            {
                spore_string_append_f(logger->string, "[%s%s%s][%s%s%s][%s] %s\n", actor_color, actor_name, CNVX_COLOR_RESET, level_color, level_name, CNVX_COLOR_RESET, name_, message_);
            }
            else
            {
                spore_string_append_f(logger->string, "[%s][%s][%s] %s\n", actor_name, level_name, name_, message_);
            }
#else
            spore_string_append_f(logger->string, "[%s][%s][%s] %s\n", actor_name, level_name, name_, message_);
#endif // !___CNVX_NOCOLOR

            if (NULL != logger->output)
            {
                if (spore_file_open_is(logger->output))
                {
                    spore_mutex_lock(logger->output_mutex);
                    spore_file_write_cstr(logger->output, canvas_string_cstr(logger->string));
                    spore_mutex_unlock(logger->output_mutex);
                }
                else
                {
                    SPRX_EXIT_ERROR(-1, CNVX_LOGGER_ERROR_LOGIC("failed to log", "output file not opened", NULL));
                }
            }
            else
            {
                spore_mutex_lock(logger->output_mutex);
                SPRX_PRINTF(canvas_string_cstr(logger->string));
                spore_mutex_unlock(logger->output_mutex);
            }
        }
    }

    spore_mutex_unlock(logger->mutex);
}

void canvas_logger_nlogf(void* const logger_, const CNVX_Logger_Level level_, const CNVX_Logger_Actor actor_, const char* const name_, const char* const format_, ...)
{
    //logger is allowed to be =NULL

    SPRX_ASSERT(___CNVX_LOGGER_LEVEL_MAX > level_, CNVX_LOGGER_ERROR_ENUM("invalid value of logger level"));
    SPRX_ASSERT(___CNVX_LOGGER_ACTOR_MAX > actor_, CNVX_LOGGER_ERROR_ENUM("invalid value of logger actor"));
    SPRX_ASSERT(NULL != name_, CNVX_LOGGER_ERROR_NULL("name"));
    SPRX_ASSERT(NULL != format_, CNVX_LOGGER_ERROR_NULL("format"));

    if (NULL == logger_)
    {
        return;
    }

    CNVX_Logger_PRVATE* const logger = logger_;

    spore_mutex_lock(logger->mutex);

    spore_string_clear(logger->string);

    if (!logger->settings.disabled_is)
    {
        if (level_ >= logger->settings.level_min)
        {
            if (logger->settings.apperance.timestamp_is)
            {
                canvas_logger_timestamp_recent_make_PRIVATE(logger);

                spore_string_append_f(logger->string, "[%s]", logger->timestamp);
            }

            if (logger->settings.apperance.thread_id_is)
            {
                if (logger->settings.apperance.thread_id_hex_is)
                {
                    spore_string_append_f(logger->string, "[0x%llx]", canvas_thread_id_current_get());
                }
                else
                {
                    spore_string_append_f(logger->string, "[%llu]", canvas_thread_id_current_get());
                }
            }

            const char* level_name = "";
            const char* level_color = "";
            const char* actor_name = "";
            const char* actor_color = "";

            canvas_logger_level_match_PRIVATE(level_, &level_name, &level_color);
            canvas_logger_actor_match_PRIVATE(logger, actor_, &actor_name, &actor_color);

#ifndef ___CNVX_NOCOLOR
            if (logger->settings.apperance.color_is)
            {
                spore_string_append_f(logger->string, "[%s%s%s][%s%s%s][%s] ", actor_color, actor_name, CNVX_COLOR_RESET, level_color, level_name, CNVX_COLOR_RESET, name_);
            }
            else
            {
                spore_string_append_f(logger->string, "[%s][%s][%s] ", actor_name, level_name, name_);
            }
#else
            spore_string_append_f(logger->string, "[%s][%s][%s] ", actor_name, level_name, name_);
#endif // !___CNVX_NOCOLOR

            va_list list;
            va_start(list, format_);

            spore_string_append_v(logger->string, format_, list);

            va_end(list);

            spore_string_push_back(logger->string, '\n');

            if (NULL != logger->output)
            {
                if (spore_file_open_is(logger->output))
                {
                    spore_mutex_lock(logger->output_mutex);
                    spore_file_write_cstr(logger->output, canvas_string_cstr(logger->string));
                    spore_mutex_unlock(logger->output_mutex);
                }
                else
                {
                    SPRX_EXIT_ERROR(-1, CNVX_LOGGER_ERROR_LOGIC("failed to log", "output file not opened", NULL));
                }
            }
            else
            {
                spore_mutex_lock(logger->output_mutex);
                SPRX_PRINTF(canvas_string_cstr(logger->string));
                spore_mutex_unlock(logger->output_mutex);
            }
        }
    }

    spore_mutex_unlock(logger->mutex);
}

void canvas_logger_nlogv(void* const logger_, const CNVX_Logger_Level level_, const CNVX_Logger_Actor actor_, const char* const name_, const char* const format_, const va_list args_)
{
    //logger is allowed to be =NULL

    SPRX_ASSERT(___CNVX_LOGGER_LEVEL_MAX > level_, CNVX_LOGGER_ERROR_ENUM("invalid value of logger level"));
    SPRX_ASSERT(___CNVX_LOGGER_ACTOR_MAX > actor_, CNVX_LOGGER_ERROR_ENUM("invalid value of logger actor"));
    SPRX_ASSERT(NULL != name_, CNVX_LOGGER_ERROR_NULL("name"));
    SPRX_ASSERT(NULL != format_, CNVX_LOGGER_ERROR_NULL("format"));

    if (NULL == logger_)
    {
        return;
    }

    CNVX_Logger_PRVATE* const logger = logger_;

    spore_mutex_lock(logger->mutex);

    if (logger->settings.apperance.timestamp_is)
    {
        canvas_logger_timestamp_recent_make_PRIVATE(logger);

        spore_string_append_f(logger->string, "[%s]", logger->timestamp);
    }

    if (logger->settings.apperance.thread_id_is)
    {
        if (logger->settings.apperance.thread_id_hex_is)
        {
            spore_string_append_f(logger->string, "[0x%llx]", canvas_thread_id_current_get());
        }
        else
        {
            spore_string_append_f(logger->string, "[%llu]", canvas_thread_id_current_get());
        }
    }

    const char* level_name = "";
    const char* level_color = "";
    const char* actor_name = "";
    const char* actor_color = "";

    canvas_logger_level_match_PRIVATE(level_, &level_name, &level_color);
    canvas_logger_actor_match_PRIVATE(logger, actor_, &actor_name, &actor_color);

#ifndef ___CNVX_NOCOLOR
    if (logger->settings.apperance.color_is)
    {
        spore_string_append_f(logger->string, "[%s%s%s][%s%s%s][%s] ", actor_color, actor_name, CNVX_COLOR_RESET, level_color, level_name, CNVX_COLOR_RESET, name_);
    }
    else
    {
        spore_string_append_f(logger->string, "[%s][%s][%s] ", actor_name, level_name, name_);
    }
#else
    spore_string_append_f(logger->string, "[%s][%s][%s] ", actor_name, level_name, name_);
#endif // !___CNVX_NOCOLOR

    spore_string_append_v(logger->string, format_, args_);

    if (NULL != logger->output)
    {
        if (spore_file_open_is(logger->output))
        {
            spore_mutex_lock(logger->output_mutex);
            spore_file_write_cstr(logger->output, canvas_string_cstr(logger->string));
            spore_mutex_unlock(logger->output_mutex);
        }
        else
        {
            SPRX_EXIT_ERROR(-1, CNVX_LOGGER_ERROR_LOGIC("failed to log", "output file not opened", NULL));
        }
    }
    else
    {
        spore_mutex_lock(logger->output_mutex);
        SPRX_PRINTF(canvas_string_cstr(logger->string));
        spore_mutex_unlock(logger->output_mutex);
    }

    spore_mutex_unlock(logger->mutex);
}
