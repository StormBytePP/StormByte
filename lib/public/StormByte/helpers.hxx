/*
 * Copyright (C) 2024-2026 David C. Manuelda (StormBytePP)
 *
 * This file is part of StormByte.
 *
 * StormByte original source is dual-licensed:
 *
 * 1. GNU Lesser General Public License v3.0 (or later)
 *    You may redistribute and/or modify this file under the terms of the
 *    GNU Lesser General Public License as published by the Free Software
 *    Foundation, either version 3 of the License, or (at your option)
 *    any later version.
 *
 * 2. Commercial license
 *    Alternatively, this file may be used under the terms of a commercial
 *    license agreement with the copyright holder
 *    (David C. Manuelda <StormByte@gmail.com>).
 *
 * Both licenses apply only to original StormByte source in this repository.
 * They do not cover other StormByte modules or any third-party material
 * shipped with this repository (including everything under thirdparty/),
 * which remains under its own license.
 *
 * Neither license grants any patent rights. Any patent licenses required
 * to use this software or third-party components must be obtained separately
 * from the patent holders.
 *
 * StormByte is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with StormByte. If not, see
 * <https://www.gnu.org/licenses/lgpl-3.0.html>.
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-StormByte-Commercial
 */

#pragma once

#include <StormByte/binary_data.hxx>
#include <StormByte/type_traits.hxx>
#include <StormByte/visibility.h>

#include <span>
#include <utility>
#include <vector>
#include <version>

/**
 * @namespace StormByte
 * @brief Root namespace of the StormByte suite.
 */
namespace StormByte {
	/**
	 * @brief Appends a span of bytes onto a @ref BinaryData.
	 * @param dest Destination owned by Base.
	 * @param src View over the bytes to copy.
	 */
	inline void append_bytes(BinaryData& dest, std::span<const std::byte> src) noexcept {
		dest.append(src);
	}

	/**
	 * @brief Appends a @ref BinaryData by copy.
	 * @param dest Destination owned by Base.
	 * @param src Source sequence (read-only).
	 */
	inline void append_bytes(BinaryData& dest, const BinaryData& src) noexcept {
		dest.append(src);
	}

	/**
	 * @brief Appends a @ref BinaryData by move on Base's heap.
	 * @param dest Destination owned by Base.
	 * @param src Source sequence; emptied after a successful append.
	 */
	inline void append_bytes(BinaryData& dest, BinaryData&& src) noexcept {
		dest.append(std::move(src));
	}

	/**
	 * @brief Appends a span of convertible elements onto a vector.
	 * @tparam T Destination element type.
	 * @tparam U Source element type (`Type::ConvertibleTo<T, U>`).
	 * @param dest Vector that receives the elements.
	 * @param src View over the elements to copy.
	 * @note Local STL helper. Do not use this for public byte blobs; use @ref append_bytes.
	 */
	template<typename T, typename U>
	void append_vector(std::vector<T>& dest, std::span<U> src) noexcept requires Type::ConvertibleTo<T, U> {
		dest.reserve(dest.size() + src.size());
#ifdef __cpp_lib_containers_ranges
		dest.append_range(src);
#else
		dest.insert(dest.end(), src.begin(), src.end());
#endif
	}

	/**
	 * @brief Appends a vector by copy.
	 * @tparam T Element type.
	 * @param dest Vector that receives the elements.
	 * @param src Source vector (read-only).
	 * @note Local STL helper. Do not use this for public byte blobs; use @ref append_bytes.
	 */
	template<typename T>
	void append_vector(std::vector<T>& dest, const std::vector<T>& src) noexcept {
		return append_vector(dest, std::span<const T>(src.data(), src.size()));
	}

	/**
	 * @brief Appends a vector by move.
	 * @tparam T Element type.
	 * @param dest Vector that receives the elements.
	 * @param src Source vector; elements are moved out.
	 * @note Local STL helper. Do not use this for public byte blobs; use @ref append_bytes.
	 */
	template<typename T>
	void append_vector(std::vector<T>& dest, std::vector<T>&& src) noexcept {
		dest.reserve(dest.size() + src.size());
#ifdef __cpp_lib_containers_ranges
		dest.append_range(std::move(src));
#else
		dest.insert(dest.end(), std::make_move_iterator(src.begin()), std::make_move_iterator(src.end()));
#endif
	}
}
