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

#include <StormByte/visibility.h>
#include <string>
#include <format>

/**
 * @namespace StormByte
 * @brief Root namespace of the StormByte suite.
 */
namespace StormByte {
	/**
	 * @class Exception
	 * @brief Base exception type for the suite.
	 *
	 * Stores the message as `const char*` so a `std::string` does not cross a DLL boundary on Windows.
	 */
	class STORMBYTE_PUBLIC Exception {
	public:
		/**
		 * @brief Constructs from a string.
		 * @param message Exception text.
		 */
		explicit Exception(const std::string& message);

		/**
		 * @brief Constructs from a moved string.
		 * @param message Exception text.
		 */
		explicit Exception(std::string&& message);

		/**
		 * @brief Constructs with `std::format`.
		 * @tparam Args Format argument types.
		 * @param fmt Format string.
		 * @param args Format arguments.
		 * @note With zero arguments the format string is the message as-is.
		 */
		template <typename... Args>
		Exception(std::format_string<Args...> fmt, Args&&... args) {
			if constexpr (sizeof...(Args) == 0) {
				m_what = copy_str(fmt);
			} else {
				std::string formatted_message = std::format(fmt, std::forward<Args>(args)...);
				m_what = copy_str(formatted_message.c_str());
			}
		}

		/**
		 * @brief Constructs with a component prefix and `std::format`.
		 * @tparam Args Format argument types.
		 * @param component Module name inserted after `StormByte::`.
		 * @param fmt Format string.
		 * @param args Format arguments.
		 * @note Final text is `StormByte::<component>: <formatted>`.
		 */
		template <typename... Args>
		Exception(const std::string& component, std::format_string<Args...> fmt, Args&&... args) {
			std::string formatted_message = std::format(fmt, std::forward<Args>(args)...);
			std::string full_message = "StormByte::" + component + ": " + formatted_message;
			m_what = copy_str(full_message.c_str());
		}

		/**
		 * @brief Copy constructor.
		 * @param e Exception to copy.
		 */
		Exception(const Exception& e);

		/**
		 * @brief Move constructor.
		 * @param e Exception to move.
		 */
		Exception(Exception&& e) noexcept;

		/**
		 * @brief Destructor.
		 */
		virtual ~Exception() noexcept;

		/**
		 * @brief Copy assignment.
		 * @param e Exception to copy.
		 * @return Reference to this exception.
		 */
		Exception& operator=(const Exception& e);

		/**
		 * @brief Move assignment.
		 * @param e Exception to move.
		 * @return Reference to this exception.
		 */
		Exception& operator=(Exception&& e) noexcept;

		/**
		 * @brief Message pointer.
		 * @return NUL-terminated message; owned by this object.
		 */
		virtual const char* 									what() const noexcept;

	private:
		const char* m_what; 									///< Owned message buffer.

		/**
		 * @brief Copies a C string into the owned buffer.
		 * @param str Source string.
		 * @return Pointer to the owned copy.
		 */
		const char* 											copy_str(const char* str) noexcept;

		/**
		 * @brief Releases the owned buffer.
		 */
		void 													free_str() noexcept;
	};

	/**
	 * @class DeserializeError
	 * @brief Thrown when deserialization fails.
	 */
	class STORMBYTE_PUBLIC DeserializeError: public Exception {
		public:
			using Exception::Exception;
	};

	/**
	 * @class OutOfBoundsError
	 * @brief Thrown when an index or range is out of bounds.
	 */
	class STORMBYTE_PUBLIC OutOfBoundsError: public Exception {
		public:
			using Exception::Exception;
	};

	/**
	 * @class Base64Error
	 * @brief Thrown when Base64 encode or decode fails.
	 */
	class STORMBYTE_PUBLIC Base64Error: public Exception {
		public:
			using Exception::Exception;
	};
}
