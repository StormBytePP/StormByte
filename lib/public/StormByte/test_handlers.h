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

#define RETURN_TEST(fn_name, fn_result) { \
	if (fn_result != 0) { \
		std::cerr << "Test " << fn_name << " FAILED!" << std::endl; \
	} \
	return fn_result; \
}

#define CurrentFileDirectory std::filesystem::path(__FILE__).parent_path()

#define ASSERT_EQUAL(fn_name, expected, actual) { \
	if ((expected) != (actual)) { \
		std::cerr << fn_name << ": Assertion failed at " << __FILE__ << ":" << __LINE__ << ": expected \"" << (expected) << "\", got \"" << (actual) << "\"" << std::endl; \
		return 1; \
	} \
}

#define ASSERT_NOT_EQUAL(fn_name, expected, actual) { \
	if ((expected) == (actual)) { \
		std::cerr << fn_name << ": Assertion failed at " << __FILE__ << ":" << __LINE__ << ": expected \"" << (expected) << "\", got \"" << (actual) << "\"" << std::endl; \
		return 1; \
	} \
}

#define ASSERT_FALSE(fn_name, condition) { \
	if ((condition)) { \
		std::cerr << fn_name << ": Assertion failed at " << __FILE__ << ":" << __LINE__ << ": condition is true, expected false" << std::endl; \
		return 1; \
	} \
}

#define ASSERT_TRUE(fn_name, condition) { \
	if (!(condition)) { \
		std::cerr << fn_name << ": Assertion failed at " << __FILE__ << ":" << __LINE__ << ": condition is false, expected true" << std::endl; \
		return 1; \
	} \
}
