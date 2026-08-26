/*
* Copyright (C) 2024-2026 David C. Manuelda (StormBytePP)
*
* This file is part of StormByte.
*
* StormByte is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* StormByte is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with StormByte. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <concepts>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>

/**
 * @namespace StormByte
 * @brief Root namespace of the StormByte library.
 *
 * Houses foundational types, concepts and utilities shared by every StormByte module.
 */
namespace StormByte {
	/**
	 * @brief Implementation details for @ref StormByte::Type concepts.
	 *
	 * These helpers are not part of the public API. Prefer the concepts in
	 * @ref StormByte::Type from user code.
	 */
	namespace {
		/**
		 * @brief Trait that is true when @p T is `std::string` or `std::wstring`.
		 * @tparam T Type to test.
		 */
		template<typename T>
		struct is_string : std::bool_constant<std::is_same_v<T, std::string> || std::is_same_v<T, std::wstring>> {};

		/**
		 * @brief Primary template: @p T is not treated as a container.
		 * @tparam T Type to test.
		 * @tparam _ SFINAE placeholder.
		 */
		template<typename T, typename _ = void>
		struct is_container : std::false_type {};

		/**
		 * @brief Specialization: @p T has `begin()`, `end()` and `value_type`, and is not a string.
		 * @tparam T Type to test.
		 */
		template<typename T>
		struct is_container<T, std::void_t<decltype(std::declval<T>().begin()), decltype(std::declval<T>().end()), typename T::value_type>>
			: std::bool_constant<!is_string<std::decay_t<T>>::value> {};

		/**
		 * @brief Primary template: @p T is not `std::optional`.
		 * @tparam T Type to test.
		 * @tparam _ SFINAE placeholder.
		 */
		template<typename T, typename _ = void>
		struct is_optional : std::false_type {};

		/**
		 * @brief Specialization: @p T is exactly `std::optional<U>` for some `U`.
		 * @tparam T Type to test.
		 */
		template<typename T>
		struct is_optional<T, std::void_t<typename T::value_type>>
			: std::is_same<T, std::optional<typename T::value_type>> {};

		/**
		 * @brief Primary template: @p T is not pair-like.
		 * @tparam T Type to test.
		 * @tparam _ SFINAE placeholder.
		 */
		template<typename T, typename _ = void>
		struct is_pair : std::false_type {};

		/**
		 * @brief Specialization: @p T exposes `first` and `second` members.
		 * @tparam T Type to test.
		 */
		template<typename T>
		struct is_pair<T, std::void_t<
			decltype(std::declval<T>().first),
			decltype(std::declval<T>().second)
		>> : std::true_type {};

		/**
		 * @brief Primary template: @p T is not a `std::variant`.
		 * @tparam T Type to test.
		 */
		template<typename T>
		struct is_variant : std::false_type {};

		/**
		 * @brief Specialization: @p T is `std::variant<Ts...>`.
		 * @tparam Ts Variant alternatives.
		 */
		template<typename... Ts>
		struct is_variant<std::variant<Ts...>> : std::true_type {};

		/**
		 * @brief Returns whether any alternative of @p VariantT is the same as @p U (cv/ref stripped).
		 * @tparam VariantT Variant type.
		 * @tparam U Candidate alternative.
		 * @tparam I Index pack over the variant alternatives.
		 * @return `true` if @p U is one of the alternatives.
		 */
		template<typename VariantT, typename U, std::size_t... I>
		constexpr bool variant_has_type_impl(std::index_sequence<I...>) noexcept {
			return ((std::is_same_v<std::remove_cvref_t<U>, std::remove_cvref_t<std::variant_alternative_t<I, VariantT>>>) || ...);
		}

		/**
		 * @brief Trait that is true when @p U is an alternative of variant @p VariantT.
		 * @tparam VariantT Variant type.
		 * @tparam U Candidate alternative.
		 */
		template<typename VariantT, typename U>
		struct variant_has_type : std::bool_constant<
			variant_has_type_impl<VariantT, U>(std::make_index_sequence<std::variant_size_v<VariantT>>())
		> {};

		/**
		 * @brief Swaps the endianness of a trivially copyable value.
		 * @tparam U Value type.
		 * @param val Value whose bytes should be reversed.
		 * @return @p val with reversed byte order.
		 */
		template<typename U>
		constexpr U swap_endian(U val) noexcept {
			union {
				U value;
				unsigned char bytes[sizeof(U)];
			} src, dest;

			src.value = val;
			for (std::size_t i = 0; i < sizeof(U); ++i) {
				dest.bytes[i] = src.bytes[sizeof(U) - 1 - i];
			}
			return dest.value;
		}
	}

	/**
	 * @namespace Type
	 * @brief C++20 concepts for compile-time type introspection.
	 *
	 * Prefer these concepts over ad-hoc SFINAE in public APIs. Container-related
	 * concepts decay cv-qualifiers and references so constraints stay stable
	 * across libstdc++, libc++ and the MSVC STL (including clang-cl).
	 */
	namespace Type {
		/**
		 * @brief Matches `std::string` or `std::wstring` exactly.
		 * @tparam T Type to test.
		 *
		 * @code
		 * template<Type::String T>
		 * void process(T str);
		 * @endcode
		 */
		template<typename T>
		concept String = is_string<T>::value;

		/**
		 * @brief Type with `begin()`, `end()` and `value_type`, excluding string types.
		 * @tparam T Type to test.
		 *
		 * Strings are excluded so they are not treated as generic sequences.
		 *
		 * @code
		 * template<Type::Container T>
		 * void process(T container);
		 * @endcode
		 */
		template<typename T>
		concept Container = is_container<T>::value;

		/**
		 * @brief Container that defines a nested `key_type`.
		 * @tparam C Container type.
		 *
		 * Typical matches: `std::map`, `std::set`, `std::unordered_map`, `std::unordered_set`.
		 *
		 * @code
		 * template<Type::HasKeyType T>
		 * void process(T container);
		 * @endcode
		 */
		template<typename C>
		concept HasKeyType = Container<std::remove_cvref_t<C>> &&
			requires { typename std::remove_cvref_t<C>::key_type; };

		/**
		 * @brief Container that defines a nested `mapped_type`.
		 * @tparam C Container type.
		 *
		 * Typical matches: `std::map`, `std::unordered_map`.
		 *
		 * @code
		 * template<Type::HasMappedType T>
		 * void process(T container);
		 * @endcode
		 */
		template<typename C>
		concept HasMappedType = Container<std::remove_cvref_t<C>> &&
			requires { typename std::remove_cvref_t<C>::mapped_type; };

		/**
		 * @brief Container that accepts `push_back(value)`.
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
			requires(std::remove_cvref_t<C>& c, typename std::remove_cvref_t<C>::value_type const& v) {
				c.push_back(v);
			};

		/**
		 * @brief Container that accepts `push_front(value)`.
		 * @tparam C Container type (cv/ref ignored).
		 *
		 * Typical matches: `std::deque`, `std::list`. `std::vector` does **not** match.
		 *
		 * @code
		 * static_assert(Type::HasPushFront<std::deque<int>>);
		 * static_assert(!Type::HasPushFront<std::vector<int>>);
		 * @endcode
		 */
		template<typename C>
		concept HasPushFront =
			Container<std::remove_cvref_t<C>> &&
			requires(std::remove_cvref_t<C>& c, typename std::remove_cvref_t<C>::value_type const& v) {
				c.push_front(v);
			};

		/**
		 * @brief Associative container that accepts `insert(value)`.
		 * @tparam C Container type (cv/ref ignored).
		 *
		 * Requires `key_type` and/or `mapped_type` so sequence containers whose
		 * `insert` is positional (`insert(iterator, value)`, e.g. `std::vector`)
		 * do not satisfy this concept on any standard library.
		 *
		 * Typical matches: `std::map`, `std::set`, `std::unordered_map`, `std::unordered_set`.
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
			requires(std::remove_cvref_t<C>& c, typename std::remove_cvref_t<C>::value_type const& v) {
				c.insert(v);
			};

		/**
		 * @brief Container that supports `operator[]` with key or index type @p U.
		 * @tparam C Container type.
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
		 * @brief Matches `std::optional<U>` for some `U`.
		 * @tparam T Type to test.
		 *
		 * @code
		 * template<Type::Optional T>
		 * void process(T opt);
		 * @endcode
		 */
		template<typename T>
		concept Optional = is_optional<T>::value;

		/**
		 * @brief Type with `first` and `second` members (e.g. `std::pair`).
		 * @tparam T Type to test.
		 *
		 * @code
		 * template<Type::Pair T>
		 * void process(T pair);
		 * @endcode
		 */
		template<typename T>
		concept Pair = is_pair<T>::value;

		/**
		 * @brief Lvalue or rvalue reference type.
		 * @tparam T Type to test.
		 *
		 * @code
		 * template<Type::Reference T>
		 * void process(T&& ref);
		 * @endcode
		 */
		template<typename T>
		concept Reference = std::is_reference_v<T>;

		/**
		 * @brief Enumeration type (`enum` or `enum class`).
		 * @tparam E Type to test.
		 *
		 * @code
		 * template<Type::Enum E>
		 * void process(E value);
		 * @endcode
		 */
		template<typename E>
		concept Enum = std::is_enum_v<std::remove_cv_t<E>>;

		/**
		 * @brief Enumeration whose underlying type is unsigned.
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
			std::is_unsigned_v<std::underlying_type_t<std::remove_cv_t<E>>>;

		/**
		 * @brief Scoped enumeration (`enum class`).
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
		 * @tparam E Enumeration type.
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
		 * @tparam E Enumeration type.
		 * @param e Value to convert.
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

		/**
		 * @brief Raw pointer type.
		 * @tparam T Type to test.
		 *
		 * @code
		 * template<Type::Pointer T>
		 * void process(T ptr);
		 * @endcode
		 */
		template<typename T>
		concept Pointer = std::is_pointer_v<T>;

		/**
		 * @brief Integer type (`int`, `long`, `short`, …).
		 * @tparam T Type to test.
		 *
		 * @code
		 * template<Type::Integral T>
		 * T add(T a, T b) { return a + b; }
		 * @endcode
		 */
		template<typename T>
		concept Integral = std::is_integral_v<T>;

		/**
		 * @brief Floating-point type (`float`, `double`, `long double`).
		 * @tparam T Type to test.
		 *
		 * @code
		 * template<Type::FloatingPoint T>
		 * T multiply(T a, T b) { return a * b; }
		 * @endcode
		 */
		template<typename T>
		concept FloatingPoint = std::is_floating_point_v<T>;

		/**
		 * @brief Integral or floating-point type.
		 * @tparam T Type to test.
		 *
		 * @code
		 * template<Type::Arithmetic T>
		 * T compute(T x) { return x * 2; }
		 * @endcode
		 */
		template<typename T>
		concept Arithmetic = std::is_arithmetic_v<T>;

		/**
		 * @brief Signed arithmetic type.
		 * @tparam T Type to test.
		 *
		 * @code
		 * template<Type::Signed T>
		 * T negate(T value) { return -value; }
		 * @endcode
		 */
		template<typename T>
		concept Signed = std::is_signed_v<T>;

		/**
		 * @brief Unsigned arithmetic type.
		 * @tparam T Type to test.
		 *
		 * @code
		 * template<Type::Unsigned T>
		 * bool check(T value) { return value >= 0; }
		 * @endcode
		 */
		template<typename T>
		concept Unsigned = std::is_unsigned_v<T>;

		/**
		 * @brief Const-qualified type.
		 * @tparam T Type to test.
		 *
		 * @code
		 * template<Type::Const T>
		 * void read_only(T value);
		 * @endcode
		 */
		template<typename T>
		concept Const = std::is_const_v<T>;

		/**
		 * @brief Class or struct type.
		 * @tparam T Type to test.
		 *
		 * @code
		 * template<Type::Class T>
		 * void inspect(T obj);
		 * @endcode
		 */
		template<typename T>
		concept Class = std::is_class_v<T>;

		/**
		 * @brief Instantiation of `std::variant`.
		 * @tparam T Type to test.
		 *
		 * @code
		 * template<Type::Variant T>
		 * void process(T var);
		 * @endcode
		 */
		template<typename T>
		concept Variant = is_variant<std::remove_cvref_t<T>>::value;

		/**
		 * @brief Variant @p T that lists @p U among its alternatives.
		 * @tparam T Variant type.
		 * @tparam U Alternative to look for.
		 *
		 * @code
		 * template<typename T, typename U>
		 * requires Type::VariantHasType<T, U>
		 * void handle_variant(T var);
		 * @endcode
		 */
		template<typename T, typename U>
		concept VariantHasType = Variant<T> && variant_has_type<std::remove_cvref_t<T>, U>::value;

		/**
		 * @brief Type that may be copied with `memcpy`.
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
		 * @brief Type constructible without arguments.
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
		 * @brief Invocable with the given argument types.
		 * @tparam F Callable type.
		 * @tparam Args Argument types.
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
		 * @brief Types that are the same after removing cv-qualifiers and references.
		 * @tparam T First type.
		 * @tparam U Second type.
		 *
		 * @code
		 * template<typename T, typename U>
		 * requires Type::SameAs<T, U>
		 * void ensure_same_type(T a, U b);
		 * @endcode
		 */
		template<typename T, typename U>
		concept SameAs = std::is_same_v<std::remove_cvref_t<T>, std::remove_cvref_t<U>>;

		/**
		 * @brief @p From is implicitly convertible to @p To.
		 * @tparam From Source type.
		 * @tparam To Target type.
		 *
		 * @code
		 * template<typename From, typename To>
		 * requires Type::ConvertibleTo<From, To>
		 * To convert(From value);
		 * @endcode
		 */
		template<typename From, typename To>
		concept ConvertibleTo = std::is_convertible_v<From, To>;
	}
}
