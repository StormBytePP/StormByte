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

#include <utility>

// Out-of-line implementation of StormByte::Clonable, included by clonable.hxx.
// See clonable.hxx for documentation of each member.

namespace StormByte {
	template<class T, typename SmartPointer>
	requires ValidSmartPointer<SmartPointer, T>
	template<class Target, typename... Args>
	typename Clonable<T, SmartPointer>::PointerType Clonable<T, SmartPointer>::MakePointer(Args&&... args) {
		if constexpr (std::is_same_v<PointerType, std::shared_ptr<T>>) {
			return std::make_shared<Target>(std::forward<Args>(args)...);
		} else if constexpr (std::is_same_v<PointerType, std::unique_ptr<T>>) {
			return std::make_unique<Target>(std::forward<Args>(args)...);
		} else {
			static_assert(false, "Unsupported smart pointer type");
		}
	}
}
