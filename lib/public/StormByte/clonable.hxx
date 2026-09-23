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

#include <concepts>
#include <memory>

/**
 * @namespace StormByte
 * @brief Root namespace of the StormByte suite.
 */
namespace StormByte {
	/**
	 * @brief `true` when @p SmartPointer is `shared_ptr<T>` or `unique_ptr<T>`.
	 * @tparam SmartPointer Candidate pointer type.
	 * @tparam T Pointee type.
	 */
	template<typename SmartPointer, typename T>
	concept ValidSmartPointer =
		std::same_as<SmartPointer, std::shared_ptr<T>> ||
		std::same_as<SmartPointer, std::unique_ptr<T>>;

	/**
	 * @class Clonable
	 * @brief Polymorphic clone/move into a smart pointer of type @p T.
	 * @tparam T Most-derived interface stored in the pointer.
	 * @tparam SmartPointer `std::shared_ptr<T>` (default) or `std::unique_ptr<T>`.
	 */
	template<class T, typename SmartPointer = std::shared_ptr<T>>
	requires ValidSmartPointer<SmartPointer, T> class Clonable {
		public:
			using PointerType	= SmartPointer;	///< `SmartPointer` alias.

			/**
			 * @brief Allocates @p Target and returns it as `PointerType`.
			 * @tparam Target Concrete type (`T` or derived).
			 * @tparam Args Constructor argument types.
			 * @param args Forwarded to `Target`.
			 * @return `shared_ptr` or `unique_ptr` according to `PointerType`.
			 */
			template<class Target, typename... Args>
			static PointerType MakePointer(Args&&... args);

			/**
			 * @brief Default constructor.
			 */
			constexpr Clonable() 											= default;

			/**
			 * @brief Copy constructor.
			 */
			constexpr Clonable(const Clonable&) 							= default;

			/**
			 * @brief Move constructor.
			 */
			constexpr Clonable(Clonable&&) noexcept							= default;

			/**
			 * @brief Copy assignment.
			 */
			constexpr Clonable& operator=(const Clonable&) 					= default;

			/**
			 * @brief Move assignment.
			 */
			constexpr Clonable& operator=(Clonable&&) noexcept				= default;

			/**
			 * @brief Destructor.
			 */
			virtual constexpr ~Clonable() noexcept 							= default;

			/**
			 * @brief Deep copy into a new `PointerType`.
			 * @return Owning pointer to the clone.
			 */
			virtual PointerType Clone() const 								= 0;

			/**
			 * @brief Moves this object into a new `PointerType`.
			 * @return Owning pointer to the moved instance.
			 */
			virtual PointerType Move() 										= 0;
	};
}

#include <StormByte/clonable.txx>
