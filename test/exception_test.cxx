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
#include <StormByte/test_handlers.h>
#include <iostream>
#include <string>
#include <string_view>
using namespace StormByte;
// ---------------------------------------------------------------------------
// Regression: the removed zero-argument fast path called copy_str(fmt)
// directly on the std::format_string object, which does not compile once
// that branch is actually selected by overload resolution.
// ---------------------------------------------------------------------------
int test_plain_message_no_args() {
	int result = 0;
	Exception e("Key not found in Iterable::operator[]");
	ASSERT_EQUAL("test_plain_message_no_args", std::string("Key not found in Iterable::operator[]"), std::string(e.what()));
	RETURN_TEST("test_plain_message_no_args", result);
}
int test_formatted_message_with_args() {
	int result = 0;
	Exception e("value is {}", 42);
	ASSERT_EQUAL("test_formatted_message_with_args", std::string("value is 42"), std::string(e.what()));
	RETURN_TEST("test_formatted_message_with_args", result);
}
int test_zero_args_format_string_ctor_is_as_is() {
	// A bare string literal always binds to Exception(std::string&&), never to
	// the format_string<> template, so force that branch explicitly with a
	// constexpr string_view and confirm it still compiles and copies as-is
	// (no std::format escaping of doubled braces).
	int result = 0;
	constexpr std::string_view sv = "literal {{brace}} as-is";
	Exception e(sv);
	ASSERT_EQUAL("test_zero_args_format_string_ctor_is_as_is", std::string("literal {{brace}} as-is"), std::string(e.what()));
	RETURN_TEST("test_zero_args_format_string_ctor_is_as_is", result);
}
// ---------------------------------------------------------------------------
// Regression: Exception(component, fmt, args...) used to be silently hijacked
// by the fmt-only constructor (the component string was treated as the whole
// message and the real format string/args were discarded). The Component
// wrapper makes it the only viable overload for a component-prefixed call.
// ---------------------------------------------------------------------------
int test_component_prefixed_message() {
	int result = 0;
	Exception e(Component("MyComponent"), "failed with code {}", 7);
	ASSERT_EQUAL("test_component_prefixed_message", std::string("StormByte::MyComponent: failed with code 7"), std::string(e.what()));
	RETURN_TEST("test_component_prefixed_message", result);
}
int test_component_prefixed_message_no_args() {
	int result = 0;
	Exception e(Component("MyComponent"), "plain text");
	ASSERT_EQUAL("test_component_prefixed_message_no_args", std::string("StormByte::MyComponent: plain text"), std::string(e.what()));
	RETURN_TEST("test_component_prefixed_message_no_args", result);
}
int main() {
	int result = 0;
	result += test_plain_message_no_args();
	result += test_formatted_message_with_args();
	result += test_zero_args_format_string_ctor_is_as_is();
	result += test_component_prefixed_message();
	result += test_component_prefixed_message_no_args();
	if (result == 0) {
		std::cout << "All tests passed!" << std::endl;
	} else {
		std::cout << result << " tests failed." << std::endl;
	}
	return result;
}
