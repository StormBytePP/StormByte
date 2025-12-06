#pragma once

#include <StormByte/type_traits.hxx>

#include <expected>
#include <format>
#include <type_traits>
#include <memory>
#include <string>

/**
 * @namespace StormByte
 * @brief Main namespace for the StormByte library.
 *
 * The `StormByte` namespace serves as the root for all components and utilities in the StormByte library.
 * It provides foundational classes and tools for building robust, thread-safe, and efficient applications.
 */
namespace StormByte {
	/**
	 * @brief Expected type with support for reference types
	 * @tparam T Expected type
	 * @tparam E Error type
	 * 
	 * Defines an alias for `std::expected`, with special handling for reference types.
	 * For reference types, the `std::reference_wrapper` ensures safe and efficient storage,
	 * while the error type is managed using `std::shared_ptr` to support dynamic allocation.
	 */
	template <typename T, class E>
	using Expected = std::conditional_t<
		Type::Reference<T>,
		std::expected<std::reference_wrapper<std::remove_reference_t<T>>, std::shared_ptr<E>>,
		std::expected<T, std::shared_ptr<E>>
	>;

	/**
	 * @brief Creates an `std::unexpected` with a shared pointer to the error
	 * @tparam E Error type
	 * @param error_ptr A shared pointer to the error
	 * @return An `std::unexpected` object containing the shared pointer to the error
	 * 
	 * This overload is used when an existing `std::shared_ptr` to the error instance is available.
	 */
	template <typename E>
	auto Unexpected(std::shared_ptr<E> error_ptr) {
		return std::unexpected<std::shared_ptr<E>>(std::move(error_ptr));
	}

	/**
	 * @brief Creates an `std::unexpected` with a new error instance
	 * @tparam E Error type
	 * @param error Error instance to store
	 * @return An `std::unexpected` object containing a shared pointer to the new error instance
	 * 
	 * This overload constructs a new error instance by forwarding the provided argument.
	 */
	template <typename E>
	auto Unexpected(E&& error) {
		return std::unexpected<std::shared_ptr<std::decay_t<E>>>(
			std::make_shared<std::decay_t<E>>(std::forward<E>(error))
		);
	}

	/**
	 * @brief Construct an Unexpected<Base> from a Derived instance.
	 *
	 * Allows calling `Unexpected<Base>(Derived(...))` to create a
	 * `std::unexpected<std::shared_ptr<Base>>` that holds a `Derived`
	 * instance inside the `shared_ptr` and upcasts it to `Base`.
	 */
	template <typename Base, typename Derived>
	auto Unexpected(Derived&& error) -> std::unexpected<std::shared_ptr<Base>>
	requires std::is_base_of_v<Base, std::decay_t<Derived>>
	{
		using DerivedT = std::decay_t<Derived>;
		return std::unexpected<std::shared_ptr<Base>>(std::static_pointer_cast<Base>(std::make_shared<DerivedT>(std::forward<Derived>(error))));
	}

	/**
	 * @brief Creates an `std::unexpected` with a formatted error message
	 * @tparam E Error type
	 * @tparam Args Types of arguments to format the error message
	 * @param fmt The format string
	 * @param args Arguments to format the string
	 * @return An `std::unexpected` object containing a shared pointer to the error
	 * 
	 * Dynamically formats the error message using `std::vformat` and constructs the error instance with the formatted message.
	 * If no arguments are provided, it directly uses the format string as the error message.
	 */
	template <typename E, typename... Args>
	auto Unexpected(const std::string& fmt, Args&&... args) {
		std::string formatted_message;

		if constexpr (sizeof...(Args) == 0) {
			// No arguments provided, use the format string directly
			formatted_message = fmt;
		} else {
			// Create lvalue references for all arguments
			auto format_args = std::make_format_args(args...);

			// Use std::vformat for runtime formatting
			formatted_message = std::vformat(fmt, format_args);
		}

		// Construct the error instance with the formatted message
		return std::unexpected<std::shared_ptr<E>>(
			std::make_shared<E>(std::move(formatted_message))
		);
	}
}
