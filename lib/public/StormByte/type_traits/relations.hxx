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
		 * @defgroup TypeRelations Type relation concepts
		 * @brief Relationships between two (or more) types: invocability, equivalence, convertibility.
		 * @{
		 */

		/**
		 * @name Relations
		 * @{
		 */

		/**
		 * @brief Invocable with argument types @p Args (`std::is_invocable`).
		 * @tparam F Callable type.
		 * @tparam Args Argument types passed to `F`.
		 *
		 * @code
		 * template<typename F, typename... Args>
		 * requires Type::Callable<F, Args...>
		 * auto invoke(F&& func, Args&&... args);
		 * @endcode
		 */
		template<typename F, typename... Args>
		concept Callable = std::is_invocable_v<F, Args...>;

		/**
		 * @brief Same type after stripping cv and references from both sides.
		 * @tparam T First type.
		 * @tparam U Second type.
		 *
		 * Not `std::same_as`: that does not strip. `int` and `const int&`
		 * match here.
		 *
		 * @code
		 * template<typename T, typename U>
		 * requires Type::SameAs<T, U>
		 * void ensure_same_type(T a, U b);
		 * @endcode
		 */
		template<typename T, typename U>
		concept SameAs =
			std::is_same_v<std::remove_cvref_t<T>, std::remove_cvref_t<U>>;

		/**
		 * @brief @p Derived derives from @p Base (`std::is_base_of`).
		 * @tparam Derived Candidate derived type.
		 * @tparam Base Candidate base type.
		 *
		 * Not `std::derived_from`: that also requires an unambiguous,
		 * public base-to-derived conversion. This only checks the
		 * inheritance relationship itself.
		 *
		 * @code
		 * struct Base {};
		 * struct Derived : Base {};
		 * static_assert(Type::DerivedFrom<Derived, Base>);
		 * @endcode
		 */
		template<typename Derived, typename Base>
		concept DerivedFrom = std::is_base_of_v<Base, Derived>;

		/** @} */
		/** @} */
	}
}
