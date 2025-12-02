#pragma once

#include <optional>
#include <string>
#include <type_traits>

/**
 * @namespace StormByte
 * @brief Main namespace for the StormByte library.
 *
 * The `StormByte` namespace serves as the root for all components and utilities in the StormByte library.
 * It provides foundational classes and tools for building robust, thread-safe, and efficient applications.
 */
namespace StormByte {
	/**
	 * @brief Type trait to check if a type is a string-like type.
	 * @tparam T The type to check.
	 *
	 * A type is considered string-like if it's std::string, std::wstring, or related types.
	 */
	template<typename T>
	struct is_string : std::false_type {};

	template<>
	struct is_string<std::string> : std::true_type {};

	template<>
	struct is_string<std::wstring> : std::true_type {};

	template<>
	struct is_string<std::u16string> : std::true_type {};

	template<>
	struct is_string<std::u32string> : std::true_type {};

	/**
	 * @brief Type trait to check if a type is a container.
	 * @tparam T The type to check.
	 *
	 * A type is considered a container if it has `begin()`, `end()`, and a `value_type`,
	 * but excludes string types which are handled separately for performance.
	 */
	template<typename T, typename _ = void>
	struct is_container : std::false_type {};

	/**
	 * @brief Type trait specialization for containers (excluding strings).
	 * @tparam T The type to check.
	 */
	template<typename T>
	struct is_container<T, std::void_t<decltype(std::declval<T>().begin()), decltype(std::declval<T>().end()), typename T::value_type>> 
		: std::bool_constant<!is_string<std::decay_t<T>>::value> {};

	/**
	 * @brief Type trait to check if a type is an optional.
	 * @tparam T The type to check.
	 *
	 * A type is considered an optional if it is `std::optional`.
	 */
	template<typename T, typename _ = void>
	struct is_optional : std::false_type {};

	/**
	 * @brief Type trait specialization for `std::optional`.
	 * @tparam T The type to check.
	 */
	template<typename T>
	struct is_optional<T, std::void_t<typename T::value_type>> 
		: std::is_same<T, std::optional<typename T::value_type>> {};

	/**
	 * @brief Type trait to check if a type is a pair.
	 * @tparam T The type to check.
	 *
	 * A type is considered a pair if it has `first` and `second` members.
	 */
	template<typename T, typename _ = void>
	struct is_pair : std::false_type {};

	/**
	 * @brief Type trait specialization for pairs.
	 * @tparam T The type to check.
	 */
	template<typename T>
	struct is_pair<T, std::void_t<
		decltype(std::declval<T>().first), 
		decltype(std::declval<T>().second)
	>> : std::true_type {};

	/**
	 * @brief Type traits for checking if a type is a reference
	 * @tparam T Type to check
	 */
	template <typename T>
	struct is_reference {
		static constexpr bool value = std::is_reference_v<T>;
	};

	/**
	 * @brief Concept to check if a type is an enumeration
	 * 
	 * @tparam E Type to check
	 */
	template<typename E>
	concept EnumType = std::is_enum_v<std::remove_cv_t<E>>;
}