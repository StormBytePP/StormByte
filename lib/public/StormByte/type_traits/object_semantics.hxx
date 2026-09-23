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

#include <StormByte/type_traits/containers.hxx>
#include <StormByte/type_traits/ranges.hxx>

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
		 * @defgroup TypeObjectSemantics Object semantics concepts
		 * @brief Construction, destruction and copy/move semantics predicates.
		 * @{
		 */

		namespace {
			/**
			 * @brief `T{src}` is well-formed for a const lvalue @p src.
			 * @tparam T Type to test.
			 */
			template<typename T, typename = void>
			inline constexpr bool ReallyCopyConstructible =
				requires(const T& src) { T{src}; };

			/**
			 * @brief Owning containers also require a copyable `value_type`.
			 * @tparam T Container that is not a @ref StormByte::Type::View.
			 */
			template<typename T>
			inline constexpr bool ReallyCopyConstructible<
				T, std::enable_if_t<
					Container<std::remove_cvref_t<T>> &&
					!View<std::remove_cvref_t<T>>
				>
			> =
				requires(const T& src) { T{src}; } &&
				ReallyCopyConstructible<typename std::remove_cvref_t<T>::value_type>;

			/**
			 * @brief `dest = src` is well-formed for const lvalue @p src.
			 * @tparam T Type to test.
			 */
			template<typename T, typename = void>
			inline constexpr bool ReallyCopyAssignable =
				requires(T& dest, const T& src) { dest = src; };

			/**
			 * @brief Owning containers also require an assignable `value_type`.
			 * @tparam T Container that is not a @ref StormByte::Type::View.
			 */
			template<typename T>
			inline constexpr bool ReallyCopyAssignable<
				T, std::enable_if_t<
					Container<std::remove_cvref_t<T>> &&
					!View<std::remove_cvref_t<T>>
				>
			> =
				requires(T& dest, const T& src) { dest = src; } &&
				ReallyCopyAssignable<typename std::remove_cvref_t<T>::value_type>;
		}

		/**
		 * @name Trivial
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

		/** @} */

		/**
		 * @name Construction
		 * @{
		 */

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
		 * @brief @ref StormByte::Type::DefaultConstructible with a trivial default constructor.
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
		 * @brief Type that can actually be copy-constructed.
		 * @tparam T Type to test.
		 *
		 * Not `std::is_copy_constructible_v`: that trait is true for
		 * `std::vector<std::unique_ptr<U>>` because `vector` declares a
		 * copy constructor even when instantiating it is ill-formed.
		 * Recurses into `value_type` only when @p T is a @ref StormByte::Type::Container
		 * and not a @ref StormByte::Type::View. `std::span<std::unique_ptr<U>>` is a
		 * container *and* a view, so it stays copyable; an owning
		 * `std::vector<std::unique_ptr<U>>` does not.
		 *
		 * @code
		 * static_assert(Type::CopyConstructible<std::vector<int>>);
		 * static_assert(!Type::CopyConstructible<std::vector<std::unique_ptr<int>>>);
		 * static_assert(Type::CopyConstructible<std::span<std::unique_ptr<int>>>);
		 * @endcode
		 */
		template<typename T>
		concept CopyConstructible = ReallyCopyConstructible<T>;

		/**
		 * @brief @ref StormByte::Type::CopyConstructible with a trivial copy constructor.
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
		 * @brief @ref StormByte::Type::MoveConstructible with a trivial move constructor.
		 * @tparam T Type to test.
		 *
		 * @code
		 * template<Type::TriviallyMoveConstructible T>
		 * void fast_construct(T* dest, T&& src);
		 * @endcode
		 */
		template<typename T>
		concept TriviallyMoveConstructible = std::is_trivially_move_constructible_v<T>;

		/** @} */

		/**
		 * @name Assignment
		 * @{
		 */

		/**
		 * @brief Type that can actually be copy-assigned.
		 * @tparam T Type to test.
		 *
		 * Not `std::is_copy_assignable_v`: same caveat as
		 * @ref StormByte::Type::CopyConstructible for containers of move-only values.
		 * Recurses into `value_type` only when @p T is a @ref StormByte::Type::Container
		 * and not a @ref StormByte::Type::View.
		 *
		 * @code
		 * template<Type::CopyAssignable T>
		 * void overwrite(T& dest, const T& src) { dest = src; }
		 * @endcode
		 */
		template<typename T>
		concept CopyAssignable = ReallyCopyAssignable<T>;

		/**
		 * @brief @ref StormByte::Type::CopyAssignable with a trivial copy-assignment operator.
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
		 * @brief @ref StormByte::Type::MoveAssignable with a trivial move-assignment operator.
		 * @tparam T Type to test.
		 *
		 * @code
		 * template<Type::TriviallyMoveAssignable T>
		 * void fast_take_over(T& dest, T&& src) { dest = std::move(src); }
		 * @endcode
		 */
		template<typename T>
		concept TriviallyMoveAssignable = std::is_trivially_move_assignable_v<T>;

		/** @} */

		/**
		 * @name Combined
		 * @{
		 */

		/**
		 * @brief Both @ref StormByte::Type::CopyConstructible and @ref StormByte::Type::CopyAssignable.
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
		 * @brief Both @ref StormByte::Type::MoveConstructible and @ref StormByte::Type::MoveAssignable.
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
		/** @} */
	}
}
