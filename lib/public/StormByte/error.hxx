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

#include <StormByte/visibility.h>

#include <string>
#include <system_error>

/**
 * @namespace StormByte
 * @brief Main namespace for the StormByte library.
 *
 * The `StormByte` namespace serves as the root for all components and utilities in the StormByte library.
 * It provides foundational classes and tools for building robust, thread-safe, and efficient applications.
 */
namespace StormByte {
	/**
	 * @namespace Error
	 * @brief Namespace for error handling in the StormByte library.
	 *
	 * The `StormByte::Error` namespace contains definitions and utilities related to error handling,
	 * including error codes and custom error categories used throughout the StormByte library.
	 */
	namespace Error {
		/**
		 * @enum Code
		 * @brief Enumeration of error codes used in the StormByte library.
		 *
		 * The `Code` enum defines various error codes that can be used throughout the StormByte library
		 * to represent different error conditions in a standardized way.
		 */
		enum class Code {
			
		};

		/**
		 * @class Category
		 * @brief Error category for StormByte errors.
		 *
		 * The `Category` class extends `std::error_category` to provide a custom category
		 * for errors defined in the StormByte library. This allows for better integration with
		 * the C++ standard library's error handling mechanisms.
		 */
		class STORMBYTE_PUBLIC Category: public std::error_category {
			public:
				/**
				 * @brief Get the name of the error category.
				 * @return Name of the error category.
				 */
				const char* name() const noexcept override;

				/**
				 * @brief Get the message corresponding to an error value.
				 * @param ev Error value.
				 * @return Corresponding error message.
				 */
				std::string message(int ev) const override;

				/**
				 * @brief Get the default error condition for an error value.
				 * @param ev Error value.
				 * @return Corresponding error condition.
				 */
				std::error_condition default_error_condition(int ev) const noexcept override;
		};

		/**
		 * @brief Get the singleton instance of the error category.
		 * @return Reference to the error category instance.
		 */
		STORMBYTE_PUBLIC const class Category& category() noexcept;
	}

	STORMBYTE_PUBLIC std::error_code make_error_code(Error::Code e);
}

namespace std {
	template<> struct is_error_code_enum<StormByte::Error::Code>: true_type {};
}