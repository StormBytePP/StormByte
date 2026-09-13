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

/**
 * @namespace StormByte::Type
 * @brief Named concepts and small type utilities used across the suite.
 *
 * Prefer these names in public templates. Do not reintroduce
 * `std::enable_if` / `void_t` traits next to them.
 *
 * Split by @defgroup across `StormByte/type_traits/*.hxx`, included below in
 * dependency order (a group that uses another group's concepts is included
 * after it). Add new concepts to the matching group file; only add a new
 * file for a genuinely new group.
 */
#include <StormByte/type_traits/detail.hxx>
#include <StormByte/type_traits/containers.hxx>
#include <StormByte/type_traits/wrappers.hxx>
#include <StormByte/type_traits/enums.hxx>
#include <StormByte/type_traits/conversions.hxx>
#include <StormByte/type_traits/categories.hxx>	// after conversions.hxx (NullablePointer)
#include <StormByte/type_traits/ranges.hxx>		// after categories.hxx + conversions.hxx (ByteInputRange/Iterator)
#include <StormByte/type_traits/object_semantics.hxx>
#include <StormByte/type_traits/relations.hxx>
#include <StormByte/type_traits/comparison.hxx>

/**
 * @namespace StormByte
 * @brief Root namespace of the StormByte suite.
 */
namespace StormByte {
	/**
	 * @brief Endian swap used by @ref Serializable on big-endian hosts.
	 *
	 * Alias of @ref Type::Detail::swap_endian so existing call sites that
	 * include only this header keep compiling until Serializable is updated.
	 */
	using Type::Detail::swap_endian;
}
