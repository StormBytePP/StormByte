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

#include <StormByte/type_traits/categories.hxx>
#include <StormByte/type_traits/object_semantics.hxx>

#include <array>
#include <bit>
#include <cstddef>

/**
 * @namespace StormByte
 * @brief Root namespace of the StormByte suite.
 */
namespace StormByte {
	/**
	 * @namespace Type
	 * @brief Named concepts and small type utilities used across the suite.
	 */
	namespace Type {
		/**
		 * @namespace Detail
		 * @brief Private helpers. Not a supported API.
		 *
		 * Other modules may call @ref StormByte::Type::Detail::swap_endian. Do not depend on
		 * any other name in this namespace.
		 */
		namespace Detail {
			/**
			 * @name Endian
			 * @{
			 */

			/**
			 * @brief Reverses the object-representation byte order of @p val.
			 * @tparam U Trivially copyable value type.
			 * @param[in] val Value whose bytes are reversed.
			 * @return @p val with endianness swapped.
			 *
			 * This **always** reverses bytes. Whether the caller should
			 * invoke it (host ≠ little-endian) is Serializable's job.
			 * Integrals go through `std::byteswap`; every other trivial
			 * type goes through `std::bit_cast` + reverse so `float`,
			 * `enum` and small POD structs stay defined.
			 *
			 * @warning Not a format detector. No BOM, no `std::endian` test.
			 */
			template<typename U>
			requires TriviallyCopyable<U> && (sizeof(U) > 0)
			constexpr U swap_endian(U val) noexcept {
				if constexpr (sizeof(U) == 1) {
					return val;
				} else if constexpr (Integral<U>) {
					return std::byteswap(val);
				} else {
					auto bytes = std::bit_cast<std::array<std::byte, sizeof(U)>>(val);
					for (std::size_t i = 0, j = sizeof(U); i < j; ++i) {
						--j;
						const std::byte tmp = bytes[i];
						bytes[i] = bytes[j];
						bytes[j] = tmp;
					}
					return std::bit_cast<U>(bytes);
				}
			}

			/** @} */
		}
	}
}
