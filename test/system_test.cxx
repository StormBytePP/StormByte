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

#include <StormByte/exception.hxx>
#include <StormByte/system.hxx>
#include <StormByte/test_handlers.h>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>
using namespace StormByte::System;
int test_several_sleeps() {
	// CI + ASAN can stretch sleeps a lot; allow a wide but still meaningful window.
	constexpr int min_ms = 80;    // must have slept roughly the requested time
	constexpr int max_ms = 2000;  // reject pathological hangs
	int result = 0;
	try {
		auto start = std::chrono::steady_clock::now();
		Sleep(std::chrono::milliseconds(100));
		auto end = std::chrono::steady_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		ASSERT_TRUE("test_several_sleeps", duration >= min_ms && duration <= max_ms);
		start = std::chrono::steady_clock::now();
		Sleep(std::chrono::seconds(1));
		end = std::chrono::steady_clock::now();
		duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		// Expect ~1s: at least 800ms, at most a few seconds under CI load
		ASSERT_TRUE("test_several_sleeps", duration >= 800 && duration <= 5000);
	} catch (const StormByte::Exception& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}

	RETURN_TEST("test_several_sleeps", result);
}

int test_temp_file_name_long_prefix_does_not_throw() {
	int result = 0;
	try {
		const std::string long_prefix(247, 'a');
		auto path = TempFileName(long_prefix);
		ASSERT_TRUE("test_temp_file_name_long_prefix_does_not_throw", std::filesystem::exists(path));
		std::filesystem::remove(path);
	} catch (const StormByte::SystemError& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}

	RETURN_TEST("test_temp_file_name_long_prefix_does_not_throw", result);
}

int test_executable_path_resolves() {
	int result = 0;
	try {
		auto path = ExecutablePath();
		ASSERT_TRUE("test_executable_path_resolves", path != std::filesystem::path("NOPATH"));
		ASSERT_TRUE("test_executable_path_resolves", std::filesystem::is_directory(path));
	} catch (const StormByte::SystemError& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}

	RETURN_TEST("test_executable_path_resolves", result);
}

int main() {
	int result = 0;
	result += test_several_sleeps();
	result += test_temp_file_name_long_prefix_does_not_throw();
	result += test_executable_path_resolves();

	if (result == 0) {
		std::cout << "All tests passed!" << std::endl;
	} else {
		std::cout << result << " tests failed." << std::endl;
	}

	return result;
}
