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
#include <string>
#include <type_traits>

/**
 * @namespace StormByte
 * @brief Root namespace of the StormByte suite.
 */
namespace StormByte {
	namespace Type {
		/**
		 * @defgroup TypeContainers Container concepts
		 * @brief Container shape and mutation capability predicates.
		 * @{
		 */

		/**
		 * @brief Text string types that are not generic containers.
		 * @tparam T Type to test.
		 *
		 * Includes `std::string`, `std::wstring`, `std::u16string` and
		 * `std::u32string`. They must stay out of @ref Container so
		 * @ref Serializable routes them through @ref Detail::Codec.
		 *
		 * @code
		 * template<Type::String T>
		 * void process(T str);
		 * @endcode
		 */
		template<typename T>
		concept String =
			std::same_as<T, std::string> ||
			std::same_as<T, std::wstring> ||
			std::same_as<T, std::u16string> ||
			std::same_as<T, std::u32string>;

		/**
		 * @brief Has `begin()`, `end()` and `value_type`, and is not a @ref String.
		 * @tparam T Type to test (cv/ref as written; strings use `std::decay_t`).
		 *
		 * Strings are excluded so serialization / pretty-print do not treat
		 * them as generic sequences of code units.
		 *
		 * @code
		 * template<Type::Container T>
		 * void process(T container);
		 * @endcode
		 */
		template<typename T>
		concept Container =
			requires(T t) {
				t.begin();
				t.end();
				typename T::value_type;
			} && !String<std::decay_t<T>>;

		/**
		 * @brief @ref Container that publishes a nested `key_type`.
		 * @tparam C Container type (cv/ref ignored for the nested lookup).
		 *
		 * Typical matches: `std::map`, `std::set`, `std::unordered_map`,
		 * `std::unordered_set`.
		 *
		 * @code
		 * template<Type::HasKeyType T>
		 * void process(T container);
		 * @endcode
		 */
		template<typename C>
		concept HasKeyType =
			Container<std::remove_cvref_t<C>> &&
			requires { typename std::remove_cvref_t<C>::key_type; };

		/**
		 * @brief @ref Container that publishes a nested `mapped_type`.
		 * @tparam C Container type (cv/ref ignored for the nested lookup).
		 *
		 * Typical matches: `std::map`, `std::unordered_map`.
		 *
		 * @code
		 * template<Type::HasMappedType T>
		 * void process(T container);
		 * @endcode
		 */
		template<typename C>
		concept HasMappedType =
			Container<std::remove_cvref_t<C>> &&
			requires { typename std::remove_cvref_t<C>::mapped_type; };

		/**
		 * @brief @ref Container that accepts `push_back(value)`.
		 * @tparam C Container type (cv/ref ignored).
		 *
		 * Typical matches: `std::vector`, `std::deque`, `std::list`.
		 *
		 * @code
		 * static_assert(Type::HasPushBack<std::vector<int>>);
		 * @endcode
		 */
		template<typename C>
		concept HasPushBack =
			Container<std::remove_cvref_t<C>> &&
			requires(std::remove_cvref_t<C>& c,
					typename std::remove_cvref_t<C>::value_type const& v) {
				c.push_back(v);
			};

		/**
		 * @brief @ref Container that accepts `push_front(value)`.
		 * @tparam C Container type (cv/ref ignored).
		 *
		 * Typical matches: `std::deque`, `std::list`. `std::vector` does
		 * **not** match.
		 *
		 * @code
		 * static_assert(Type::HasPushFront<std::deque<int>>);
		 * static_assert(!Type::HasPushFront<std::vector<int>>);
		 * @endcode
		 */
		template<typename C>
		concept HasPushFront =
			Container<std::remove_cvref_t<C>> &&
			requires(std::remove_cvref_t<C>& c,
					typename std::remove_cvref_t<C>::value_type const& v) {
				c.push_front(v);
			};

		/**
		 * @brief Associative @ref Container that accepts `insert(value)`.
		 * @tparam C Container type (cv/ref ignored).
		 *
		 * Requires @ref HasKeyType or @ref HasMappedType so positional
		 * `insert(iterator, value)` on `std::vector` does not satisfy this
		 * on any standard library.
		 *
		 * Typical matches: `std::map`, `std::set`, `std::unordered_map`,
		 * `std::unordered_set`.
		 *
		 * @code
		 * static_assert(Type::HasInsert<std::map<int, int>>);
		 * static_assert(Type::HasInsert<std::set<int>>);
		 * static_assert(!Type::HasInsert<std::vector<int>>);
		 * @endcode
		 */
		template<typename C>
		concept HasInsert =
			Container<std::remove_cvref_t<C>> &&
			(HasKeyType<C> || HasMappedType<C>) &&
			requires(std::remove_cvref_t<C>& c,
					typename std::remove_cvref_t<C>::value_type const& v) {
				c.insert(v);
			};

		/**
		 * @brief @ref Container that supports `operator[]` with key/index @p U.
		 * @tparam C Container type as written (no cv/ref strip — historical).
		 * @tparam U Key or index type passed to `operator[]`.
		 *
		 * @code
		 * static_assert(Type::HasSubscript<std::vector<int>, std::size_t>);
		 * static_assert(Type::HasSubscript<std::map<int, int>, int>);
		 * @endcode
		 */
		template<typename C, typename U>
		concept HasSubscript = Container<C> && requires(C& c, U const& u) {
			{ c[u] };
		};

		/**
		 * @brief @ref Container that publishes `size()` convertible to `std::size_t`.
		 * @tparam C Container type (cv/ref ignored).
		 *
		 * @code
		 * static_assert(Type::Sized<std::vector<int>>);
		 * @endcode
		 */
		template<typename C>
		concept Sized =
			Container<std::remove_cvref_t<C>> &&
			requires(std::remove_cvref_t<C> const& c) {
				{ c.size() } -> std::convertible_to<std::size_t>;
			};
		/** @} */
	}
}
