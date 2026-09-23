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
 * @namespace StormByte
 * @brief Root namespace of the StormByte suite.
 */
/**
 * @namespace StormByte::Type
 * @brief Named concepts and small type utilities used across the suite.
 *
 * Prefer these names in public templates. Do not reintroduce
 * `std::enable_if` / `void_t` traits next to them.
 *
 * Split by @defgroup across `StormByte/type_traits/*.hxx`. Each file
 * `#include`s the group files its own concepts build on (e.g. `detail.hxx`
 * on `categories.hxx`/`object_semantics.hxx`, `enums.hxx` on
 * `categories.hxx`, `ranges.hxx` on `categories.hxx`/`conversions.hxx`), so
 * the list below only needs to be exhaustive, not ordered. Add new concepts
 * to the matching group file, referencing another group's concept directly
 * (plus its `#include`) instead of repeating a raw `std::is_*` check; only
 * add a new file for a genuinely new group.
 */
#include <StormByte/type_traits/detail.hxx>
#include <StormByte/type_traits/containers.hxx>
#include <StormByte/type_traits/wrappers.hxx>
#include <StormByte/type_traits/enums.hxx>
#include <StormByte/type_traits/conversions.hxx>
#include <StormByte/type_traits/categories.hxx>
#include <StormByte/type_traits/ranges.hxx>
#include <StormByte/type_traits/object_semantics.hxx>
#include <StormByte/type_traits/relations.hxx>
#include <StormByte/type_traits/comparison.hxx>
