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
 * @brief Root namespace of the StormByte suite.
 */
/**
 * @namespace StormByte::System
 * @brief Path and timing helpers that hide OS differences.
 */
namespace StormByte::System {
	/**
	 * @brief Builds a unique temporary file path.
	 * @param prefix File-name prefix. Default `"TMP"`.
	 * @return Absolute path of the temporary file.
	 */
	STORMBYTE_PUBLIC std::filesystem::path TempFileName(const std::string& prefix = "TMP");

	/**
	 * @brief Current working directory.
	 * @return Absolute path of the process cwd.
	 */
	STORMBYTE_PUBLIC std::filesystem::path CurrentPath();

	/**
	 * @brief Directory that contains the running executable.
	 * @return That directory, or the path `"NOPATH"` if it cannot be resolved.
	 */
	STORMBYTE_PUBLIC std::filesystem::path ExecutablePath();

	/**
	 * @brief Suspends the calling thread.
	 * @tparam Rep Duration representation.
	 * @tparam Period Duration period.
	 * @param duration Sleep interval.
	 */
	template <typename Rep, typename Period>
	STORMBYTE_PUBLIC void Sleep(const std::chrono::duration<Rep, Period>& duration);
}
