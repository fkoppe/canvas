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

#ifndef ___CNVX___HANDLER_H
#define ___CNVX___HANDLER_H

#include "sprx/core/essentials.h"

#include "cnvx/event/event.h"

void* canvas_handler_new(const size_t growth);
void canvas_handler_delete(void* const handler);

void canvas_handler_push(void* const handler, const CNVX_Event event);
CNVX_Event* canvas_handler_next(void* const handler);
void canvas_handler_reset(void* const handler);

#endif // ___CNVX___HANDLER_H
