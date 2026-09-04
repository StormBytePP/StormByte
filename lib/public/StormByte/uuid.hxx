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
 * @brief Root namespace of the StormByte suite.
 */
namespace StormByte {
	/**
	 * @brief RFC 4122 UUID version 4 (lowercase).
	 * @return 36-character string `xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx` where `y` is `8`, `9`, `a` or `b`.
	 * @note Prefers the OS CSPRNG; falls back to a PRNG. Details in `uuid.cxx`.
	 */
	STORMBYTE_PUBLIC std::string GenerateUUIDv4() noexcept;
}
