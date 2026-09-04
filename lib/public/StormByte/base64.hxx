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

#include <cstddef>
#include <span>
#include <string>
#include <vector>

/**
 * @namespace StormByte
 * @brief Root namespace of the StormByte suite.
 */
namespace StormByte {
	/**
	 * @brief Decodes a Base64 string into bytes.
	 *
	 * Alphabet `A–Z a–z 0–9 + /` and padding `=`.
	 * Space, tab, newline and CR are ignored.
	 * Decoding stops at the first `=`.
	 * Padding is accepted but not strictly checked.
	 *
	 * @param input Base64 text (`std::string` or `const char*` via conversion).
	 * @return Decoded bytes.
	 * @throws StormByte::Base64Error If a character is outside the alphabet.
	 */
	std::vector<std::byte> STORMBYTE_PUBLIC Base64Decode(const std::string& input);

	/**
	 * @brief Encodes bytes as Base64 with `=` padding.
	 * @param input Bytes to encode.
	 */
	std::string STORMBYTE_PUBLIC Base64Encode(const std::vector<std::byte>& input);

	/**
	 * @brief Encodes a contiguous byte span as Base64 with `=` padding.
	 * @param input Bytes to encode.
	 */
	std::string STORMBYTE_PUBLIC Base64Encode(std::span<const std::byte> input);
}
