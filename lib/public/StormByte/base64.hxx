/*
 * Copyright (C) 2024-2026 David C. Manuelda (StormBytePP)
 *
 * This file is part of StormByte.
 *
 * StormByte is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * StormByte is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with StormByte. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <StormByte/visibility.h>

#include <cstddef>
#include <span>
#include <string>
#include <vector>

/**
 * @namespace StormByte
 * @brief Main namespace for the StormByte library.
 */
namespace StormByte {
	/**
	 * @brief Decodes a Base64-encoded string into raw bytes.
	 *
	 * The input may contain the standard Base64 alphabet (A-Z, a-z, 0-9, +, /)
	 * and the padding character '='. Whitespace (space, tab, newline, carriage return)
	 * is ignored. Decoding stops at the first '=' character.
	 *
	 * Padding is accepted but not strictly validated (the decoder is lenient).
	 *
	 * @param input Base64-encoded string.
	 * @return Decoded binary data as a vector of std::byte.
	 * @throws StormByte::Base64Error if the input contains invalid characters.
	 *
	 * @note Accepts both std::string and const char* (via implicit conversion).
	 */
	std::vector<std::byte> STORMBYTE_PUBLIC Base64Decode(const std::string& input);

	/**
	 * @brief Encodes raw binary data into a Base64 string.
	 *
	 * Uses the standard Base64 alphabet and adds the required '=' padding.
	 *
	 * @param input Binary data to encode.
	 * @return Base64-encoded string.
	 */
	std::string STORMBYTE_PUBLIC Base64Encode(const std::vector<std::byte>& input);

	/**
	 * @brief Encodes raw binary data into a Base64 string (span overload).
	 *
	 * This overload avoids unnecessary copies when the data is already
	 * available as a contiguous range (e.g. std::array, C array, etc.).
	 *
	 * @param input Binary data to encode.
	 * @return Base64-encoded string.
	 */
	std::string STORMBYTE_PUBLIC Base64Encode(std::span<const std::byte> input);
}
