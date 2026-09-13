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

// UnsignedEnum reuses Unsigned on the enum's underlying type.
#include <StormByte/type_traits/categories.hxx>

#include <type_traits>

/**
 * @namespace StormByte
 * @brief Root namespace of the StormByte suite.
 */
namespace StormByte {
	namespace Type {
		/**
		 * @defgroup TypeEnums Enumeration concepts
		 * @brief Concepts and helpers for `enum` / `enum class` types.
		 * @{
		 */

		/**
		 * @brief Unscoped or scoped enumeration (`enum` / `enum class`).
		 * @tparam E Type to test (cv stripped).
		 *
		 * @code
		 * template<Type::Enum E>
		 * void process(E value);
		 * @endcode
		 */
		template<typename E>
		concept Enum = std::is_enum_v<std::remove_cv_t<E>>;

		/**
		 * @brief @ref Enum whose underlying type is unsigned.
		 * @tparam E Type to test.
		 *
		 * @code
		 * template<Type::UnsignedEnum E>
		 * void process(E value);
		 * @endcode
		 */
		template<typename E>
		concept UnsignedEnum =
			Enum<E> &&
			Unsigned<std::underlying_type_t<std::remove_cv_t<E>>>;

		/**
		 * @brief Scoped enumeration (`enum class` / `enum struct`).
		 * @tparam E Type to test.
		 *
		 * @code
		 * template<Type::ScopedEnum E>
		 * void process(E value);
		 * @endcode
		 */
		template<typename E>
		concept ScopedEnum = std::is_scoped_enum_v<E>;

		/**
		 * @brief Underlying integer type of enumeration @p E.
		 * @tparam E Enumeration type satisfying @ref Enum.
		 *
		 * @code
		 * enum class Foo : uint16_t { A };
		 * Type::UnderlyingType<Foo> value = 0;
		 * @endcode
		 */
		template<typename E>
		requires Enum<E>
		using UnderlyingType = std::underlying_type_t<std::remove_cv_t<E>>;

		/**
		 * @brief Converts an enumeration value to its underlying integer.
		 * @tparam E Enumeration type satisfying @ref Enum.
		 * @param[in] e Value to convert.
		 * @return Underlying integer representation of @p e.
		 *
		 * @code
		 * enum class Foo : int { A = 3 };
		 * auto n = Type::ToUnderlying(Foo::A); // 3
		 * @endcode
		 */
		template<typename E>
		requires Enum<E>
		constexpr UnderlyingType<E> ToUnderlying(E e) noexcept {
			return static_cast<UnderlyingType<E>>(e);
		}
		/** @} */
	}
}
