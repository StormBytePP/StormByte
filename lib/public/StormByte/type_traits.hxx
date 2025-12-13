#pragma once

#include <concepts>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>

/**
 * @namespace StormByte
 * @brief Main namespace for the StormByte library.
 *
 * The `StormByte` namespace serves as the root for all components and utilities in the StormByte library.
 * It provides foundational classes and tools for building robust, thread-safe, and efficient applications.
 */
namespace StormByte {
	// Implementation detail: type traits for internal use only
	namespace {
		template<typename T>
		struct is_string : std::bool_constant<std::is_same_v<T, std::string> || std::is_same_v<T, std::wstring>> {};

		template<typename T, typename _ = void>
		struct is_container : std::false_type {};

		template<typename T>
		struct is_container<T, std::void_t<decltype(std::declval<T>().begin()), decltype(std::declval<T>().end()), typename T::value_type>> 
			: std::bool_constant<!is_string<std::decay_t<T>>::value> {};

		template<typename T, typename _ = void>
		struct is_optional : std::false_type {};

		template<typename T>
		struct is_optional<T, std::void_t<typename T::value_type>> 
			: std::is_same<T, std::optional<typename T::value_type>> {};

		template<typename T, typename _ = void>
		struct is_pair : std::false_type {};

		template<typename T>
		struct is_pair<T, std::void_t<
			decltype(std::declval<T>().first), 
			decltype(std::declval<T>().second)
		>> : std::true_type {};

		template<typename T>
		struct is_variant : std::false_type {};

		template<typename... Ts>
		struct is_variant<std::variant<Ts...>> : std::true_type {};

		template<typename VariantT, typename U, std::size_t... I>
		constexpr bool variant_has_type_impl(std::index_sequence<I...>) noexcept {
			return ((std::is_same_v<std::remove_cvref_t<U>, std::remove_cvref_t<std::variant_alternative_t<I, VariantT>>>) || ...);
		}

		template<typename VariantT, typename U>
		struct variant_has_type : std::bool_constant<
			variant_has_type_impl<VariantT, U>(std::make_index_sequence<std::variant_size_v<VariantT>>())
		> {};
	}

	/**
	 * @namespace Type
	 * @brief Modern C++20 concepts for type checking.
	 *
	 * Provides clean, idiomatic concepts for compile-time type introspection.
	 * Use these instead of traditional type traits for template constraints and concept checking.
	 */
	namespace Type {
		/**
		 * @brief Concept to check if a type is std::string or std::wstring.
		 * @tparam T The type to check.
		 *
		 * A type satisfies String if it is exactly std::string or std::wstring.
		 * @code
		 * template<Type::String T>
		 * void process(T str) { ... }
		 * @endcode
		 */
		template<typename T>
		concept String = is_string<T>::value;

		/**
		 * @brief Concept to check if a type is a container (excluding strings).
		 * @tparam T The type to check.
		 *
		 * A type satisfies Container if it has `begin()`, `end()`, and a `value_type`,
		 * but excludes string types which are handled separately.
		 * @code
		 * template<Type::Container T>
		 * void process(T container) { ... }
		 * @endcode
		 */
		template<typename T>
		concept Container = is_container<T>::value;

		/**
		 * @brief Concept to check if a type is an optional.
		 * @tparam T The type to check.
		 *
		 * A type satisfies Optional if it is `std::optional<U>` for some type U.
		 * @code
		 * template<Type::Optional T>
		 * void process(T opt) { ... }
		 * @endcode
		 */
		template<typename T>
		concept Optional = is_optional<T>::value;

		/**
		 * @brief Concept to check if a type is a pair.
		 * @tparam T The type to check.
		 *
		 * A type satisfies Pair if it has `first` and `second` members.
		 * @code
		 * template<Type::Pair T>
		 * void process(T pair) { ... }
		 * @endcode
		 */
		template<typename T>
		concept Pair = is_pair<T>::value;

		/**
		 * @brief Concept to check if a type is a reference.
		 * @tparam T Type to check.
		 *
		 * A type satisfies Reference if it is an lvalue or rvalue reference.
		 * @code
		 * template<Type::Reference T>
		 * void process(T&& ref) { ... }
		 * @endcode
		 */
		template<typename T>
		concept Reference = std::is_reference_v<T>;

		/**
		 * @brief Concept to check if a type is an enumeration.
		 * @tparam E Type to check.
		 *
		 * A type satisfies Enum if it is an enumeration type (enum or enum class).
		 * @code
		 * template<Type::Enum E>
		 * void process(E value) { ... }
		 * @endcode
		 */
		template<typename E>
		concept Enum = std::is_enum_v<std::remove_cv_t<E>>;

		/**
		 * @brief Concept to check if a type is a scoped enumeration (enum class).
		 * @tparam E Type to check.
		 *
		 * A type satisfies ScopedEnum if it is a scoped enumeration (enum class).
		 * @code
		 * template<Type::ScopedEnum E>
		 * void process(E value) { ... }
		 * @endcode
		 */
		template<typename E>
		concept ScopedEnum = std::is_scoped_enum_v<E>;

		/**
		 * @brief Concept to check if a type is a pointer.
		 * @tparam T Type to check.
		 *
		 * A type satisfies Pointer if it is a pointer type (raw pointer).
		 * @code
		 * template<Type::Pointer T>
		 * void process(T ptr) { ... }
		 * @endcode
		 */
		template<typename T>
		concept Pointer = std::is_pointer_v<T>;

		/**
		 * @brief Concept to check if a type is an integral type.
		 * @tparam T Type to check.
		 *
		 * A type satisfies Integral if it is an integer type (int, long, short, etc.).
		 * @code
		 * template<Type::Integral T>
		 * T add(T a, T b) { return a + b; }
		 * @endcode
		 */
		template<typename T>
		concept Integral = std::is_integral_v<T>;

		/**
		 * @brief Concept to check if a type is a floating-point type.
		 * @tparam T Type to check.
		 *
		 * A type satisfies FloatingPoint if it is float, double, or long double.
		 * @code
		 * template<Type::FloatingPoint T>
		 * T multiply(T a, T b) { return a * b; }
		 * @endcode
		 */
		template<typename T>
		concept FloatingPoint = std::is_floating_point_v<T>;

		/**
		 * @brief Concept to check if a type is arithmetic.
		 * @tparam T Type to check.
		 *
		 * A type satisfies Arithmetic if it is an integral or floating-point type.
		 * @code
		 * template<Type::Arithmetic T>
		 * T compute(T x) { return x * 2; }
		 * @endcode
		 */
		template<typename T>
		concept Arithmetic = std::is_arithmetic_v<T>;

		/**
		 * @brief Concept to check if a type is signed.
		 * @tparam T Type to check.
		 *
		 * A type satisfies Signed if it is a signed arithmetic type.
		 * @code
		 * template<Type::Signed T>
		 * T negate(T value) { return -value; }
		 * @endcode
		 */
		template<typename T>
		concept Signed = std::is_signed_v<T>;

		/**
		 * @brief Concept to check if a type is unsigned.
		 * @tparam T Type to check.
		 *
		 * A type satisfies Unsigned if it is an unsigned arithmetic type.
		 * @code
		 * template<Type::Unsigned T>
		 * bool check(T value) { return value >= 0; }
		 * @endcode
		 */
		template<typename T>
		concept Unsigned = std::is_unsigned_v<T>;

		/**
		 * @brief Concept to check if a type is const-qualified.
		 * @tparam T Type to check.
		 *
		 * A type satisfies Const if it is const-qualified.
		 * @code
		 * template<Type::Const T>
		 * void read_only(T value) { ... }
		 * @endcode
		 */
		template<typename T>
		concept Const = std::is_const_v<T>;

		/**
		 * @brief Concept to check if a type is a class or struct.
		 * @tparam T Type to check.
		 *
		 * A type satisfies Class if it is a class or struct type.
		 * @code
		 * template<Type::Class T>
		 * void inspect(T obj) { ... }
		 * @endcode
		 */
		template<typename T>
		concept Class = std::is_class_v<T>;

		/**
		 * @brief Concept to check if a type is a variant.
		 * @tparam T Type to check.
		 *
		 * A type satisfies Variant if it is an instance of std::variant.
		 * @code
		 * template<Type::Variant T>
		 * void process(T var) { ... }
		 * @endcode
		 */
		template<typename T>
		concept Variant = is_variant<std::remove_cvref_t<T>>::value;

		/**
		 * @brief Concept to check if a variant type contains a specific type.
		 * @tparam T Variant type to check.
		 * @tparam U Type to look for within the variant.
		 *
		 * A type satisfies VariantHasType if U is one of the alternative types in the variant T.
		 * @code
		 * template<Type::VariantHasType<T, U> T, U>
		 * void handle_variant(T var) { ... }
		 * @endcode
		 */
		template<typename T, typename U>
		concept VariantHasType = Variant<T> && variant_has_type<std::remove_cvref_t<T>, U>::value;

		/**
		 * @brief Concept to check if a type is trivially copyable.
		 * @tparam T Type to check.
		 *
		 * A type satisfies TriviallyCopyable if it can be safely copied with memcpy.
		 * @code
		 * template<Type::TriviallyCopyable T>
		 * void fast_copy(T* dest, const T* src, size_t n) { std::memcpy(dest, src, n * sizeof(T)); }
		 * @endcode
		 */
		template<typename T>
		concept TriviallyCopyable = std::is_trivially_copyable_v<T>;

		/**
		 * @brief Concept to check if a type has a trivial destructor.
		 * @tparam T Type to check.
		 *
		 * A type satisfies TriviallyDestructible if its destructor is trivial (no cleanup needed).
		 * @code
		 * template<Type::TriviallyDestructible T>
		 * void pool_allocate(T* ptr) { ... }
		 * @endcode
		 */
		template<typename T>
		concept TriviallyDestructible = std::is_trivially_destructible_v<T>;

		/**
		 * @brief Concept to check if a type is default constructible.
		 * @tparam T Type to check.
		 *
		 * A type satisfies DefaultConstructible if it can be constructed without arguments.
		 * @code
		 * template<Type::DefaultConstructible T>
		 * T create() { return T{}; }
		 * @endcode
		 */
		template<typename T>
		concept DefaultConstructible = std::is_default_constructible_v<T>;

		/**
		 * @brief Concept to check if a type is copy constructible.
		 * @tparam T Type to check.
		 *
		 * A type satisfies CopyConstructible if it can be copy-constructed.
		 * @code
		 * template<Type::CopyConstructible T>
		 * T duplicate(const T& original) { return T{original}; }
		 * @endcode
		 */
		template<typename T>
		concept CopyConstructible = std::is_copy_constructible_v<T>;

		/**
		 * @brief Concept to check if a type is move constructible.
		 * @tparam T Type to check.
		 *
		 * A type satisfies MoveConstructible if it can be move-constructed.
		 * @code
		 * template<Type::MoveConstructible T>
		 * T transfer(T&& source) { return T{std::move(source)}; }
		 * @endcode
		 */
		template<typename T>
		concept MoveConstructible = std::is_move_constructible_v<T>;

		/**
		 * @brief Concept to check if a type is callable.
		 * @tparam F Type to check.
		 * @tparam Args Argument types for the call.
		 *
		 * A type satisfies Callable if it can be invoked with the given arguments.
		 * @code
		 * template<typename F, typename... Args>
		 * requires Type::Callable<F, Args...>
		 * auto invoke(F&& func, Args&&... args) { return func(std::forward<Args>(args)...); }
		 * @endcode
		 */
		template<typename F, typename... Args>
		concept Callable = std::is_invocable_v<F, Args...>;

		/**
		 * @brief Concept to check if two types are the same.
		 * @tparam T First type.
		 * @tparam U Second type.
		 *
		 * Types satisfy SameAs if they are identical after removing cv-qualifiers and references.
		 * @code
		 * template<typename T, typename U>
		 * requires Type::SameAs<T, U>
		 * void ensure_same_type(T a, U b) { ... }
		 * @endcode
		 */
		template<typename T, typename U>
		concept SameAs = std::is_same_v<std::remove_cvref_t<T>, std::remove_cvref_t<U>>;

		/**
		 * @brief Concept to check if one type is convertible to another.
		 * @tparam From Source type.
		 * @tparam To Target type.
		 *
		 * Types satisfy ConvertibleTo if From can be implicitly converted to To.
		 * @code
		 * template<typename From, typename To>
		 * requires Type::ConvertibleTo<From, To>
		 * To convert(From value) { return static_cast<To>(value); }
		 * @endcode
		 */
		template<typename From, typename To>
		concept ConvertibleTo = std::is_convertible_v<From, To>;
	}
}