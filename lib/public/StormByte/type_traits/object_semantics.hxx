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
	namespace Type {
		/**
		 * @defgroup TypeObjectSemantics Object semantics concepts
		 * @brief Construction, destruction and copy/move semantics predicates.
		 * @{
		 */

		/**
		 * @brief Type that may be copied with `memcpy` / as-if `memcpy`.
		 * @tparam T Type to test.
		 *
		 * @code
		 * template<Type::TriviallyCopyable T>
		 * void fast_copy(T* dest, const T* src, size_t n);
		 * @endcode
		 */
		template<typename T>
		concept TriviallyCopyable = std::is_trivially_copyable_v<T>;

		/**
		 * @brief Type with a trivial destructor.
		 * @tparam T Type to test.
		 *
		 * @code
		 * template<Type::TriviallyDestructible T>
		 * void pool_allocate(T* ptr);
		 * @endcode
		 */
		template<typename T>
		concept TriviallyDestructible = std::is_trivially_destructible_v<T>;

		/**
		 * @brief Type constructible from an empty initializer (`T{}` / `T()`).
		 * @tparam T Type to test.
		 *
		 * @code
		 * template<Type::DefaultConstructible T>
		 * T create() { return T{}; }
		 * @endcode
		 */
		template<typename T>
		concept DefaultConstructible = std::is_default_constructible_v<T>;

		/**
		 * @brief @ref DefaultConstructible with a trivial default constructor.
		 * @tparam T Type to test.
		 *
		 * @code
		 * template<Type::TriviallyDefaultConstructible T>
		 * T* pool_new();
		 * @endcode
		 */
		template<typename T>
		concept TriviallyDefaultConstructible = std::is_trivially_default_constructible_v<T>;

		/**
		 * @brief Type that can be copy-constructed.
		 * @tparam T Type to test.
		 *
		 * @code
		 * template<Type::CopyConstructible T>
		 * T duplicate(const T& original) { return T{original}; }
		 * @endcode
		 */
		template<typename T>
		concept CopyConstructible = std::is_copy_constructible_v<T>;

		/**
		 * @brief @ref CopyConstructible with a trivial copy constructor.
		 * @tparam T Type to test.
		 *
		 * @code
		 * template<Type::TriviallyCopyConstructible T>
		 * void fast_construct(T* dest, const T& src);
		 * @endcode
		 */
		template<typename T>
		concept TriviallyCopyConstructible = std::is_trivially_copy_constructible_v<T>;

		/**
		 * @brief Type that can be copy-assigned (`operator=(const T&)`).
		 * @tparam T Type to test.
		 *
		 * @code
		 * template<Type::CopyAssignable T>
		 * void overwrite(T& dest, const T& src) { dest = src; }
		 * @endcode
		 */
		template<typename T>
		concept CopyAssignable = std::is_copy_assignable_v<T>;

		/**
		 * @brief @ref CopyAssignable with a trivial copy-assignment operator.
		 * @tparam T Type to test.
		 *
		 * @code
		 * template<Type::TriviallyCopyAssignable T>
		 * void fast_assign(T& dest, const T& src) { dest = src; }
		 * @endcode
		 */
		template<typename T>
		concept TriviallyCopyAssignable = std::is_trivially_copy_assignable_v<T>;

		/**
		 * @brief Type that can be move-constructed.
		 * @tparam T Type to test.
		 *
		 * @code
		 * template<Type::MoveConstructible T>
		 * T transfer(T&& source) { return T{std::move(source)}; }
		 * @endcode
		 */
		template<typename T>
		concept MoveConstructible = std::is_move_constructible_v<T>;

		/**
		 * @brief @ref MoveConstructible with a trivial move constructor.
		 * @tparam T Type to test.
		 *
		 * @code
		 * template<Type::TriviallyMoveConstructible T>
		 * void fast_construct(T* dest, T&& src);
		 * @endcode
		 */
		template<typename T>
		concept TriviallyMoveConstructible = std::is_trivially_move_constructible_v<T>;

		/**
		 * @brief Type that can be move-assigned (`operator=(T&&)`).
		 * @tparam T Type to test.
		 *
		 * @code
		 * template<Type::MoveAssignable T>
		 * void take_over(T& dest, T&& src) { dest = std::move(src); }
		 * @endcode
		 */
		template<typename T>
		concept MoveAssignable = std::is_move_assignable_v<T>;

		/**
		 * @brief @ref MoveAssignable with a trivial move-assignment operator.
		 * @tparam T Type to test.
		 *
		 * @code
		 * template<Type::TriviallyMoveAssignable T>
		 * void fast_take_over(T& dest, T&& src) { dest = std::move(src); }
		 * @endcode
		 */
		template<typename T>
		concept TriviallyMoveAssignable = std::is_trivially_move_assignable_v<T>;

		/**
		 * @brief Both @ref CopyConstructible and @ref CopyAssignable.
		 * @tparam T Type to test.
		 *
		 * @code
		 * template<Type::Copyable T>
		 * T duplicate(const T& original);
		 * @endcode
		 */
		template<typename T>
		concept Copyable = CopyConstructible<T> && CopyAssignable<T>;

		/**
		 * @brief Both @ref MoveConstructible and @ref MoveAssignable.
		 * @tparam T Type to test.
		 *
		 * @code
		 * template<Type::Movable T>
		 * T transfer(T&& source);
		 * @endcode
		 */
		template<typename T>
		concept Movable = MoveConstructible<T> && MoveAssignable<T>;

		/**
		 * @brief Type that can be swapped with `std::swap` (`std::is_swappable`).
		 * @tparam T Type to test.
		 *
		 * @code
		 * template<Type::Swappable T>
		 * void exchange(T& a, T& b) { std::swap(a, b); }
		 * @endcode
		 */
		template<typename T>
		concept Swappable = std::is_swappable_v<T>;
		/** @} */
	}
}
