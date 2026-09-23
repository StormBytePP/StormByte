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

#include <StormByte/test_handlers.h>

#include <array>
#include <stdexcept>
#include <string>

// -------------------
// Compare
// -------------------

int test_assert_equal() {
	int result = 0;
	const std::array<int, 3> expected{1, 2, 3};
	const std::array<int, 3> actual{1, 2, 3};
	ASSERT_EQUAL("test_assert_equal", expected, actual);
	ASSERT_EQUAL("test_assert_equal", 1, 1);
	RETURN_TEST("test_assert_equal", result);
}

int test_assert_not_equal() {
	int result = 0;
	ASSERT_NOT_EQUAL("test_assert_not_equal", 1, 2);
	RETURN_TEST("test_assert_not_equal", result);
}

int test_assert_true_false() {
	int result = 0;
	ASSERT_TRUE("test_assert_true_false", true);
	ASSERT_FALSE("test_assert_true_false", false);
	RETURN_TEST("test_assert_true_false", result);
}

// -------------------
// Exception
// -------------------

int test_assert_throws() {
	int result = 0;
	ASSERT_THROWS("test_assert_throws", throw std::runtime_error("expected"), std::runtime_error);
	RETURN_TEST("test_assert_throws", result);
}

int test_assert_no_throw() {
	int result = 0;
	ASSERT_NO_THROW("test_assert_no_throw", std::string("no throw"));
	RETURN_TEST("test_assert_no_throw", result);
}

// -------------------
// Value
// -------------------

int test_assert_near() {
	int result = 0;
	ASSERT_NEAR("test_assert_near", 1.0, 1.0001, 0.001);
	RETURN_TEST("test_assert_near", result);
}

int test_assert_contains() {
	int result = 0;
	ASSERT_CONTAINS("test_assert_contains", std::string("StormByte tests"), "Byte");
	RETURN_TEST("test_assert_contains", result);
}

int test_assert_not_null() {
	int result = 0;
	int value = 42;
	ASSERT_NOT_NULL("test_assert_not_null", &value);
	RETURN_TEST("test_assert_not_null", result);
}

int main() {
	int result = 0;

	// -------------------
	// Compare
	// -------------------
	result += test_assert_equal();
	result += test_assert_not_equal();
	result += test_assert_true_false();

	// -------------------
	// Exception
	// -------------------
	result += test_assert_throws();
	result += test_assert_no_throw();

	// -------------------
	// Value
	// -------------------
	result += test_assert_near();
	result += test_assert_contains();
	result += test_assert_not_null();

	if (result == 0)
		std::cout << "All tests passed!" << std::endl;
	else
		std::cout << result << " tests failed." << std::endl;
	return result;
}
