#pragma once

#include <StormByte/visibility.h>
#include <string>
#include <format>

/**
 * @namespace StormByte
 * @brief Main namespace for the StormByte library.
 *
 * The `StormByte` namespace serves as the root for all components and utilities in the StormByte library.
 * It provides foundational classes and tools for building robust, thread-safe, and efficient applications.
 */
namespace StormByte {
	/**
	 * @class Exception
	 * @brief Base class for exceptions in the StormByte library.
	 *
	 * The `Exception` class provides a consistent mechanism for handling errors across library boundaries.
	 * It ensures compatibility with both Windows and Linux platforms and avoids issues with `std::string`
	 * across DLL boundaries by using `const char*` for internal storage.
	 */
	class STORMBYTE_PUBLIC Exception {
	public:
		/**
		 * @brief Constructor
		 * @param message Exception message as a `std::string`
		 */
		explicit Exception(const std::string& message);

		/**
		 * @brief Constructor
		 * @param message Exception message as a moved `std::string`
		 */
		explicit Exception(std::string&& message);

		/**
		 * @brief Constructor forwards the message to the `std::format` function
		 * @tparam Args Format argument types
		 * @param fmt Format string
		 * @param args Arguments for formatting
		 * 
		 * If no arguments are provided, the format string is used as the exception message directly.
		 */
		template <typename... Args>
		Exception(std::format_string<Args...> fmt, Args&&... args) {
			if constexpr (sizeof...(Args) == 0) {
				// No arguments provided, use the format string directly
				m_what = copy_str(fmt);
			} else {
				// Format the message with the provided arguments
				std::string formatted_message = std::format(fmt, std::forward<Args>(args)...);
				m_what = copy_str(formatted_message.c_str());
			}
		}

		/**
		 * @brief Copy constructor
		 * @param e Exception to copy
		 */
		Exception(const Exception& e);

		/**
		 * @brief Move constructor
		 * @param e Exception to move
		 */
		Exception(Exception&& e) noexcept;

		/**
		 * @brief Destructor
		 */
		virtual ~Exception() noexcept;

		/**
		 * @brief Copy assignment operator
		 * @param e Exception to copy
		 * @return Reference to this exception
		 */
		Exception& operator=(const Exception& e);

		/**
		 * @brief Move assignment operator
		 * @param e Exception to move
		 * @return Reference to this exception
		 */
		Exception& operator=(Exception&& e) noexcept;

		/**
		 * @brief Gets the exception message
		 * @return Exception message as `const char*`
		 */
		virtual const char* 									what() const noexcept;

	private:
		const char* m_what; 									///< Exception message

		/**
		 * @brief Copies a string to internal storage
		 * @param str String to copy
		 * @return Copied string as `const char*`
		 */
		const char* 											copy_str(const char* str) noexcept;

		/**
		 * @brief Frees the allocated string from internal storage
		 */
		void 													free_str() noexcept;
	};

	class STORMBYTE_PUBLIC DeserializeError: public Exception {
		public:
			using Exception::Exception;
	};
}
