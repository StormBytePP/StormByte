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
#include <system_error>

/**
 * @namespace StormByte
 * @brief Root namespace of the StormByte suite.
 */
namespace StormByte {
	/**
	 * @namespace StormByte::Error
	 * @brief `std::error_code` integration for suite-wide codes.
	 */
	namespace Error {
		/**
		 * @enum Code
		 * @brief Suite error codes (`std::error_code` enum).
		 * @note No enumerators are defined yet; the category and `make_error_code` are in place for when they are.
		 */
		enum class Code {
			
		};

		/**
		 * @class Category
		 * @brief `std::error_category` for `StormByte::Error::Code`.
		 */
		class STORMBYTE_PUBLIC Category: public std::error_category {
			public:
				/**
				 * @brief Category name.
				 * @return Stable C string identifying this category.
				 */
				const char* name() const noexcept override;

				/**
				 * @brief Message for an enumerator value.
				 * @param ev Integer value of `Code`.
				 * @return Human-readable message.
				 */
				std::string message(int ev) const override;

				/**
				 * @brief Default `std::error_condition` for an enumerator value.
				 * @param ev Integer value of `Code`.
				 * @return Matching condition.
				 */
				std::error_condition default_error_condition(int ev) const noexcept override;
		};

		/**
		 * @brief Process-wide category singleton.
		 * @return Reference to the suite category.
		 */
		STORMBYTE_PUBLIC const class Category& category() noexcept;
	}

	/**
	 * @brief Builds an `std::error_code` from `Error::Code`.
	 * @param e Suite error enumerator.
	 * @return `std::error_code` in `Error::category()`.
	 */
	STORMBYTE_PUBLIC std::error_code make_error_code(Error::Code e);
}

namespace std {
	/**
	 * @brief Marks `StormByte::Error::Code` as an `std::error_code` enum.
	 */
	template<> struct is_error_code_enum<StormByte::Error::Code>: true_type {};
}
