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

#include <StormByte/bitmask.hxx>
#include <StormByte/test_handlers.h>

#include <cstdint>

using namespace StormByte;

enum class MyFlags : uint8_t {
	FlagA = 0x01,
	FlagB = 0x02,
	FlagC = 0x04,
	FlagD = 0x08
};

class MyBitmask: public Bitmask<MyBitmask, MyFlags> {
	public:
		using Bitmask<MyBitmask, MyFlags>::Bitmask;
};

// -------------------
// Construct
// -------------------

int test_default_is_empty() {
	int result = 0;
	MyBitmask mask;
	ASSERT_EQUAL("test_default_is_empty", static_cast<MyFlags>(0), mask.Value());
	RETURN_TEST("test_default_is_empty", result);
}

int test_construct_from_flag() {
	int result = 0;
	MyBitmask mask(MyFlags::FlagA);
	ASSERT_EQUAL("test_construct_from_flag", MyFlags::FlagA, mask.Value());
	RETURN_TEST("test_construct_from_flag", result);
}

int test_construct_from_combined() {
	int result = 0;
	MyBitmask mask(MyFlags::FlagA | MyFlags::FlagB);
	ASSERT_EQUAL("test_construct_from_combined", MyFlags::FlagA | MyFlags::FlagB, mask.Value());
	RETURN_TEST("test_construct_from_combined", result);
}

int test_copy_independent() {
	int result = 0;
	MyBitmask original(MyFlags::FlagA);
	MyBitmask copy(original);
	copy.Add(MyFlags::FlagB);
	ASSERT_EQUAL("test_copy_independent", MyFlags::FlagA, original.Value());
	ASSERT_EQUAL("test_copy_independent", MyFlags::FlagA | MyFlags::FlagB, copy.Value());
	RETURN_TEST("test_copy_independent", result);
}

// -------------------
// Operators
// -------------------

int test_or_union() {
	int result = 0;
	MyBitmask a(MyFlags::FlagA);
	MyBitmask b(MyFlags::FlagB);
	MyBitmask c = a | b;
	ASSERT_EQUAL("test_or_union", MyFlags::FlagA | MyFlags::FlagB, c.Value());
	RETURN_TEST("test_or_union", result);
}

int test_and_intersection() {
	int result = 0;
	MyBitmask left(MyFlags::FlagA | MyFlags::FlagB | MyFlags::FlagC);
	MyBitmask right(MyFlags::FlagA | MyFlags::FlagC);
	MyBitmask both = left & right;
	ASSERT_EQUAL("test_and_intersection", MyFlags::FlagA | MyFlags::FlagC, both.Value());
	RETURN_TEST("test_and_intersection", result);
}

int test_xor_toggle() {
	int result = 0;
	MyBitmask left(MyFlags::FlagA | MyFlags::FlagC);
	MyBitmask right(MyFlags::FlagC);
	MyBitmask xored = left ^ right;
	ASSERT_EQUAL("test_xor_toggle", MyFlags::FlagA, xored.Value());
	RETURN_TEST("test_xor_toggle", result);
}

int test_not_clears_flag() {
	int result = 0;
	MyBitmask a(MyFlags::FlagA);
	MyBitmask inverted = ~a;
	ASSERT_TRUE("test_not_clears_flag", (inverted.Value() & MyFlags::FlagA) == static_cast<MyFlags>(0));
	ASSERT_TRUE("test_not_clears_flag", inverted.Has(MyFlags::FlagB));
	RETURN_TEST("test_not_clears_flag", result);
}

int test_or_assign() {
	int result = 0;
	MyBitmask mask(MyFlags::FlagA);
	mask |= MyBitmask(MyFlags::FlagC);
	ASSERT_EQUAL("test_or_assign", MyFlags::FlagA | MyFlags::FlagC, mask.Value());
	RETURN_TEST("test_or_assign", result);
}

int test_and_assign() {
	int result = 0;
	MyBitmask mask(MyFlags::FlagA | MyFlags::FlagB | MyFlags::FlagC);
	mask &= MyBitmask(MyFlags::FlagA | MyFlags::FlagC);
	ASSERT_EQUAL("test_and_assign", MyFlags::FlagA | MyFlags::FlagC, mask.Value());
	RETURN_TEST("test_and_assign", result);
}

int test_xor_assign() {
	int result = 0;
	MyBitmask mask(MyFlags::FlagA | MyFlags::FlagC);
	mask ^= MyBitmask(MyFlags::FlagC);
	ASSERT_EQUAL("test_xor_assign", MyFlags::FlagA, mask.Value());
	RETURN_TEST("test_xor_assign", result);
}

int test_equality() {
	int result = 0;
	MyBitmask a(MyFlags::FlagA | MyFlags::FlagB);
	MyBitmask b(MyFlags::FlagA | MyFlags::FlagB);
	MyBitmask c(MyFlags::FlagA);
	ASSERT_TRUE("test_equality", a == b);
	ASSERT_TRUE("test_equality", a != c);
	RETURN_TEST("test_equality", result);
}

// -------------------
// Modifiers
// -------------------

int test_add_sets_bits() {
	int result = 0;
	MyBitmask mask(MyFlags::FlagA);
	mask.Add(MyFlags::FlagD);
	ASSERT_EQUAL("test_add_sets_bits", MyFlags::FlagA | MyFlags::FlagD, mask.Value());
	mask.Add(MyFlags::FlagA);
	ASSERT_EQUAL("test_add_sets_bits", MyFlags::FlagA | MyFlags::FlagD, mask.Value());
	RETURN_TEST("test_add_sets_bits", result);
}

int test_remove_clears_bits() {
	int result = 0;
	MyBitmask mask(MyFlags::FlagA | MyFlags::FlagB | MyFlags::FlagC);
	mask.Remove(MyFlags::FlagB);
	ASSERT_EQUAL("test_remove_clears_bits", MyFlags::FlagA | MyFlags::FlagC, mask.Value());
	mask.Remove(MyFlags::FlagD);
	ASSERT_EQUAL("test_remove_clears_bits", MyFlags::FlagA | MyFlags::FlagC, mask.Value());
	RETURN_TEST("test_remove_clears_bits", result);
}

// -------------------
// Observers
// -------------------

int test_has_all_bits() {
	int result = 0;
	MyBitmask mask(MyFlags::FlagA | MyFlags::FlagB);
	ASSERT_TRUE("test_has_all_bits", mask.Has(MyFlags::FlagA));
	ASSERT_TRUE("test_has_all_bits", mask.Has(MyFlags::FlagA | MyFlags::FlagB));
	ASSERT_FALSE("test_has_all_bits", mask.Has(MyFlags::FlagA | MyFlags::FlagC));
	ASSERT_TRUE("test_has_all_bits", mask.Has(MyBitmask(MyFlags::FlagB)));
	ASSERT_FALSE("test_has_all_bits", mask.Has(MyBitmask(MyFlags::FlagC)));
	RETURN_TEST("test_has_all_bits", result);
}

int test_has_any_bits() {
	int result = 0;
	MyBitmask mask(MyFlags::FlagA);
	ASSERT_TRUE("test_has_any_bits", mask.HasAny(MyFlags::FlagA | MyFlags::FlagC));
	ASSERT_FALSE("test_has_any_bits", mask.HasAny(MyFlags::FlagB | MyFlags::FlagC));
	ASSERT_TRUE("test_has_any_bits", mask.HasAny(MyBitmask(MyFlags::FlagA | MyFlags::FlagD)));
	ASSERT_FALSE("test_has_any_bits", mask.HasAny(MyBitmask(MyFlags::FlagD)));
	RETURN_TEST("test_has_any_bits", result);
}

int test_has_none_bits() {
	int result = 0;
	MyBitmask mask(MyFlags::FlagA);
	ASSERT_TRUE("test_has_none_bits", mask.HasNone(MyFlags::FlagB | MyFlags::FlagC));
	ASSERT_FALSE("test_has_none_bits", mask.HasNone(MyFlags::FlagA | MyFlags::FlagB));
	ASSERT_TRUE("test_has_none_bits", mask.HasNone(MyBitmask(MyFlags::FlagD)));
	ASSERT_FALSE("test_has_none_bits", mask.HasNone(MyBitmask(MyFlags::FlagA)));
	RETURN_TEST("test_has_none_bits", result);
}

int main() {
	int result = 0;

	// -------------------
	// Construct
	// -------------------
	result += test_default_is_empty();
	result += test_construct_from_flag();
	result += test_construct_from_combined();
	result += test_copy_independent();

	// -------------------
	// Operators
	// -------------------
	result += test_or_union();
	result += test_and_intersection();
	result += test_xor_toggle();
	result += test_not_clears_flag();
	result += test_or_assign();
	result += test_and_assign();
	result += test_xor_assign();
	result += test_equality();

	// -------------------
	// Modifiers
	// -------------------
	result += test_add_sets_bits();
	result += test_remove_clears_bits();

	// -------------------
	// Observers
	// -------------------
	result += test_has_all_bits();
	result += test_has_any_bits();
	result += test_has_none_bits();

	if (result == 0)
		std::cout << "All tests passed!" << std::endl;
	else
		std::cout << result << " tests failed." << std::endl;
	return result;
}
