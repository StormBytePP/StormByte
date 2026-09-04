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

#include <chrono>
#include <filesystem>
#include <string>

/**
 * @namespace StormByte
 * @brief Main namespace for the StormByte library.
 *
 * The `StormByte` namespace serves as the root for all components and utilities in the StormByte library.
 * It provides foundational classes and tools for building robust, thread-safe, and efficient applications.
 */
namespace StormByte::System {
	/**
	 * @brief Safely gets a temporary file name.
	 * @param prefix The prefix for the temporary file name. Defaults to "TMP".
	 * @return The full path of the temporary file.
	 */
	STORMBYTE_PUBLIC std::filesystem::path TempFileName(const std::string& prefix = "TMP");

	/**
	 * @brief Gets the current working directory.
	 * @return The current working directory path.
	 */
	STORMBYTE_PUBLIC std::filesystem::path CurrentPath();

	/**
	 * @brief Gets the directory that contains the running executable.
	 * @return The directory path of the executable, or "NOPATH" on failure.
	 */
	STORMBYTE_PUBLIC std::filesystem::path ExecutablePath();

	/**
	 * @brief Sleeps for a specific duration.
	 * @tparam Rep The representation of the duration.
	 * @tparam Period The period of the duration.
	 * @param duration The time to sleep, specified as a `std::chrono::duration`.
	 */
	template <typename Rep, typename Period>
	STORMBYTE_PUBLIC void Sleep(const std::chrono::duration<Rep, Period>& duration);
}
