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

#include <StormByte/visibility.h>

#include <string>

/**
 * @namespace StormByte
 * @brief Main namespace for the StormByte library.
 *
 * The `StormByte` namespace serves as the root for all components and utilities in the StormByte library.
 * It provides foundational classes and tools for building robust, thread-safe, and efficient applications.
 */
namespace StormByte {
	/**
	 * @brief Generate a RFC4122-compliant UUID version 4 string.
	 *
	 * See `uuid.cxx` for details and implementation notes about randomness
	 * source selection (OS CSPRNG preferred, fallback to PRNG).
	 *
	 * @return A 36-character lowercase UUID string in the form
	 *         "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx" where `y` is one of
	 *         8, 9, a, or b.
	 */
	STORMBYTE_PUBLIC std::string GenerateUUIDv4() noexcept;
}
