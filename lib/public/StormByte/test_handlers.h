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

#pragma once

#include <exception>
#include <filesystem>
#include <iostream>
#include <ostream>
#include <string_view>

namespace StormByte::Test::Detail {
	template<typename T>
	void PrintValue(std::ostream& stream, const T& value) {
		if constexpr (requires { stream << value; }) {
			stream << value;
		} else {
			stream << "<unprintable>";
		}
	}
}

/**
 * @def RETURN_TEST
 * @brief Prints `FAILED` to `stderr` when @p fn_result is not `0`, then `return`s that value.
 * @param fn_name Test name written to the log.
 * @param fn_result Integer status (`0` = pass).
 */
#define RETURN_TEST(fn_name, fn_result) do { \
	const int stormbyte_test_result = (fn_result); \
	if (stormbyte_test_result != 0) { \
		std::cerr << "Test " << fn_name << " FAILED!" << std::endl; \
	} \
	return stormbyte_test_result; \
} while (false)

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
#define ASSERT_EQUAL(fn_name, expected, actual) do { \
	const auto& stormbyte_expected = (expected); \
	const auto& stormbyte_actual = (actual); \
	if (!(stormbyte_expected == stormbyte_actual)) { \
		std::cerr << fn_name << ": Assertion failed at " << __FILE__ << ":" << __LINE__ << ": expected \""; \
		::StormByte::Test::Detail::PrintValue(std::cerr, stormbyte_expected); \
		std::cerr << "\", got \""; \
		::StormByte::Test::Detail::PrintValue(std::cerr, stormbyte_actual); \
		std::cerr << "\"" << std::endl; \
		return 1; \
	} \
} while (false)

/**
 * @def ASSERT_NOT_EQUAL
 * @brief Fails the test (`return 1`) when @p expected == @p actual.
 * @param fn_name Test name written to the log.
 * @param expected Value that must differ.
 * @param actual Observed value.
 */
#define ASSERT_NOT_EQUAL(fn_name, expected, actual) do { \
	const auto& stormbyte_expected = (expected); \
	const auto& stormbyte_actual = (actual); \
	if (stormbyte_expected == stormbyte_actual) { \
		std::cerr << fn_name << ": Assertion failed at " << __FILE__ << ":" << __LINE__ << ": values should differ; both were \""; \
		::StormByte::Test::Detail::PrintValue(std::cerr, stormbyte_actual); \
		std::cerr << "\"" << std::endl; \
		return 1; \
	} \
} while (false)

/**
 * @def ASSERT_FALSE
 * @brief Fails the test (`return 1`) when @p condition is true.
 * @param fn_name Test name written to the log.
 * @param condition Expression that must be false.
 */
#define ASSERT_FALSE(fn_name, condition) do { \
	if ((condition)) { \
		std::cerr << fn_name << ": Assertion failed at " << __FILE__ << ":" << __LINE__ << ": condition is true, expected false" << std::endl; \
		return 1; \
	} \
} while (false)

/**
 * @def ASSERT_TRUE
 * @brief Fails the test (`return 1`) when @p condition is false.
 * @param fn_name Test name written to the log.
 * @param condition Expression that must be true.
 */
#define ASSERT_TRUE(fn_name, condition) do { \
	if (!(condition)) { \
		std::cerr << fn_name << ": Assertion failed at " << __FILE__ << ":" << __LINE__ << ": condition is false, expected true" << std::endl; \
		return 1; \
	} \
} while (false)

/**
 * @def ASSERT_THROWS
 * @brief Fails when @p expression does not throw @p exception_type.
 */
#define ASSERT_THROWS(fn_name, expression, exception_type) do { \
	bool stormbyte_threw = false; \
	try { \
		(void)(expression); \
	} catch (const exception_type&) { \
		stormbyte_threw = true; \
	} catch (...) { \
	} \
	if (!stormbyte_threw) { \
		std::cerr << fn_name << ": Assertion failed at " << __FILE__ << ":" << __LINE__ << ": expected " << #exception_type << " to be thrown" << std::endl; \
		return 1; \
	} \
} while (false)

/**
 * @def ASSERT_NO_THROW
 * @brief Fails when @p expression throws any exception.
 */
#define ASSERT_NO_THROW(fn_name, expression) do { \
	try { \
		(void)(expression); \
	} catch (const std::exception& stormbyte_exception) { \
		std::cerr << fn_name << ": Assertion failed at " << __FILE__ << ":" << __LINE__ << ": unexpected exception: " << stormbyte_exception.what() << std::endl; \
		return 1; \
	} catch (...) { \
		std::cerr << fn_name << ": Assertion failed at " << __FILE__ << ":" << __LINE__ << ": unexpected non-standard exception" << std::endl; \
		return 1; \
	} \
} while (false)

/**
 * @def ASSERT_NEAR
 * @brief Fails when two arithmetic values differ by more than @p tolerance.
 */
#define ASSERT_NEAR(fn_name, expected, actual, tolerance) do { \
	const auto stormbyte_expected = (expected); \
	const auto stormbyte_actual = (actual); \
	const auto stormbyte_tolerance = (tolerance); \
	const auto stormbyte_difference = stormbyte_expected > stormbyte_actual ? \
		stormbyte_expected - stormbyte_actual : stormbyte_actual - stormbyte_expected; \
	if (stormbyte_difference > stormbyte_tolerance) { \
		std::cerr << fn_name << ": Assertion failed at " << __FILE__ << ":" << __LINE__ << ": values differ beyond tolerance" << std::endl; \
		return 1; \
	} \
} while (false)

/**
 * @def ASSERT_CONTAINS
 * @brief Fails when @p haystack does not contain @p needle.
 */
#define ASSERT_CONTAINS(fn_name, haystack, needle) do { \
	const auto& stormbyte_haystack = (haystack); \
	const auto& stormbyte_needle = (needle); \
	if (stormbyte_haystack.find(stormbyte_needle) == std::string_view::npos) { \
		std::cerr << fn_name << ": Assertion failed at " << __FILE__ << ":" << __LINE__ << ": expected text to contain \"" << stormbyte_needle << "\"" << std::endl; \
		return 1; \
	} \
} while (false)

/**
 * @def ASSERT_NOT_NULL
 * @brief Fails when @p pointer is null.
 */
#define ASSERT_NOT_NULL(fn_name, pointer) do { \
	if ((pointer) == nullptr) { \
		std::cerr << fn_name << ": Assertion failed at " << __FILE__ << ":" << __LINE__ << ": pointer is null" << std::endl; \
		return 1; \
	} \
} while (false)
