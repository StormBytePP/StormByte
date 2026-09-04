/*
* Copyright (C) 2024-2026 David C. Manuelda (StormBytePP)
*
* This file is part of StormByte.
*
* StormByte is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License version 3
* or later, as published by the Free Software Foundation.
*
* StormByte is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with StormByte. If not, see
* <https://www.gnu.org/licenses/lgpl-3.0.html>.
*/

#pragma once

#include <StormByte/platform.h>

/**
 * @def STORMBYTE_PUBLIC
 * @brief Export or import the symbol (`dllexport`/`dllimport` on Windows, default visibility elsewhere).
 */
/**
 * @def STORMBYTE_PRIVATE
 * @brief Hide the symbol (empty on Windows, hidden visibility elsewhere).
 */
#ifdef WINDOWS
	#ifdef StormByte_EXPORTS
		#define STORMBYTE_PUBLIC	__declspec(dllexport)
	#else
		#define STORMBYTE_PUBLIC	__declspec(dllimport)
	#endif
	#define STORMBYTE_PRIVATE
#else
	#define STORMBYTE_PUBLIC		__attribute__ ((visibility ("default")))
	#define STORMBYTE_PRIVATE		__attribute__ ((visibility ("hidden")))
#endif
