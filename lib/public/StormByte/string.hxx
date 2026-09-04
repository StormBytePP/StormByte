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

#include <StormByte/exception.hxx>
#include <StormByte/expected.hxx>
#include <StormByte/visibility.h>

#include <algorithm>
#include <cstddef>
#include <cmath>
#include <iomanip>
#include <locale>
#include <queue>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

/**
 * @namespace StormByte
 * @brief Root namespace of the StormByte suite.
 */
/**
 * @namespace StormByte::String
 * @brief String helpers (case, split, UTF-8, human-readable numbers).
 */
namespace StormByte::String {
	/**
	 * @enum Format
	 * @brief How `HumanReadable` prints a number.
	 */
	enum class STORMBYTE_PUBLIC Format : unsigned short {
		Raw,						///< Unscaled decimal.
		HumanReadableNumber,		///< Grouped / locale number.
		HumanReadableBytes			///< Byte units (KiB, MiB, …).
	};

	/**
	 * @brief `level` tab characters, or empty when `level == 0`.
	 * @param level Indentation depth.
	 */
	constexpr STORMBYTE_PUBLIC std::string Indent(const int& level) noexcept {
		return level == 0 ? std::string() : std::string(level, '\t');
	}

	/**
	 * @brief `true` when every character is a decimal digit (`::isdigit`).
	 * @param str String to test.
	 * @note Empty string is not numeric.
	 */
	constexpr STORMBYTE_PUBLIC bool IsNumeric(const std::string& str) noexcept {
		return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
	}

	/**
	 * @brief Lowercase copy.
	 * @param str Input.
	 */
	STORMBYTE_PUBLIC std::string ToLower(const std::string& str) noexcept;

	/**
	 * @brief Uppercase copy.
	 * @param str Input.
	 */
	STORMBYTE_PUBLIC std::string ToUpper(const std::string& str) noexcept;

	/**
	 * @brief Splits on @p delimiter into a queue (empty tokens kept).
	 * @param str Input.
	 * @param delimiter Separator.
	 */
	STORMBYTE_PUBLIC std::queue<std::string> Explode(const std::string& str, const char delimiter);

	/**
	 * @brief Splits on spaces into a vector.
	 * @param str Input.
	 */
	STORMBYTE_PUBLIC std::vector<std::string> Split(const std::string& str);

	/**
	 * @brief Formats an arithmetic value (not `wchar_t`).
	 * @tparam T Arithmetic type.
	 * @param number Value.
	 * @param format `Raw`, `HumanReadableNumber` or `HumanReadableBytes`.
	 * @param locale Locale name, default `"en_US.UTF-8"`.
	 */
	template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> && !std::is_same_v<T, wchar_t>>>
	STORMBYTE_PUBLIC std::string HumanReadable(const T& number, const Format& format, const std::string& locale = "en_US.UTF-8") noexcept;

	/**
	 * @brief Wide string to UTF-8.
	 * @param ws Wide input.
	 * @throws std::runtime_error On conversion failure.
	 */
	STORMBYTE_PUBLIC std::string UTF8Encode(const std::wstring& ws);

	/**
	 * @brief UTF-8 to wide string.
	 * @param s UTF-8 input.
	 * @throws std::runtime_error On conversion failure.
	 */
	STORMBYTE_PUBLIC std::wstring UTF8Decode(const std::string& s);

	/**
	 * @brief Normalizes `\n` / `\r\n` to a single newline style.
	 * @param str Input.
	 */
	STORMBYTE_PUBLIC std::string SanitizeNewlines(const std::string& str) noexcept;

	/**
	 * @brief Interprets bytes as `char` and builds a string.
	 * @param byte_vector Input bytes.
	 */
	STORMBYTE_PUBLIC std::string FromByteVector(const std::vector<std::byte>& byte_vector) noexcept;

	/**
	 * @brief Copies string bytes into a `vector<std::byte>`.
	 * @param str Input.
	 */
	STORMBYTE_PUBLIC std::vector<std::byte> ToByteVector(const std::string& str) noexcept;

	/**
	 * @brief Strips every whitespace character.
	 * @param str Input.
	 */
	STORMBYTE_PUBLIC std::string RemoveWhitespace(const std::string& str) noexcept;

	/**
	 * @brief `true` when @p str parses as an integer.
	 * @param str Input.
	 */
	STORMBYTE_PUBLIC bool IsInteger(const std::string& str) noexcept;
}
