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
 * @brief Main namespace for the StormByte library.
 *
 * The `StormByte` namespace serves as the root for all components and utilities in the StormByte library.
 * It provides foundational classes and tools for building robust, thread-safe, and efficient applications.
 */
namespace StormByte {
	/**
	 * @brief Concept to check if a type is a valid smart pointer
	 * @tparam SmartPointer smart pointer type
	 * @tparam T type
	 */
	template<typename SmartPointer, typename T>
	concept ValidSmartPointer = 
		std::same_as<SmartPointer, std::shared_ptr<T>> || 
		std::same_as<SmartPointer, std::unique_ptr<T>>;

	/**
	 * @class Clonable
	 * @brief A class that can be cloned
	 */
	template<class T, typename SmartPointer = std::shared_ptr<T>>
	requires ValidSmartPointer<SmartPointer, T> class Clonable {
		public:
			using PointerType	= SmartPointer;	///< Pointer type
			/**
			 * Makes a pointer
			 * @tparam Target The type of the object to create a pointer for (can be `T` or a derived type).
			 * @tparam Args The constructor arguments for `Target`.
			 * @param args Arguments to forward to `Target`'s constructor.
			 * @return A smart pointer of type `PointerType` managing the newly created `Target`.
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
			 * Constructor
			 */
			constexpr Clonable() 											= default;

			/**
			 * Copy constructor
			 */
			constexpr Clonable(const Clonable&) 							= default;

			/**
			 * Move constructor
			 */
			constexpr Clonable(Clonable&&) noexcept							= default;

			/**
			 * Assignment operator
			 */
			constexpr Clonable& operator=(const Clonable&) 					= default;

			/**
			 * Move assignment operator
			 */
			constexpr Clonable& operator=(Clonable&&) noexcept				= default;

			/**
			 * Destructor
			 */
			virtual constexpr ~Clonable() noexcept 							= default;

			/**
			 * Clones the object
			 * @return cloned object
			 */
			virtual PointerType Clone() const 								= 0;

			/**
			 * Moves the object
			 * @return moved object
			 */
			virtual PointerType Move() 										= 0;
	};
}
