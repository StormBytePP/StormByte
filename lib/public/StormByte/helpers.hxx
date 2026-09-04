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

#include <StormByte/type_traits.hxx>
#include <StormByte/visibility.h>

#include <span>
#include <vector>
#include <version>

/**
 * @namespace StormByte
 * @brief Root namespace of the StormByte suite.
 */
namespace StormByte {
	/**
	 * @brief Appends a span of convertible elements onto a vector.
	 * @tparam T Destination element type.
	 * @tparam U Source element type (`Type::ConvertibleTo<T, U>`).
	 * @param dest Vector that receives the elements.
	 * @param src View over the elements to copy.
	 * @note Reserves once, then uses `append_range` when the library has it.
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
	 * @note Reserves once, then `append_range(std::move(src))` when available.
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
