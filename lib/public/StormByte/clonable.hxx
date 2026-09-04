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
			static PointerType MakePointer(Args&&... args) {
				if constexpr (std::is_same_v<PointerType, std::shared_ptr<T>>) {
					return std::make_shared<Target>(std::forward<Args>(args)...);
				} else if constexpr (std::is_same_v<PointerType, std::unique_ptr<T>>) {
					return std::make_unique<Target>(std::forward<Args>(args)...);
				} else {
					static_assert(false, "Unsupported smart pointer type");
				}
			}

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
