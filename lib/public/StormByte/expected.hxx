/*
 * Copyright (C) 2024-2026 David C. Manuelda (StormBytePP)
 *
 * This file is part of StormByte.
 *
 * StormByte original source is dual-licensed:
 *
 * 1. GNU Lesser General Public License v3.0 (or later)
 *    You may redistribute and/or modify this file under the terms of the
 *    GNU Lesser General Public License as published by the Free Software
 *    Foundation, either version 3 of the License, or (at your option)
 *    any later version.
 *
 * 2. Commercial license
 *    Alternatively, this file may be used under the terms of a commercial
 *    license agreement with the copyright holder
 *    (David C. Manuelda <StormByte@gmail.com>).
 *
 * Both licenses apply only to original StormByte source in this repository.
 * They do not cover other StormByte modules or any third-party material
 * shipped with this repository (including everything under thirdparty/),
 * which remains under its own license.
 *
 * Neither license grants any patent rights. Any patent licenses required
 * to use this software or third-party components must be obtained separately
 * from the patent holders.
 *
 * StormByte is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with StormByte. If not, see
 * <https://www.gnu.org/licenses/lgpl-3.0.html>.
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-StormByte-Commercial
 */

#pragma once

#include <StormByte/type_traits.hxx>

#include <expected>
#include <format>
#include <type_traits>
#include <memory>
#include <string>

/**
 * @namespace StormByte
 * @brief Root namespace of the StormByte suite.
 */
namespace StormByte {
	/**
	 * @brief `std::expected` alias with reference and shared-error handling.
	 * @tparam T Value type. References are stored as `std::reference_wrapper`.
	 * @tparam E Error type. Always stored as `std::shared_ptr<E>`.
	 */
	template <typename T, class E>
	using Expected = std::conditional_t<
		Type::Reference<T>,
		std::expected<std::reference_wrapper<std::remove_reference_t<T>>, std::shared_ptr<E>>,
		std::expected<T, std::shared_ptr<E>>
	>;

	/**
	 * @brief Builds `std::unexpected` from an existing error pointer.
	 * @tparam E Error type.
	 * @param error_ptr Shared pointer to the error.
	 * @return `std::unexpected` holding that pointer.
	 */
	template <typename E>
	auto Unexpected(std::shared_ptr<E> error_ptr) {
		return std::unexpected<std::shared_ptr<E>>(std::move(error_ptr));
	}

	/**
	 * @brief Builds `std::unexpected` by constructing `E`.
	 * @tparam E Error type.
	 * @param error Error instance (moved or copied into a `shared_ptr`).
	 * @return `std::unexpected` holding `shared_ptr<decay_t<E>>`.
	 */
	template <typename E>
	auto Unexpected(E&& error) {
		return std::unexpected<std::shared_ptr<std::decay_t<E>>>(
			std::make_shared<std::decay_t<E>>(std::forward<E>(error))
		);
	}

	/**
	 * @brief Builds `std::unexpected<shared_ptr<Base>>` from a `Derived` instance.
	 * @tparam Base Error base type stored in the pointer.
	 * @tparam Derived Concrete error type (`std::is_base_of_v<Base, Derived>`).
	 * @param error Derived instance to own.
	 * @return `std::unexpected` with an upcast `shared_ptr<Base>`.
	 */
	template <typename Base, typename Derived>
	auto Unexpected(Derived&& error) -> std::unexpected<std::shared_ptr<Base>>
	requires std::is_base_of_v<Base, std::decay_t<Derived>> &&
		(!std::same_as<Base, std::decay_t<Derived>>)
	{
		using DerivedT = std::decay_t<Derived>;
		return std::unexpected<std::shared_ptr<Base>>(std::static_pointer_cast<Base>(std::make_shared<DerivedT>(std::forward<Derived>(error))));
	}

	/**
	 * @brief Builds `std::unexpected` from a format string and `E(string)` constructor.
	 * @tparam E Error type.
	 * @tparam Args Format argument types.
	 * @param fmt Format string (used as-is when `Args` is empty).
	 * @param args Format arguments for `std::vformat`.
	 * @return `std::unexpected` holding `make_shared<E>(formatted)`.
	 */
	template <typename E, typename... Args>
	auto Unexpected(const std::string& fmt, Args&&... args) {
		std::string formatted_message;

		if constexpr (sizeof...(Args) == 0) {
			formatted_message = fmt;
		} else {
			auto format_args = std::make_format_args(args...);
			formatted_message = std::vformat(fmt, format_args);
		}

		return std::unexpected<std::shared_ptr<E>>(
			std::make_shared<E>(std::move(formatted_message))
		);
	}
}
