#include <StormByte/exception.hxx>
#include <StormByte/system.hxx>

#include <StormByte/test_handlers.h>

#include <chrono>
#include <iostream>

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

int main() {
	int result = 0;
	result += test_several_sleeps();

	if (result == 0) {
		std::cout << "All tests passed!" << std::endl;
	} else {
		std::cout << result << " tests failed." << std::endl;
	}
	return result;
}