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

#include <concepts>
#include <cstddef>
#include <functional>
#include <type_traits>

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
		 * @defgroup TypeComparison Comparison and hashing concepts
		 * @brief Equality, ordering and `std::hash` support for a type.
		 * @{
		 */

		/**
		 * @name Comparison
		 * @{
		 */

		/**
		 * @brief Type whose `==` and `!=` both yield something convertible to `bool`.
		 * @tparam T Type to test.
		 *
		 * Not `std::equality_comparable`: this only checks same-type
		 * comparison, not the mixed-type / common-reference machinery.
		 *
		 * @code
		 * template<Type::EqualityComparable T>
		 * bool same(T const& a, T const& b) { return a == b; }
		 * @endcode
		 */
		template<typename T>
		concept EqualityComparable =
			requires(std::remove_cvref_t<T> const& a, std::remove_cvref_t<T> const& b) {
				{ a == b } -> std::convertible_to<bool>;
				{ a != b } -> std::convertible_to<bool>;
			};

		/**
		 * @brief Type whose `<=>` operator is well-formed.
		 * @tparam T Type to test.
		 *
		 * @code
		 * template<Type::ThreeWayComparable T>
		 * auto order(T const& a, T const& b) { return a <=> b; }
		 * @endcode
		 */
		template<typename T>
		concept ThreeWayComparable =
			requires(std::remove_cvref_t<T> const& a, std::remove_cvref_t<T> const& b) {
				{ a <=> b };
			};

		/**
		 * @brief Type with a valid `std::hash` specialization.
		 * @tparam T Type to test.
		 *
		 * @code
		 * template<Type::Hashable T>
		 * std::size_t hash_of(T const& value) { return std::hash<std::remove_cvref_t<T>>{}(value); }
		 * @endcode
		 */
		template<typename T>
		concept Hashable =
			requires(std::remove_cvref_t<T> const& t) {
				{ std::hash<std::remove_cvref_t<T>>{}(t) } -> std::convertible_to<std::size_t>;
			};

		/** @} */
		/** @} */
	}
}
