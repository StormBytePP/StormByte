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

/**
 * @file type_traits.hxx
 * @brief Compile-time type introspection for StormByte (concepts, C++26).
 *
 * Public surface is @ref StormByte::Type. Detection uses `concept` +
 * `requires`, not `void_t` / `enable_if`. Cv/ref handling is explicit so
 * libstdc++, libc++ and the MSVC STL (including clang-cl) agree.
 */

#include <array>
#include <bit>
#include <concepts>
#include <cstddef>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>

/**
 * @namespace StormByte
 * @brief Root namespace of the StormByte library.
 */
namespace StormByte {
	/**
	 * @namespace StormByte::Type
	 * @brief Named concepts and small type utilities used across the suite.
	 *
	 * Prefer these names in public templates. Do not reintroduce
	 * `std::enable_if` / `void_t` traits next to them.
	 */
	namespace Type {
		/**
		 * @namespace StormByte::Type::Detail
		 * @brief Private helpers. Not a supported API.
		 *
		 * @note Other modules may call @ref swap_endian. Do not depend on
		 *       any other name in this namespace.
		 */
		namespace Detail {
			/**
			 * @brief `true` when @p T is exactly `std::variant<Ts...>` after cv/ref strip.
			 * @tparam T Type to test.
			 *
			 * Partial specialization of a variable template — not SFINAE.
			 * Needed because there is no standard `std::is_variant`.
			 */
			template<typename T>
			constexpr bool is_variant_v = false;

			/**
			 * @brief Specialization for every `std::variant` alternative list.
			 * @tparam Ts Variant alternatives.
			 */
			template<typename... Ts>
			constexpr bool is_variant_v<std::variant<Ts...>> = true;

			/**
			 * @brief Fold: is cv/ref-stripped @p U one of @p VariantT's alternatives?
			 * @tparam VariantT A `std::variant<…>` (already stripped by the caller).
			 * @tparam U Candidate alternative.
			 * @tparam I Index pack over `std::variant_size_v<VariantT>`.
			 * @param[in] seq Index sequence; unused except to expand @p I.
			 * @return `true` if @p U matches any alternative.
			 */
			template<typename VariantT, typename U, std::size_t... I>
			constexpr bool variant_has_type_impl(std::index_sequence<I...> seq) noexcept {
				(void)seq;
				return ((std::is_same_v<
					std::remove_cvref_t<U>,
					std::remove_cvref_t<std::variant_alternative_t<I, VariantT>>
				>) || ...);
			}

			/**
			 * @brief Convenience wrapper around @ref variant_has_type_impl.
			 * @tparam VariantT A `std::variant<…>`.
			 * @tparam U Candidate alternative.
			 */
			template<typename VariantT, typename U>
			constexpr bool variant_has_type_v =
				variant_has_type_impl<VariantT, U>(
					std::make_index_sequence<std::variant_size_v<VariantT>>()
				);

			/**
			 * @brief Reverses the object-representation byte order of @p val.
			 * @tparam U Trivially copyable value type.
			 * @param[in] val Value whose bytes are reversed.
			 * @return @p val with endianness swapped.
			 *
			 * @note This **always** reverses bytes. Whether the caller should
			 *       invoke it (host ≠ little-endian) is Serializable's job.
			 *       Integrals go through `std::byteswap`; every other trivial
			 *       type goes through `std::bit_cast` + reverse so `float`,
			 *       `enum` and small POD structs stay defined.
			 *
			 * @warning Not a format detector. No BOM, no `std::endian` test.
			 */
			template<typename U>
			requires std::is_trivially_copyable_v<U> && (sizeof(U) > 0)
			constexpr U swap_endian(U val) noexcept {
				if constexpr (sizeof(U) == 1) {
					return val;
				} else if constexpr (std::integral<U>) {
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
		}

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
		 * @brief Exactly `std::optional<U>` for some `U`.
		 * @tparam T Type to test (no decay).
		 *
		 * A type that merely has `value_type` does not match.
		 *
		 * @code
		 * template<Type::Optional T>
		 * void process(T opt);
		 * @endcode
		 */
		template<typename T>
		concept Optional =
			requires { typename T::value_type; } &&
			std::same_as<T, std::optional<typename T::value_type>>;

		/**
		 * @brief Type with accessible `first` and `second` members.
		 * @tparam T Type to test.
		 *
		 * Broader than `std::pair`: any struct with those members matches.
		 * That is intentional and must stay that way.
		 *
		 * @code
		 * template<Type::Pair T>
		 * void process(T pair);
		 * @endcode
		 */
		template<typename T>
		concept Pair = requires(T t) {
			t.first;
			t.second;
		};

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
			std::is_unsigned_v<std::underlying_type_t<std::remove_cv_t<E>>>;

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

		/**
		 * @brief Raw (possibly cv-qualified) pointer type. Not a smart pointer.
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
		 * @brief Integral type (`bool`, `char`, `int`, `long`, …), including cv.
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
		 * @brief Signed arithmetic type (`std::is_signed`).
		 * @tparam T Type to test.
		 *
		 * @note Floating-point types are signed. `bool` is not.
		 *
		 * @code
		 * template<Type::Signed T>
		 * T negate(T value) { return -value; }
		 * @endcode
		 */
		template<typename T>
		concept Signed = std::is_signed_v<T>;

		/**
		 * @brief Unsigned arithmetic type (`std::is_unsigned`).
		 * @tparam T Type to test.
		 *
		 * @code
		 * template<Type::Unsigned T>
		 * bool non_negative(T value) { return value >= T{0}; }
		 * @endcode
		 */
		template<typename T>
		concept Unsigned = std::is_unsigned_v<T>;

		/**
		 * @brief Top-level `const` qualifier (`std::is_const_v`).
		 * @tparam T Type to test as written.
		 *
		 * `const int` matches. `int&` and `const int&` do not: a reference
		 * type is never `const` itself.
		 *
		 * @code
		 * template<Type::Const T>
		 * void read_only(T value);
		 * @endcode
		 */
		template<typename T>
		concept Const = std::is_const_v<T>;

		/**
		 * @brief Class or struct type (not union, not enum).
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
		 * @brief Instantiation of `std::variant`, after stripping cv/ref.
		 * @tparam T Type to test.
		 *
		 * @code
		 * template<Type::Variant T>
		 * void process(T var);
		 * @endcode
		 */
		template<typename T>
		concept Variant = Detail::is_variant_v<std::remove_cvref_t<T>>;

		/**
		 * @brief @ref Variant @p T that lists @p U among its alternatives.
		 * @tparam T Variant type (cv/ref stripped before the lookup).
		 * @tparam U Alternative to look for (cv/ref stripped).
		 *
		 * @code
		 * template<typename T, typename U>
		 * requires Type::VariantHasType<T, U>
		 * void handle_variant(T var);
		 * @endcode
		 */
		template<typename T, typename U>
		concept VariantHasType =
			Variant<T> &&
			Detail::variant_has_type_v<std::remove_cvref_t<T>, U>;

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
		 * @note Not `std::same_as`: that does not strip. `int` and `const int&`
		 *       match here.
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
		 * @brief @p From is implicitly convertible to @p To (`std::is_convertible`).
		 * @tparam From Source type.
		 * @tparam To Destination type.
		 *
		 * @note Not `std::convertible_to`: that also requires an explicit
		 *       `To` construct from `From` and would tighten the contract.
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

	/**
	 * @brief Endian swap used by @ref Serializable on big-endian hosts.
	 *
	 * Alias of @ref Type::Detail::swap_endian so existing call sites that
	 * include only this header keep compiling until Serializable is updated.
	 */
	using Type::Detail::swap_endian;
}
