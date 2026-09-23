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

#include <type_traits>

/**
 * @namespace StormByte
 * @brief Root namespace of the StormByte suite.
 */
namespace StormByte {
	/**
	 * @namespace Type
	 * @brief Named concepts and small type utilities used across the suite.
	 */
	namespace Type {
		/**
		 * @defgroup TypeConversions Type conversion concepts
		 * @brief Implicit and explicit convertibility predicates.
		 * @{
		 */

		/**
		 * @name Conversion
		 * @{
		 */

		/**
		 * @brief @p From is implicitly convertible to @p To (`std::is_convertible`).
		 * @tparam From Source type.
		 * @tparam To Destination type.
		 *
		 * Not `std::convertible_to`: that also requires an explicit
		 * `To` construct from `From` and would tighten the contract.
		 */
		template<typename From, typename To>
		concept ConvertibleTo = std::is_convertible_v<From, To>;

		/**
		 * @brief @p From may be explicitly converted to @p To with `static_cast`.
		 * @tparam From Source type.
		 * @tparam To Destination type.
		 *
		 * Accepts both implicit and explicit conversions. Unlike
		 * @ref ConvertibleTo, this matches types with an explicit conversion
		 * operator or constructor.
		 */
		template<typename From, typename To>
		concept ExplicitlyConvertibleTo = requires(From value) {
			static_cast<To>(value);
		};

		/** @} */
		/** @} */
	}
}
