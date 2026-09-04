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

#include <filesystem>
#include <iostream>

/**
 * @def RETURN_TEST
 * @brief Prints `FAILED` to `stderr` when @p fn_result is not `0`, then `return`s that value.
 * @param fn_name Test name written to the log.
 * @param fn_result Integer status (`0` = pass).
 */
#define RETURN_TEST(fn_name, fn_result) { \
	if (fn_result != 0) { \
		std::cerr << "Test " << fn_name << " FAILED!" << std::endl; \
	} \
	return fn_result; \
}

/**
 * @def CurrentFileDirectory
 * @brief Directory of the translation unit (`std::filesystem::path` of `__FILE__`).
 */
#define CurrentFileDirectory std::filesystem::path(__FILE__).parent_path()

/**
 * @def ASSERT_EQUAL
 * @brief Fails the test (`return 1`) when @p expected != @p actual.
 * @param fn_name Test name written to the log.
 * @param expected Expected value.
 * @param actual Observed value.
 */
#define ASSERT_EQUAL(fn_name, expected, actual) { \
	if ((expected) != (actual)) { \
		std::cerr << fn_name << ": Assertion failed at " << __FILE__ << ":" << __LINE__ << ": expected \"" << (expected) << "\", got \"" << (actual) << "\"" << std::endl; \
		return 1; \
	} \
}

/**
 * @def ASSERT_NOT_EQUAL
 * @brief Fails the test (`return 1`) when @p expected == @p actual.
 * @param fn_name Test name written to the log.
 * @param expected Value that must differ.
 * @param actual Observed value.
 */
#define ASSERT_NOT_EQUAL(fn_name, expected, actual) { \
	if ((expected) == (actual)) { \
		std::cerr << fn_name << ": Assertion failed at " << __FILE__ << ":" << __LINE__ << ": expected \"" << (expected) << "\", got \"" << (actual) << "\"" << std::endl; \
		return 1; \
	} \
}

/**
 * @def ASSERT_FALSE
 * @brief Fails the test (`return 1`) when @p condition is true.
 * @param fn_name Test name written to the log.
 * @param condition Expression that must be false.
 */
#define ASSERT_FALSE(fn_name, condition) { \
	if ((condition)) { \
		std::cerr << fn_name << ": Assertion failed at " << __FILE__ << ":" << __LINE__ << ": condition is true, expected false" << std::endl; \
		return 1; \
	} \
}

/**
 * @def ASSERT_TRUE
 * @brief Fails the test (`return 1`) when @p condition is false.
 * @param fn_name Test name written to the log.
 * @param condition Expression that must be true.
 */
#define ASSERT_TRUE(fn_name, condition) { \
	if (!(condition)) { \
		std::cerr << fn_name << ": Assertion failed at " << __FILE__ << ":" << __LINE__ << ": condition is false, expected true" << std::endl; \
		return 1; \
	} \
}
