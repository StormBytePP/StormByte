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


#include <array>
#include <bit>
#include <concepts>
#include <cstddef>
#include <functional>
#include <iterator>
#include <optional>
#include <ranges>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>

/**
 * @namespace StormByte
 * @brief Root namespace of the StormByte suite.
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

		/**
		 * @defgroup TypeWrappers Standard wrapper and aggregate concepts
		 * @brief Predicates for standard wrappers and pair-like aggregate values.
		 * @{
		 */

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
		/** @} */

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
		/** @} */

		/**
		 * @defgroup TypeConversions Type conversion concepts
		 * @brief Implicit and explicit convertibility predicates.
		 * @{
		 */

		/**
		 * @brief @p From is implicitly convertible to @p To (`std::is_convertible`).
		 * @tparam From Source type.
		 * @tparam To Destination type.
		 *
		 * @note Not `std::convertible_to`: that also requires an explicit
		 *       `To` construct from `From` and would tighten the contract.
		 */
		template<typename From, typename To>
		concept ConvertibleTo = std::is_convertible_v<From, To>;

		/**
		 * @brief @p From may be explicitly converted to @p To with `static_cast`.
		 * @tparam From Source type.
		 * @tparam To Destination type.
		 *
		 * @note Accepts both implicit and explicit conversions. Unlike
		 *       @ref ConvertibleTo, this matches types with an explicit conversion
		 *       operator or constructor.
		 */
		template<typename From, typename To>
		concept ExplicitlyConvertibleTo = requires(From value) {
			static_cast<To>(value);
		};
		/** @} */

		/**
		 * @defgroup TypeCategories Fundamental type category concepts
		 * @brief Thin wrappers around the basic `std::is_*` type category predicates.
		 * @{
		 */

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
		 * @brief Smart pointer-like type: dereferenceable and exposes `get()`.
		 * @tparam T Type to test (cv/ref ignored).
		 *
		 * Matches `std::unique_ptr`, `std::shared_ptr` and similar RAII
		 * wrappers without naming them directly. A raw @ref Pointer does
		 * not match: it has no `get()`.
		 *
		 * @code
		 * static_assert(Type::SmartPointer<std::shared_ptr<int>>);
		 * static_assert(!Type::SmartPointer<int*>);
		 * @endcode
		 */
		template<typename T>
		concept SmartPointer =
			!Pointer<std::remove_cvref_t<T>> &&
			requires(std::remove_cvref_t<T> const& p) {
				{ *p };
				{ p.operator->() };
				{ p.get() };
			};

		/**
		 * @brief Smart pointer-like type that can test whether it contains an object.
		 * @tparam T Type to test (cv/ref ignored).
		 *
		 * Requires dereference, member access, `get()`, and an implicit or explicit
		 * conversion to `bool`. Unlike @ref SmartPointer, this is suitable for APIs
		 * that evaluate `!pointer` before dereferencing it.
		 */
		template<typename T>
		concept NullablePointer =
			SmartPointer<T> &&
			ExplicitlyConvertibleTo<std::remove_cvref_t<T>, bool>;

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
		/** @} */

		/**
		 * @defgroup TypeRanges Range and iterator concepts
		 * @brief Range and iterator predicates forwarding to the standard ranges library.
		 *
		 * These concepts inspect their template arguments as written. Pass `R&` to
		 * preserve lvalue range semantics when needed.
		 * @{
		 */

		/** @brief Valid range (`std::ranges::range`). @tparam R Type to test. */
		template<typename R>
		concept Range = std::ranges::range<R>;

		/** @brief Input range (`std::ranges::input_range`). @tparam R Type to test. */
		template<typename R>
		concept InputRange = std::ranges::input_range<R>;

		/** @brief Output range for values of type @p T (`std::ranges::output_range`). @tparam R Type to test. @tparam T Value type to write. */
		template<typename R, typename T>
		concept OutputRange = std::ranges::output_range<R, T>;

		/** @brief Forward range (`std::ranges::forward_range`). @tparam R Type to test. */
		template<typename R>
		concept ForwardRange = std::ranges::forward_range<R>;

		/** @brief Bidirectional range (`std::ranges::bidirectional_range`). @tparam R Type to test. */
		template<typename R>
		concept BidirectionalRange = std::ranges::bidirectional_range<R>;

		/** @brief Random-access range (`std::ranges::random_access_range`). @tparam R Type to test. */
		template<typename R>
		concept RandomAccessRange = std::ranges::random_access_range<R>;

		/** @brief Contiguous range (`std::ranges::contiguous_range`). @tparam R Type to test. */
		template<typename R>
		concept ContiguousRange = std::ranges::contiguous_range<R>;

		/** @brief Sized range (`std::ranges::sized_range`). @tparam R Type to test. */
		template<typename R>
		concept SizedRange = std::ranges::sized_range<R>;

		/** @brief Common range (`std::ranges::common_range`). @tparam R Type to test. */
		template<typename R>
		concept CommonRange = std::ranges::common_range<R>;

		/** @brief Range view (`std::ranges::view`). @tparam R Type to test. */
		template<typename R>
		concept View = std::ranges::view<R>;

		/** @brief Borrowed range (`std::ranges::borrowed_range`). @tparam R Type to test. */
		template<typename R>
		concept BorrowedRange = std::ranges::borrowed_range<R>;

		/** @brief Input iterator (`std::input_iterator`). @tparam I Type to test. */
		template<typename I>
		concept InputIterator = std::input_iterator<I>;

		/** @brief Output iterator for values of type @p T (`std::output_iterator`). @tparam I Iterator type. @tparam T Value type to write. */
		template<typename I, typename T>
		concept OutputIterator = std::output_iterator<I, T>;

		/** @brief Forward iterator (`std::forward_iterator`). @tparam I Type to test. */
		template<typename I>
		concept ForwardIterator = std::forward_iterator<I>;

		/** @brief Bidirectional iterator (`std::bidirectional_iterator`). @tparam I Type to test. */
		template<typename I>
		concept BidirectionalIterator = std::bidirectional_iterator<I>;

		/** @brief Random-access iterator (`std::random_access_iterator`). @tparam I Type to test. */
		template<typename I>
		concept RandomAccessIterator = std::random_access_iterator<I>;

		/** @brief Contiguous iterator (`std::contiguous_iterator`). @tparam I Type to test. */
		template<typename I>
		concept ContiguousIterator = std::contiguous_iterator<I>;

		/** @brief Valid sentinel for an iterator (`std::sentinel_for`). @tparam S Sentinel type. @tparam I Iterator type. */
		template<typename S, typename I>
		concept SentinelFor = std::sentinel_for<S, I>;

		/** @brief Sized sentinel for an iterator (`std::sized_sentinel_for`). @tparam S Sentinel type. @tparam I Iterator type. */
		template<typename S, typename I>
		concept SizedSentinelFor = std::sized_sentinel_for<S, I>;

		/** @brief Value type of a valid range. @tparam R Range type. */
		template<Range R>
		using RangeValue = std::ranges::range_value_t<R>;

		/** @brief Reference type produced by a valid range. @tparam R Range type. */
		template<Range R>
		using RangeReference = std::ranges::range_reference_t<R>;

		/** @brief Difference type associated with a valid range. @tparam R Range type. */
		template<Range R>
		using RangeDifference = std::ranges::range_difference_t<R>;

		/** @brief Value type associated with an iterator. @tparam I Iterator type. */
		template<typename I>
		using IteratorValue = std::iter_value_t<I>;

		/**
		 * @brief Input range whose scalar values can be explicitly converted to `std::byte`.
		 * @tparam R Range type.
		 *
		 * Class-valued ranges are rejected. Conversion may be implicit or explicit.
		 */
		template<typename R>
		concept ByteInputRange =
			InputRange<R> &&
			!Class<RangeValue<R>> &&
			ExplicitlyConvertibleTo<RangeValue<R>, std::byte>;

		/**
		 * @brief Input iterator whose scalar values can be explicitly converted to `std::byte`.
		 * @tparam I Iterator type.
		 *
		 * Class-valued iterator values are rejected. Conversion may be implicit or explicit.
		 */
		template<typename I>
		concept ByteInputIterator =
			InputIterator<I> &&
			!Class<IteratorValue<I>> &&
			ExplicitlyConvertibleTo<IteratorValue<I>, std::byte>;
		/** @} */

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

		/**
		 * @defgroup TypeRelations Type relation concepts
		 * @brief Relationships between two (or more) types: invocability, equivalence, convertibility.
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
		 * @brief @p Derived derives from @p Base (`std::is_base_of`).
		 * @tparam Derived Candidate derived type.
		 * @tparam Base Candidate base type.
		 *
		 * @note Not `std::derived_from`: that also requires an unambiguous,
		 *       public base-to-derived conversion. This only checks the
		 *       inheritance relationship itself.
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

		/**
		 * @defgroup TypeComparison Comparison and hashing concepts
		 * @brief Equality, ordering and `std::hash` support for a type.
		 * @{
		 */

		/**
		 * @brief Type whose `==` and `!=` both yield something convertible to `bool`.
		 * @tparam T Type to test.
		 *
		 * @note Not `std::equality_comparable`: this only checks same-type
		 *       comparison, not the mixed-type / common-reference machinery.
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
	}

	/**
	 * @brief Endian swap used by @ref Serializable on big-endian hosts.
	 *
	 * Alias of @ref Type::Detail::swap_endian so existing call sites that
	 * include only this header keep compiling until Serializable is updated.
	 */
	using Type::Detail::swap_endian;
}
