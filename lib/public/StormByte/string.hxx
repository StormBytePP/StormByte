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
#include <cctype>
#include <iomanip>
#include <locale>
#include <queue>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

/**
 * @namespace StormByte
 * @brief Root namespace of the StormByte suite.
 */
/**
 * @namespace StormByte::String
 * @brief String helpers (case, split, UTF-8, human-readable numbers).
 *
 * Read-only inputs are @c std::string_view / @c std::wstring_view.
 * @c std::string, @c std::wstring and string literals convert to those views.
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
	 * @brief `true` when every character is a decimal digit (`std::isdigit`).
	 * @param str Text to test.
	 * @note Empty text is not numeric.
	 */
	constexpr STORMBYTE_PUBLIC bool IsNumeric(std::string_view str) noexcept {
		return !str.empty() && std::all_of(str.begin(), str.end(), [](unsigned char c) {
			return std::isdigit(c) != 0;
		});
	}

	/**
	 * @brief Lowercase copy of @p str.
	 * @param str Input text.
	 * @return New string; input is not modified.
	 */
	STORMBYTE_PUBLIC std::string ToLower(std::string_view str) noexcept;

	/**
	 * @brief Uppercase copy of @p str.
	 * @param str Input text.
	 * @return New string; input is not modified.
	 */
	STORMBYTE_PUBLIC std::string ToUpper(std::string_view str) noexcept;

	/**
	 * @brief Splits on @p delimiter into a queue (empty tokens kept).
	 * @param str Input text.
	 * @param delimiter Separator.
	 */
	STORMBYTE_PUBLIC std::queue<std::string> Explode(std::string_view str, const char delimiter);

	/**
	 * @brief Splits on ASCII whitespace into a vector.
	 * @param str Input text.
	 */
	STORMBYTE_PUBLIC std::vector<std::string> Split(std::string_view str);

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
	 * @brief Wide text to UTF-8.
	 * @param ws Wide input.
	 * @throws StormByte::UTF8Error On invalid Unicode input.
	 */
	STORMBYTE_PUBLIC std::string UTF8Encode(std::wstring_view ws);

	/**
	 * @brief UTF-8 text to wide string.
	 * @param s UTF-8 input.
	 * @throws StormByte::UTF8Error On invalid Unicode input.
	 */
	STORMBYTE_PUBLIC std::wstring UTF8Decode(std::string_view s);

	/**
	 * @brief Normalizes `\r\n` to `\n`. Lone `\r` is kept.
	 * @param str Input text.
	 */
	STORMBYTE_PUBLIC std::string SanitizeNewlines(std::string_view str) noexcept;

	/**
	 * @brief Interprets bytes as `char` and builds a string.
	 * @param byte_vector Input bytes.
	 */
	STORMBYTE_PUBLIC std::string FromByteVector(const std::vector<std::byte>& byte_vector) noexcept;

	/**
	 * @brief Copies text bytes into a `vector<std::byte>`.
	 * @param str Input text.
	 */
	STORMBYTE_PUBLIC std::vector<std::byte> ToByteVector(std::string_view str) noexcept;

	/**
	 * @brief Strips every whitespace character.
	 * @param str Input text.
	 */
	STORMBYTE_PUBLIC std::string RemoveWhitespace(std::string_view str) noexcept;

	/**
	 * @brief `true` when @p str parses as an integer (optional leading `+`/`-`).
	 * @param str Input text.
	 */
	STORMBYTE_PUBLIC bool IsInteger(std::string_view str) noexcept;
}
