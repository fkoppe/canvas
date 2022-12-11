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

#include "cnvx/event/handler.h"

#include "sprx/container/vector.h"
#include "sprx/core/assert.h"
#include "sprx/core/core.h"

#define CNVX_HANDLER_ERROR_ALLOCATION SPRX_ERROR_ALLOCATION("handler", NULL, NULL)
//#define CNVX_WINDOW_ERROR_LOGIC(what, info, care) SPRX_ERROR_LOGIC(what, "window", info, care)
//#define CNVX_WINDOW_ERROR_ARGUMENT(care) SPRX_ERROR_ARGUMENT("window", NULL, care)
#define CNVX_HANDLER_ERROR_NULL(info) SPRX_ERROR_NULL("handler", info)
//#define CNVX_WINDOW_ERROR_ENUM(info) SPRX_ERROR_ENUM("window", info, NULL)
//#define CNVX_WINDOW_ERROR_GLFW(info) SPRX_ERROR_API("glfw", "window", info, NULL)

typedef struct CNVX_Handler_PRIVATE
{
	size_t growth;
	void* vec_events;
	size_t popped;
} CNVX_Handler_PRIVATE;

void* canvas_handler_new(const size_t growth_)
{
	CNVX_Handler_PRIVATE* const handler = malloc(sizeof(*handler));
	SPRX_ASSERT(NULL != handler, CNVX_HANDLER_ERROR_ALLOCATION);

	handler->growth = growth_;
	handler->vec_events = spore_vector_new_c(sizeof(CNVX_Event), handler->growth);
	handler->popped = 0;

	return handler;
}

void canvas_handler_delete(void* const handler_)
{
	SPRX_ASSERT(NULL != handler_, CNVX_HANDLER_ERROR_NULL("handler"));

	CNVX_Handler_PRIVATE* const handler = handler_;

	spore_vector_delete(handler->vec_events);

	free(handler_);
}

void canvas_handler_push(void* const handler_, const CNVX_Event event_)
{
	SPRX_ASSERT(NULL != handler_, CNVX_HANDLER_ERROR_NULL("handler"));

	CNVX_Handler_PRIVATE* const handler = handler_;

	spore_vector_push_back_grow(handler->vec_events, handler->growth, &event_);
}

CNVX_Event* canvas_handler_next(void* const handler_)
{
	SPRX_ASSERT(NULL != handler_, CNVX_HANDLER_ERROR_NULL("handler"));

	CNVX_Handler_PRIVATE* const handler = handler_;

	if (spore_vector_capacity(handler->vec_events) > handler->popped)
	{
		handler->popped++;

		return SPRX_VECTOR_AT(handler->vec_events, handler->popped - 1, CNVX_Event);
	}

	return NULL;
}

void canvas_handler_reset(void* const handler_)
{
	SPRX_ASSERT(NULL != handler_, CNVX_HANDLER_ERROR_NULL("handler"));

	CNVX_Handler_PRIVATE* const handler = handler_;

	spore_vector_clear_reserve(handler->vec_events, handler->growth);
}