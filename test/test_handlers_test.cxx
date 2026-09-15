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

int test_existing_assertions() {
	int result = 0;
	const std::array<int, 3> expected{1, 2, 3};
	const std::array<int, 3> actual{1, 2, 3};
	ASSERT_EQUAL("test_existing_assertions", expected, actual);
	ASSERT_NOT_EQUAL("test_existing_assertions", 1, 2);
	ASSERT_TRUE("test_existing_assertions", true);
	ASSERT_FALSE("test_existing_assertions", false);
	RETURN_TEST("test_existing_assertions", result);
}

int test_exception_assertions() {
	int result = 0;
	ASSERT_THROWS("test_exception_assertions", throw std::runtime_error("expected"), std::runtime_error);
	ASSERT_NO_THROW("test_exception_assertions", std::string("no throw"));
	RETURN_TEST("test_exception_assertions", result);
}

int test_value_assertions() {
	int result = 0;
	ASSERT_NEAR("test_value_assertions", 1.0, 1.0001, 0.001);
	ASSERT_CONTAINS("test_value_assertions", std::string("StormByte tests"), "Byte");
	int value = 42;
	ASSERT_NOT_NULL("test_value_assertions", &value);
	RETURN_TEST("test_value_assertions", result);
}

int main() {
	int result = 0;
	result += test_existing_assertions();
	result += test_exception_assertions();
	result += test_value_assertions();
	if (result == 0) {
		std::cout << "All tests passed!" << std::endl;
	} else {
		std::cout << result << " tests failed." << std::endl;
	}

	return result;
}
