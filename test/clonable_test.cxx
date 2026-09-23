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

#include <StormByte/clonable.hxx>
#include <StormByte/test_handlers.h>

#include <memory>
#include <utility>

using namespace StormByte;

namespace {
	class SharedItem: public Clonable<SharedItem> {
		public:
			explicit SharedItem(int value): value(value) {}

			PointerType Clone() const override {
				return MakePointer<SharedItem>(value);
			}

			PointerType Move() override {
				return MakePointer<SharedItem>(std::move(*this));
			}

			int value;
	};

	class UniqueItem: public Clonable<UniqueItem, std::unique_ptr<UniqueItem>> {
		public:
			explicit UniqueItem(int value): value(value) {}

			PointerType Clone() const override {
				return MakePointer<UniqueItem>(value);
			}

			PointerType Move() override {
				return MakePointer<UniqueItem>(std::move(*this));
			}

			int value;
	};
}

// -------------------
// Shared
// -------------------

int test_shared_clone_copies() {
	int result = 0;
	SharedItem original(42);
	auto clone = original.Clone();
	ASSERT_TRUE("test_shared_clone_copies", clone != nullptr);
	ASSERT_EQUAL("test_shared_clone_copies", 42, clone->value);
	ASSERT_TRUE("test_shared_clone_copies", clone.get() != &original);
	RETURN_TEST("test_shared_clone_copies", result);
}

int test_shared_clone_is_independent() {
	int result = 0;
	SharedItem original(1);
	auto clone = original.Clone();
	clone->value = 99;
	ASSERT_EQUAL("test_shared_clone_is_independent", 1, original.value);
	ASSERT_EQUAL("test_shared_clone_is_independent", 99, clone->value);
	RETURN_TEST("test_shared_clone_is_independent", result);
}

int test_shared_move() {
	int result = 0;
	SharedItem original(7);
	auto moved = original.Move();
	ASSERT_TRUE("test_shared_move", moved != nullptr);
	ASSERT_EQUAL("test_shared_move", 7, moved->value);
	ASSERT_TRUE("test_shared_move", moved.get() != &original);
	RETURN_TEST("test_shared_move", result);
}

// -------------------
// Unique
// -------------------

int test_unique_clone_copies() {
	int result = 0;
	UniqueItem original(42);
	auto clone = original.Clone();
	ASSERT_TRUE("test_unique_clone_copies", clone != nullptr);
	ASSERT_EQUAL("test_unique_clone_copies", 42, clone->value);
	ASSERT_TRUE("test_unique_clone_copies", clone.get() != &original);
	RETURN_TEST("test_unique_clone_copies", result);
}

int test_unique_clone_is_independent() {
	int result = 0;
	UniqueItem original(1);
	auto clone = original.Clone();
	clone->value = 99;
	ASSERT_EQUAL("test_unique_clone_is_independent", 1, original.value);
	ASSERT_EQUAL("test_unique_clone_is_independent", 99, clone->value);
	RETURN_TEST("test_unique_clone_is_independent", result);
}

int test_unique_move() {
	int result = 0;
	UniqueItem original(7);
	auto moved = original.Move();
	ASSERT_TRUE("test_unique_move", moved != nullptr);
	ASSERT_EQUAL("test_unique_move", 7, moved->value);
	ASSERT_TRUE("test_unique_move", moved.get() != &original);
	RETURN_TEST("test_unique_move", result);
}

int main() {
	int result = 0;

	// -------------------
	// Shared
	// -------------------
	result += test_shared_clone_copies();
	result += test_shared_clone_is_independent();
	result += test_shared_move();

	// -------------------
	// Unique
	// -------------------
	result += test_unique_clone_copies();
	result += test_unique_clone_is_independent();
	result += test_unique_move();

	if (result == 0)
		std::cout << "All tests passed!" << std::endl;
	else
		std::cout << result << " tests failed." << std::endl;
	return result;
}
